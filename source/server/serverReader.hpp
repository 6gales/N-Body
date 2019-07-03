#ifndef SCHOOL_SERVER_SERVERREADER_HPP
#define SCHOOL_SERVER_SERVERREADER_HPP

#include <boost/asio.hpp>

class ServerReader {
private:
    boost::asio::streambuf& buf;
    bool isWasCommand = false;
    bool isStart = false;
    bool isWasCount = false;
    size_t count = 0;

public:
    explicit ServerReader(boost::asio::streambuf& buf) : buf(buf) {}

    size_t check_count_objects(const boost::system::error_code &err_code, size_t bytes);
};

#endif //SCHOOL_SERVER_SERVERREADER_HPP
