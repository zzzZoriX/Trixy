#include "../include/trixy_ping.hpp"
#include "../../include/trixy_network.hpp"
#include <string>
#include <string_view>
#include <fstream>


using namespace network::ping;

void pinger::init_sl(const std::string_view pslf) {
    std::ifstream pslfp(pslf.data());

    if (!pslfp.is_open()) throw network_exception(
        std::string("Can't open the ping servers list file\n") +
        "Create this file if it not exist"
    );

    std::string line;

    while (std::getline(pslfp, line)) if (validate_host(line))
        sl.push_back("https://" + line);
}

bool validate_host(const std::string_view host) {
    return std::regex_match(host.data(), std::regex(R"www[.]([a-zA-Z0-9-_.]+)[.]com"));
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
}