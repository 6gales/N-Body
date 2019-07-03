#include "clientReader.h"

size_t ClientReader::check_count_objects(const boost::system::error_code &err_code, size_t bytes) {
    std::string msg{boost::asio::buffers_begin(buf.data()), boost::asio::buffers_end(buf.data())};

    if (bytes + offset > 8 && !isWasCount) {
        count = ((size_t)msg.at(0) << 56) | ((size_t)msg.at(1) << 48) | ((size_t)msg.at(2) << 40) | ((size_t)msg.at(3) << 32)
                | ((size_t)msg.at(4) << 24) | ((size_t)msg.at(5) << 16) | ((size_t)msg.at(6) << 8) | (msg.at(7));
        isWasCount = true;
        if (count == 0) return 0;
    }
    if (bytes + offset >= sizeof(float)*7*count) return 0;
    offset += bytes;
    return 1;
}
