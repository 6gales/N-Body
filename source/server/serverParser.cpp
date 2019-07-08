#include <cstring>
#include "serverParser.hpp"

using ull = unsigned long long;

std::string convert_particles_to_msg(std::vector<Particle> &data_particle) {
    std::string send_msg{};
    send_msg.append("DATA ", 5);
//    TODO if need sending count, delete comments
//    char counts[8];
//    for (size_t j = 0; j < 8; ++j) {
//        counts[j] = (char)(count >> ((7-j)*8));
//    }
//    send_msg.append(counts, 8);
    for (const Particle &p : data_particle) {
        float mass = p.getMass();
        float x = p.getX();
        float y = p.getY();
        float z = p.getZ();
        send_msg.append(reinterpret_cast<const char*>(&(mass)), 4);
        send_msg.append(reinterpret_cast<const char*>(&(x)), 4);
        send_msg.append(reinterpret_cast<const char*>(&(y)), 4);
        send_msg.append(reinterpret_cast<const char*>(&(z)), 4);
    }

    return send_msg;
}

std::vector<Particle> parse_start_message(const char* read_msg, ull count) {
    std::string data{read_msg, sizeof(float)*7*count};
    auto res = std::vector<Particle>{count};
    for (ull i = 0; i < count; ++i) {
        float mass, x, y, z, v_x, v_y, v_z;
        memcpy(&mass, data.c_str() + i*7 * 4, sizeof(float));
        memcpy(&x, data.c_str() + (i*7 + 1) * 4, sizeof(float));
        memcpy(&y, data.c_str() + (i*7 + 2) * 4, sizeof(float));
        memcpy(&z, data.c_str() + (i*7 + 3) * 4, sizeof(float));
        memcpy(&v_x, data.c_str() + (i*7 + 4) * 4, sizeof(float));
        memcpy(&v_y, data.c_str() + (i*7 + 5) * 4, sizeof(float));
        memcpy(&v_z, data.c_str() + (i*7 + 6) * 4, sizeof(float));
        res[i] = {mass, x, y, z, v_x, v_y, v_z};
    }

    return res;

}
