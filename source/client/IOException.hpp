#pragma once

#include <exception>
#include <string>

class IOException : public std::exception {
public:
    IOException() noexcept = default;

    IOException(const IOException& ex) noexcept : error_msg(ex.error_msg) { }

    const char *what() const noexcept override {
        return error_msg.c_str();
    }

    ~IOException() noexcept override = default;

    explicit IOException(std::string  errorMsg) : error_msg(std::move(errorMsg)) {}

private:
    const std::string error_msg;
};