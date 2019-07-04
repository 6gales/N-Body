#include <string>
#include <memory>
#include <cstring>
#include <vector>
#include <iostream>
#include "../computer/abstractComputer.h"

std::vector<Particle> start_message(const std::string &msg, std::shared_ptr<Computer> computer) {
    size_t count_particles = ((size_t)msg.at(0) << 56) | ((size_t)msg.at(1) << 48) | ((size_t)msg.at(2) << 40) | ((size_t)msg.at(3) << 32)
                             | ((size_t)msg.at(4) << 24) | ((size_t)msg.at(5) << 16) | ((size_t)msg.at(6) << 8) | (msg.at(7));
    std::string particles_info = msg.substr(8);
    std::vector<Particle> particles{count_particles};
    for (size_t i = 0; i < count_particles; ++i) {
        float mass, x, y, z, v_x, v_y, v_z;
        memcpy(&mass, particles_info.c_str() + i*7 * 4, sizeof(float));
        memcpy(&x, particles_info.c_str() + (i*7 + 1) * 4, sizeof(float));
        memcpy(&y, particles_info.c_str() + (i*7 + 2) * 4, sizeof(float));
        memcpy(&z, particles_info.c_str() + (i*7 + 3) * 4, sizeof(float));
        memcpy(&v_x, particles_info.c_str() + (i*7 + 4) * 4, sizeof(float));
        memcpy(&v_y, particles_info.c_str() + (i*7 + 5) * 4, sizeof(float));
        memcpy(&v_z, particles_info.c_str() + (i*7 + 6) * 4, sizeof(float));
        particles[i] = {mass, x, y, z, v_x, v_y, v_z};
    }
//    computer->init(particles, count_particles);

//    return computer->iterate();

    return particles;

}

std::vector<Particle> stop_message(const std::string &msg, std::shared_ptr<Computer> computer) {
    return std::vector<Particle>{};
    //TODO close socket and stop sending message
}

std::vector<Particle> next_message(const std::string &msg, std::shared_ptr<Computer> computer) {
//    return computer->iterate();
}

std::vector<Particle> parse_message(const std::string &msg, std::shared_ptr<Computer> computer) {
    if (!msg.compare(0, 5, "START")) {
        return start_message(msg.substr(5), computer);
    } else if (!msg.compare(0, 5, "STOP ")) {
        return stop_message(msg, computer);
    } else if (!msg.compare(0, 5, "NEXT ")) {
        return next_message(msg, computer);
    }

    return std::vector<Particle>{};
}

std::string convert_particles_to_msg(const std::vector<Particle> &particles) {
    std::string send_msg{};
    char counts[8];
    for (size_t j = 0; j < 8; ++j) {
        counts[j] = (char)(particles.size() >> ((7-j)*8));
    }
    send_msg.append(counts, 8);
    for (const Particle &p : particles) {
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

