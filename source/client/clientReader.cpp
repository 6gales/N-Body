#include "clientReader.h"

size_t ClientReader::check_count_objects(const boost::system::error_code &err_code, size_t bytes) {
    std::string msg{boost::asio::buffers_begin(buf.data()), boost::asio::buffers_end(buf.data())};

    if (bytes + offset > 13 && !isWasCount) {
        count = ((size_t)msg.at(5) << 56) | ((size_t)msg.at(6) << 48) | ((size_t)msg.at(7) << 40) | ((size_t)msg.at(8) << 32)
                | ((size_t)msg.at(9) << 24) | ((size_t)msg.at(10) << 16) | ((size_t)msg.at(11) << 8) | (msg.at(12));
        isWasCount = true;
        if (count == 0) return 0;
    }
    if (bytes + offset >= sizeof(float)*7*count) return 0;
    offset += bytes;
    return 1;
}
