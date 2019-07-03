

#include <iostream>
#include <vector>
#include "../computer/abstractComputer.h"

void printParticles(std::vector<Particle> particles, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        std::cerr << "mass = " << particles[i].getMass() << ", x = " << particles[i].getX() << ", y = " << particles[i].getY()
                    << ", z = " << particles[i].getZ() << ", v_x = " << particles[i].getVelX() << ", v_y = " << particles[i].getVelY()
                    << ", v_z = " << particles[i].getVelZ() << std::endl;
    }
}