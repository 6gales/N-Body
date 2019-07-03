//
// Created by maksimych on 02.07.19.
//

#include <string>
#include "../computer/abstractComputer.h"

std::string construct_data_message(const Particle* particles, size_t count) {
    std::string send_msg{};
    char counts[8];
    for (size_t j = 0; j < 8; ++j) {
        counts[j] = (char)(count >> ((7-j)*8));
    }
    send_msg.append(counts);
    for (size_t i = 0; i < count; ++i) {
        float mass = particles[0].getMass();
        float x = particles[0].getX();
        float y = particles[0].getY();
        float z = particles[0].getZ();
        send_msg.append(reinterpret_cast<const char*>(&(mass)));
        send_msg.append(reinterpret_cast<const char*>(&(x)));
        send_msg.append(reinterpret_cast<const char*>(&(y)));
        send_msg.append(reinterpret_cast<const char*>(&(z)));
    }
}
