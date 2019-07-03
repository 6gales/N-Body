#ifndef SCHOOL_SERVER_CLIENTREADER_H
#define SCHOOL_SERVER_CLIENTREADER_H
#include <boost/asio.hpp>

class ClientReader {
private:
    boost::asio::streambuf& buf;
    size_t offset = 0;
    bool isWasCount = false;
    size_t count = 0;

public:
    explicit ClientReader(boost::asio::streambuf& buf) : buf(buf) {}

    size_t check_count_objects(const boost::system::error_code &err_code, size_t bytes);
};
#endif //SCHOOL_SERVER_CLIENTREADER_H
