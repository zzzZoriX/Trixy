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
    parsed_console(const int argc, char* argv[]);

    bool is_executable_command() const;

private:
    bool is_available_command(const std::string_view cmd) const;
    bool is_available_sub_command(const std::string_view scmd) const;
    bool is_breakable_command(const std::string_view cmd) const;
    console::command define_command_enum(const std::string_view cmd) const;
    void load_args_by_sub_command(const std::string_view scmd);
    std::string convert_long_to_short(const std::string_view arg) const;
};

exception_template(console_exception);


void commands_handler(const parsed_console& console);

}