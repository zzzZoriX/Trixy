#include <iostream>
#include "../include/trixy_core_includes.hpp"

using namespace trixy_core;


int trixy_main(int argc, char* argv[]) {
    try {
        auto parsed_console = console::parsed_console(argc, argv);

        network::ping::pinger pinger(2000, 32);

        for (const auto pl = pinger.ping_server_list(); const auto p: pl) {
            std::cout << p.host << " | " << p.ip_addr << std::boolalpha << " ? " << p.is_reachable <<
                std::endl << p.avg_ping << " / " << p.max_ping << " / " << p.min_ping << " || " << p.packets_sent << " , " << p.packet_loss_percent << std::endl <<
                p.err_msg << std::endl;
        }
    }
    catch (const console::console_exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}