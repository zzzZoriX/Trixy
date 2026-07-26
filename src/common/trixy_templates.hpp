#pragma once

namespace common {

#define exception_template(excn) class excn : public std::exception {\
    std::string err; \
public: \
    excn(const std::string& e) noexcept: err(e) {} \
    const char* what(void) const noexcept override { return err.c_str(); } \
}


#define USAGE_MSG_TEMPLATE  "|> Trixy version: v0.0.1\n|\n" \
                            "|> How to use:\n" \
                            "|:   trixy <command> <sub-command> <args>...\n" \
                            "|> Example:\n" \
                            "|:   trixy go check -ito \"ignore_this_servers.txt\"\n|\n" \
                            "|> Available commands: \n" \
                            "|:   go     -- start some trixy service\n" \
                            "|:   get    -- get some trixy info\n" \
                            "|:   set    -- set some trixy info\n" \
                            "|:   wht    -- print support info about trixy command or this msg\n" \
                            "|:   stop   -- stop trixy\n|\n" \
                            "|> for info about command type: trixy wht / \"<command>\""

}