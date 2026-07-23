#include <iostream>
#include "../include/trixy_core_includes.hpp"

using namespace trixy_core;


int trixy_main(int argc, char* argv[]) {
    try {
        auto parsed_console = console::parsed_console(argc, argv);

        std::cout << static_cast<int>(parsed_console.command) << " " << parsed_console.args.at("-t") << std::endl;
    }
    catch (const console::console_exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}