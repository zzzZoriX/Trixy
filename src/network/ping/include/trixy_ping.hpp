#pragma once

#include <vector>
#include <string_view>
#include <string>
#include <fstream>
#include <regex>
#include <boost/asio.hpp>
#include <chrono>

#include "../../../common/trixy_types.hpp"
#include "../../include/trixy_network.hpp"

using namespace trixy_common;
using namespace boost::asio;


namespace network::ping {

struct server_ping_result {
    std::string ip_addr, host;
    double packet_loss_percent;
    ping_t min_ping, max_ping, avg_ping;
    bool is_reachable;
};

struct icmp_header {
    ubyte type, code;
    uint16_t checksum, id, seq_num;
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


    void init_sl(const std::string_view pslf);

    void ping_server(const std::string_view host);
    void finalize_ping_result();

public:
using ping_list = std::vector<server_ping_result>;

    pinger(const size_t timeout, const size_t payload_size, const std::string_view pslf = "../../../../config/pslf.txt") noexcept:
        timeout_ms(timeout), payload_size(payload_size)
    {
        pid = static_cast<uint16_t>(GetCurrentProcessId());

        init_sl(pslf);

        // todo: add config loading
    }


    ping_list ping_server_list();
};

}