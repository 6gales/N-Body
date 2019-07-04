#include <iostream>
#include "clientReader.hpp"

size_t ClientReader::check_count_objects(const boost::system::error_code &err_code, size_t bytes) {
    if (bytes > 8 && !isWasCount) {
        std::string msg{boost::asio::buffers_begin(buf.data()), boost::asio::buffers_end(buf.data())};
        count = ((size_t)msg.at(0) << 56) | ((size_t)msg.at(1) << 48) | ((size_t)msg.at(2) << 40) | ((size_t)msg.at(3) << 32)
                | ((size_t)msg.at(4) << 24) | ((size_t)msg.at(5) << 16) | ((size_t)msg.at(6) << 8) | (msg.at(7));
        isWasCount = true;
        if (count == 0) return 0;
    }
    if (bytes >= (sizeof(float)*4*count + sizeof(size_t)) && isWasCount) return 0;
    return 1;
}
