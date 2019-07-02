//
// Created by maksimych on 02.07.19.
//

#include <string>
#include "../computer/abstractComputer.h"

std::string construct_data_message(const Particle* particles, size_t count) {
    std::string send_msg{};
    char counts[8];
    counts[0] = (char)(count >> 56);
    counts[1] = (char)(count >> 48);
    counts[2] = (char)(count >> 40);
    counts[3] = (char)(count >> 32);
    counts[4] = (char)(count >> 24);
    counts[5] = (char)(count >> 16);
    counts[6] = (char)(count >> 8);
    counts[7] = (char)(count);
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
