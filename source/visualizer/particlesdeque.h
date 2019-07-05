#pragma once

#include <deque>
#include <vector>
#include "abstractComputer.h"

class ParticlesDeque {
public:

    void push_back(const std::vector<Particle> &p);

    void push_front(const std::vector<Particle> &p);

    void pop_back();

    void pop_front();

    std::vector<Particle> get_back();

    std::vector<Particle> get_front();

private:
    std::deque<std::vector<Particle>> deq{};
};
