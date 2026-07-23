#pragma once

namespace common {

#define exception_template(excn) class excn : public std::exception {\
    std::string err; \
public: \
    excn(const std::string& e) noexcept: err(e) {} \
    const char* what(void) const noexcept override { return err.c_str(); } \
}

}