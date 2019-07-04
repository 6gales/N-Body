#include <iostream>
#include "clientReader.hpp"

using ull = unsigned long long;

size_t ClientReader::check_count_objects(const boost::system::error_code &err_code, size_t bytes) {
    if (bytes > 8 && !isWasCount) {
        std::string msg{boost::asio::buffers_begin(buf.data()), boost::asio::buffers_end(buf.data())};
        count = (((ull)msg.at(0) << 56) & 0xFF00000000000000) | (((ull)msg.at(1) << 48) & 0x00FF000000000000) | (((ull)msg.at(2) << 40) & 0x0000FF0000000000)
                  | (((ull)msg.at(3) << 32) & 0x000000FF00000000) | (((ull)msg.at(4) << 24) & 0x00000000FF000000) | (((ull)msg.at(5) << 16) & 0x0000000000FF0000)
                  | (((ull)msg.at(6) << 8) & 0x000000000000FF00) | ((ull)msg.at(7) & 0x00000000000000FF);
        isWasCount = true;
        if (count == 0) return 0;
    }
    if (bytes >= (sizeof(float)*4*count + sizeof(ull)) && isWasCount) return 0;
    return 1;
}
