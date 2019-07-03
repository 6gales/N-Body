#ifndef SCHOOL_SERVER_CLIENTREADER_HPP
#define SCHOOL_SERVER_CLIENTREADER_HPP
#include <boost/asio.hpp>

class ClientReader {
private:
    boost::asio::streambuf& buf;
    bool isWasCount = false;
    size_t count = 0;

public:
    explicit ClientReader(boost::asio::streambuf& buf) : buf(buf) {}

    size_t check_count_objects(const boost::system::error_code &err_code, size_t bytes);
};
#endif //SCHOOL_SERVER_CLIENTREADER_HPP
