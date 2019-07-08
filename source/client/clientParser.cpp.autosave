#include "clientParser.hpp"
#include "IOException.hpp"
#include <cstring>

ull parse_file(std::istream &data_file, std::string &start_message, std::vector<float> &particles_mass) {
    start_message.append("START", 5);
    std::string count_str;
    std::getline(data_file, count_str);
    ull count = std::stoull(count_str);
    particles_mass.resize(count);
    char byte_count[sizeof(ull)];
    for (size_t i = 0; i < sizeof(ull); ++i) {
        byte_count[i] = (char)(count >> ((sizeof(ull)-i-1)*sizeof(ull)));
    }
    start_message.append(byte_count, sizeof(ull));
    for (ull j = 0; j < count; ++j) {
        float datas[7];
        data_file >> datas[0] >> datas[1] >> datas[2] >> datas[3] >> datas[4] >> datas[5] >> datas[6];
        for (size_t i = 0; i < 7; ++i) {
            const char* p_numb = reinterpret_cast<const char*>(datas+i);
            start_message.append(p_numb, sizeof(float));
        }
        particles_mass[j] = datas[0];
        if (data_file.bad()) {
            throw IOException{"data file error"};
        }

    }
    return count;
}

std::vector<Particle> parse_data_msg(const char* read_msg, ull count) {
    std::string str_msg{read_msg, sizeof(float)*4*count};
    std::vector<Particle> particles{count};
    for (ull i = 0; i < count; ++i) {
        float mass, x, y, z;
        memcpy(&mass, str_msg.c_str() + (i*4 * 4), sizeof(float));
        memcpy(&x, str_msg.c_str() + ((i*4 + 1) * 4), sizeof(float));
        memcpy(&y, str_msg.c_str() + ((i*4 + 2) * 4), sizeof(float));
        memcpy(&z, str_msg.c_str() + ((i*4 + 3) * 4), sizeof(float));
        particles[i] = {mass, x, y, z, 0, 0, 0};
    }

    return particles;
}
