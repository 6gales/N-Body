#include "particlesdeque.h"
#include <vector>

void ParticlesDeque::push_back(const std::vector<Particle> &p) {
    deq.push_back(p);
}

void ParticlesDeque::push_front(const std::vector<Particle> &p) {
    deq.push_front(p);
}

void ParticlesDeque::pop_back() {
    if (!deq.empty()) deq.pop_back();
}

void ParticlesDeque::pop_front() {
    if (!deq.empty()) deq.pop_front();
}

std::vector<Particle> ParticlesDeque::get_back() {
    if (deq.empty()) return std::vector<Particle>{};
    return deq.back();
}

std::vector<Particle> ParticlesDeque::get_front() {
    if (deq.empty()) return std::vector<Particle>{};
    return deq.front();
}
