#include "../include/trixy_ping.hpp"
#include "../../include/trixy_network.hpp"
#include <string>
#include <string_view>
#include <fstream>
#include <numeric>
#include <algorithm>


using namespace network::ping;

void pinger::init_sl(const std::string_view pslf) {
    std::ifstream pslfp(pslf.data());

    if (!pslfp.is_open()) throw network_exception(
        std::string("Can't open the ping servers list file\n") + "Create this file if it not exist"
    );

    std::string line;

    while (std::getline(pslfp, line)) if (validate_host(line))
        sl.push_back("https://" + line);
}

bool network::ping::validate_host(const std::string_view host) {
    return std::regex_match(host.data(), std::regex("(www[.])?([a-zA-Z0-9-_.]+)[.]com"));
}

pinger::ping_list pinger::ping_server_list() {
    ping_list results;

    for (const auto& server: sl) results.emplace_back(ping_server(server));

    return results;
}


server_ping_result pinger::ping_server(const std::string_view host) {
    current_result = server_ping_result();

    current_result.host = host;
    current_result.packets_sent = max_packets;

    rtt_list.clear();

    curr_seq = 0;
    replies_count = 0;

    try {
        io_context ioc;

        socket = std::make_unique<ip::icmp::socket>(ioc, ip::icmp::v4());
        resolver = std::make_unique<ip::icmp::resolver>(ioc);
        timer = std::make_unique<steady_timer>(ioc);

        dest = *resolver->resolve(ip::icmp::v4(), current_result.host, "").begin();

        current_result.ip_addr = dest.address().to_string();

        ioc.run();
    } catch (const std::exception& e) {
        current_result.err_msg = std::string("Network/DNS error: ") + e.what();
        current_result.is_reachable = false;
    }

    current_result.packet_loss_percent = (max_packets - replies_count) / max_packets * 100.0;

    if (rtt_list.empty()) {
        current_result.is_reachable = false;

        if (current_result.err_msg.empty()) current_result.err_msg = "Timeout. 100% packet loss";
    } else {
        current_result.is_reachable = true;

        current_result.max_ping = *std::ranges::max_element(rtt_list);
        current_result.min_ping = *std::ranges::min_element(rtt_list);

        current_result.avg_ping = std::accumulate(rtt_list.begin(), rtt_list.end(), 0) / rtt_list.size();
    }

    return current_result;
}


void pinger::async_send() {
    if (!current_result.err_msg.empty()) return;

    const std::string    payload(payload_size, 'E');
    std::vector<uint8_t> buffer(sizeof(icmp_header) + payload.size());

    const auto header = reinterpret_cast<icmp_header*>(buffer.data());
    header->type = 8;
    header->code = 0;
    header->checksum = 0;
    header->id = htons(pid);
    header->seq_num = htons(++curr_seq);

    std::memcpy(buffer.data() + sizeof(icmp_header), payload.data(), payload.size());

    header->checksum = icmp_header::calculate_checksum(buffer.data(), buffer.size());

    send_time = std::chrono::steady_clock::now();

    socket->send_to(boost::asio::buffer(buffer), dest);

    timer->expires_after(std::chrono::milliseconds(timeout_ms));
    timer->async_wait([this](const boost::system::error_code& ec) {
        if (!ec) {
            if (curr_seq < max_packets) async_send();
            else socket->close();
        }
    });
}

void pinger::async_receive() {
    reply_buf.consume(reply_buf.size());

    socket->async_receive_from(reply_buf.prepare(65536), sender,
                                [this](const boost::system::error_code& ec, std::size_t bytes_recvd) {
                                    if (!ec) {
                                        reply_buf.commit(bytes_recvd);
                                        handle_receive(bytes_recvd);
                                    }
                                });
}

void pinger::handle_receive(const size_t length) {
    const auto receive_time = std::chrono::steady_clock::now();
    timer->cancel();

    const auto buffer = static_cast<const uint8_t*>(reply_buf.data().data());

    if (const size_t ip_header_len = (buffer[0] & 0x0F) * 4; length >= ip_header_len + sizeof(icmp_header)) {
        if (auto header = reinterpret_cast<const icmp_header*>(buffer + ip_header_len); header->type == 0 && ntohs(header->id) == pid) {
            const auto rtt = std::chrono::duration_cast<std::chrono::microseconds>(receive_time - send_time).count() / 1000.0;

            rtt_list.push_back(rtt);
            replies_count++;
        }
    }

    if (curr_seq < max_packets) {
        timer->expires_after(std::chrono::seconds(1));
        timer->async_wait([this](const boost::system::error_code& ec) {
            if (!ec) {
                async_send();
                async_receive();
            }
        });
    } else {
        socket->close();
    }
}
