#include <iostream>
#include "../include/trixy_core_includes.hpp"

int trixy_main(int argc, char* argv[]) {
    try {

    }
    catch (const trixy_core::console::console_exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}