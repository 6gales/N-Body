#include <string>
#include <memory>
#include <cstring>
#include "../computer/abstractComputer.h"

const Particle* start_message(const std::string &msg, std::shared_ptr<Computer> computer) {
    if (msg.size() <= 5) return nullptr;
    size_t count_particles = ((size_t)msg.at(5) << 56) | ((size_t)msg.at(6) << 48) | ((size_t)msg.at(7) << 40) | ((size_t)msg.at(8) << 32)
                             | ((size_t)msg.at(9) << 24) | ((size_t)msg.at(10) << 16) | ((size_t)msg.at(11) << 8) | (msg.at(12));
    std::string particles_info = msg.substr(13, msg.size()-13);
    std::shared_ptr<Particle> particles(new Particle[count_particles]);
    for (size_t i = 0; i < count_particles; ++i) {
        float mass, x, y, z, v_x, v_y, v_z;
        memcpy(&mass, particles_info.c_str() + i*7, sizeof(float));
        memcpy(&x, particles_info.c_str() + (i+1)*7, sizeof(float));
        memcpy(&y, particles_info.c_str() + (i+2)*7, sizeof(float));
        memcpy(&z, particles_info.c_str() + (i+3)*7, sizeof(float));
        memcpy(&v_x, particles_info.c_str() + (i+4)*7, sizeof(float));
        memcpy(&v_y, particles_info.c_str() + (i+5)*7, sizeof(float));
        memcpy(&v_z, particles_info.c_str() + (i+6)*7, sizeof(float));
        particles.get()[i] = Particle(mass, x, y, z, v_x, v_y, v_z);
    }
    computer->init(particles, count_particles);

    return computer->iterate();

}

const Particle* stop_message(const std::string &msg, std::shared_ptr<Computer> computer) {
    //TODO close socket and stop sending message
}

const Particle* next_message(const std::string &msg, std::shared_ptr<Computer> computer) {
    return computer->iterate();
}

const Particle* parse_message(const std::string &msg, std::shared_ptr<Computer> computer) {
    if (!msg.compare(0, 5, "START")) {
        start_message(msg, computer);
    } else if (!msg.compare(0, 5, "STOP ")) {
        stop_message(msg, computer);
    } else if (!msg.compare(0, 5, "NEXT ")) {
        next_message(msg, computer);
    }
}

