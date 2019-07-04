#pragma once

#include <boost/asio.hpp>

class ClientReader {
private:
    boost::asio::streambuf& buf;
    bool isWasCount = false;
    unsigned long long count = 0;

public:
    explicit ClientReader(boost::asio::streambuf& buf) : buf(buf) {}

    size_t check_count_objects(const boost::system::error_code &err_code, size_t bytes);
};