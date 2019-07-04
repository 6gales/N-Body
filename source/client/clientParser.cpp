#include <string>
#include <iostream>
#include <cstring>
#include <vector>
#include "../computer/abstractComputer.h"
#include "IOException.hpp"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <bitset>

using ull = unsigned long long;

std::string parse_line(const std::string& line) {
    std::string part_msg{};

    std::vector<std::string> numbers_vector;
    split(numbers_vector, line, boost::is_any_of(" \n"), boost::token_compress_on);
    for (auto &str_numb : numbers_vector) {
        const auto numb = static_cast<const float>(std::stod(str_numb));
        const char* p_numb = reinterpret_cast<const char*>(&numb);
        part_msg.append(p_numb, 4);
    }

    return part_msg;
}

ull parse_file(std::istream &data_file, std::string &start_message) {
    start_message.append("START", 5);
    std::string count_str;
    std::getline(data_file, count_str);
    ull count = std::stoull(count_str);
    char byte_count[8];
    for (size_t i = 0; i < 8; ++i) {
        byte_count[i] = (char)(count >> ((7-i)*8));
    }
    start_message.append(byte_count, 8);
    for (ull j = 0; j < count; ++j) {
        std::string line;
        std::getline(data_file, line);
        std::string part_msg = parse_line(line);
        if (data_file.bad()) {
            throw IOException{"data file error"};
        }

        start_message.append(part_msg);
    }
    return count;
}

std::vector<Particle> parse_msg_from_server(const std::string &msg) {
    ull count_particles = (((ull)msg.at(0) << 56) & 0xFF00000000000000) | (((ull)msg.at(1) << 48) & 0x00FF000000000000) | (((ull)msg.at(2) << 40) & 0x0000FF0000000000)
            | (((ull)msg.at(3) << 32) & 0x000000FF00000000) | (((ull)msg.at(4) << 24) & 0x00000000FF000000) | (((ull)msg.at(5) << 16) & 0x0000000000FF0000)
            | (((ull)msg.at(6) << 8) & 0x000000000000FF00) | ((ull)msg.at(7) & 0x00000000000000FF);
    std::string particles_info = msg.substr(8);
    std::vector<Particle> particles{count_particles};
    for (ull i = 0; i < count_particles; ++i) {
        float mass, x, y, z;
        memcpy(&mass, particles_info.c_str() + (i*4 * 4), sizeof(float));
        memcpy(&x, particles_info.c_str() + ((i*4 + 1) * 4), sizeof(float));
        memcpy(&y, particles_info.c_str() + ((i*4 + 2) * 4), sizeof(float));
        memcpy(&z, particles_info.c_str() + ((i*4 + 3) * 4), sizeof(float));
        particles[i] = {mass, x, y, z, 0, 0, 0};
    }

    return particles;
}