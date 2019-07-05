#pragma once
#include <string>
#include <memory>
#include <cstring>
#include <vector>
#include <iostream>
#include "../computer/abstractComputer.h"

enum CommandType : char {
    START_, STOP_, NEXT_, ALIVE_, PAUSE_, UNKNOWN_
};

CommandType start_message(const std::string &msg, std::shared_ptr<Computer> computer, std::vector<Particle> &result);

CommandType stop_message(const std::string &msg, std::shared_ptr<Computer> computer, std::vector<Particle> &result);

CommandType next_message(const std::string &msg, std::shared_ptr<Computer> computer, std::vector<Particle> &result);

CommandType pause_message(const std::string &msg, std::shared_ptr<Computer> computer, std::vector<Particle> &result);

CommandType alive_message(const std::string &msg, std::shared_ptr<Computer> computer, std::vector<Particle> &result);

CommandType parse_message(const std::string &msg, std::shared_ptr<Computer> computer, std::vector<Particle> &result);

std::string convert_particles_to_msg(const std::vector<Particle> &particles);