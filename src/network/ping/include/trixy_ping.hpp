#pragma once

#include <vector>
#include <string_view>
#include <string>
#include <regex>
#include <boost/asio.hpp>
#include <chrono>

#include "../../../common/trixy_types.hpp"


using namespace trixy_common;
using namespace boost::asio;


namespace network::ping {

struct server_ping_result {
    std::string ip_addr, host, err_msg;
    size_t packets_sent;
    double packet_loss_percent;
    ping_t min_ping, max_ping, avg_ping;
    bool is_reachable;
};

struct icmp_header {
    ubyte type, code;
    uint16_t checksum, id, seq_num;

    static uint16_t calculate_checksum(const uint8_t* data, size_t length) {
        uint32_t sum = 0;
        auto ptr = reinterpret_cast<const uint16_t*>(data);

        while (length > 1) {
            sum += *ptr++;
            length -= 2;
        }

        if (length > 0) {
            sum += *reinterpret_cast<const uint8_t*>(ptr);
        }

        while (sum >> 16) {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }

        return static_cast<uint16_t>(~sum);
    }
};

class pinger {
using servers_list = std::vector<std::string>;

    servers_list sl;

    size_t timeout_ms, payload_size;
    uint16_t pid;

    server_ping_result current_result;
    size_t curr_seq, replies_count, max_packets = 4;

    std::vector<double> rtt_list;
    std::chrono::steady_clock::time_point send_time;

    std::unique_ptr<ip::icmp::socket> socket;
    std::unique_ptr<ip::icmp::resolver> resolver;
    std::unique_ptr<steady_timer> timer;
    ip::icmp::endpoint dest, sender;

    streambuf reply_buf;


    void init_sl(const std::string_view pslf);

    server_ping_result ping_server(const std::string_view host);

    void async_send();
    void async_receive();
    void handle_receive(const size_t length);

public:
using ping_list = std::vector<server_ping_result>;

    pinger(const size_t timeout, const size_t payload_size, const std::string_view pslf = "config/pslf.txt"):
        timeout_ms(timeout), payload_size(payload_size)
    {
        pid = static_cast<uint16_t>(GetCurrentProcessId());

        init_sl(pslf);

        // todo: add config loading
    }


    ping_list ping_server_list();
};

bool validate_host(const std::string_view host);

}