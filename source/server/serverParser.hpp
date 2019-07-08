#pragma once
#include <string>
#include <vector>
#include "../computer/abstractComputer.h"

std::string convert_particles_to_msg(std::vector<Particle> &data_particle);

std::vector<Particle> parse_start_message(const char* read_msg, unsigned long long count);