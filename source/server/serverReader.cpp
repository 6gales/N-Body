#include <iostream>
#include "serverReader.hpp"

using ull = unsigned long long;

size_t ServerReader::check_count_objects(const boost::system::error_code &err_code, size_t bytes) {
    if (bytes == 5 && !isWasCommand) {
        std::string msg{boost::asio::buffers_begin(buf.data()), boost::asio::buffers_end(buf.data())};
        isWasCommand = true;
        if (!msg.compare(0, 5, "START")) {
            isStart = true;
        } else {
            return 0;
        }
    }
    if (bytes == 13 && isStart && !isWasCount) {
        std::string msg{boost::asio::buffers_begin(buf.data()), boost::asio::buffers_end(buf.data())};
        count = (((ull)msg.at(5) << 56) & 0xFF00000000000000) | (((ull)msg.at(6) << 48) & 0x00FF000000000000) | (((ull)msg.at(7) << 40) & 0x0000FF0000000000)
                  | (((ull)msg.at(8) << 32) & 0x000000FF00000000) | (((ull)msg.at(9) << 24) & 0x00000000FF000000) | (((ull)msg.at(10) << 16) & 0x0000000000FF0000)
                  | (((ull)msg.at(11) << 8) & 0x000000000000FF00) | ((ull)msg.at(12) & 0x00000000000000FF);
        isWasCount = true;
        if (count == 0) return 0;
    }
    if (bytes == (sizeof(float)*7*count + 5 + sizeof(ull)) && isWasCount) {
        return 0;
    }
    return 1;
}