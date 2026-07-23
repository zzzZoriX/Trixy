#include "../include/trixy_console.hpp"
#include <regex>

using namespace trixy_core::console;

parsed_console::parsed_console(const int argc, char* argv[]) {
    if (argc < 2) throw console_exception(
        std::string("Too few arguments") +
        "\nExample of correct usage: trixy go ping"
    );

    command = command::NONE;

    for (int i = 1; i < argc; ++i) {
        const std::string arg{argv[i]};

        if (i == 1) {
            if (!is_available_command(arg)) throw console_exception(
                std::string("Unexpected argument: ") +
                arg +
                "\nType \"trixy wht ?\" for info about usage Trixy"
            );

            command = define_command_enum(arg);

            if (is_breakable_command(arg)) break;

            continue;
        }

        if (i == 2) {
            if (!is_available_sub_command(arg)) throw console_exception(
                std::string("Unexpected argument: ") +
                arg +
                "\nType \"trixy wht ?\" for info about usage Trixy"
            );

            sub_command = arg;

            load_args_by_sub_command(arg);

            continue;
        }

        if (args.contains(convert_long_to_short(arg))) {
            if (++i == argc) throw console_exception(
                "Expected argument but command have not it"
            );

            std::string value(argv[i]);

            if (!std::regex_match(value, std::regex(R"(^["].*?["]$)"))) throw console_exception(
                "Expected argument value but not given"
            );

            args[convert_long_to_short(arg)] = value;
        }
        else throw console_exception(
            std::string("Unexpected argument: ") +
            arg +
            "\nType \"trixy wht ?\" for info about usage Trixy"
        );
    }
}

bool parsed_console::is_available_command(const std::string_view cmd) {
    return  cmd == "go" ||
            cmd == "set" ||
            cmd == "get" ||
            cmd == "wht" ||
            cmd == "stop";
}

bool parsed_console::is_available_sub_command(const std::string_view scmd) {
    return  scmd == "check" ||
            scmd == "ping";
}

bool parsed_console::is_breakable_command(const std::string_view cmd) {
    return cmd == "stop";
}

command parsed_console::define_command_enum(const std::string_view cmd) {
    if (cmd == "go") return command::GO;
    if (cmd == "set") return command::SET;
    if (cmd == "get") return command::GET;
    if (cmd == "wht") return command::WHT;
    if (cmd == "stop") return command::STOP;

    return command::NONE;
}

void parsed_console::load_args_by_sub_command(const std::string_view scmd) {
    if (scmd == "check") {
        args = {{"-ito", ""}, {"-t", ""}};
    }
}

std::string parsed_console::convert_long_to_short(const std::string_view arg) {
    if (arg == "-ito" || arg == "-t") return arg.data();

    if (arg == "--instead-of") return "-ito";
    if (arg == "--timer") return "-t";

    return "";
}
