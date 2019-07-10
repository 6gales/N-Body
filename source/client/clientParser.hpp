#pragma once
#include <fstream>
#include <vector>
#include <Particle.h>

using ull = unsigned long long;

ull parse_file(std::istream &data_file, std::string &start_message, std::vector<float> &particles_mass);

std::vector<Particle> parse_data_msg(const char*, ull);
