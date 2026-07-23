#pragma once

#include <string_view>
#include <string>
#include <exception>
#include <map>
#include "../../common/trixy_templates.hpp"


namespace trixy_core::console {

using arguments = std::map<std::string, std::string>;

enum class command {
    NONE,
    GO,
    SET,
    GET,
    WHT,
    STOP
};

struct parsed_console {
    command command;
    std::string sub_command;
    arguments args;

    parsed_console() = delete;
    parsed_console(const int argc, const char* argv[]);

private:
    bool is_available_command(const std::string_view cmd);
    bool is_available_sub_command(const std::string_view scmd);
    bool is_breakable_command(const std::string_view cmd);
    console::command define_command_enum(const std::string_view cmd);
    void load_args_by_sub_command(const std::string_view scmd);
    std::string convert_long_to_short(const std::string_view arg);
};

exception_template(console_exception);

}