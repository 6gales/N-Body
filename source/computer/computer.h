#pragma once

#include <memory>
#include "abstractComputer.h"
#include "./N_square/sequentialComputer/sequentialComputer.h"
#include "Runge_Kuta/sequentialRKComputer/sequentialRKComputer.h"
#include "./Barnes_Hut/sequentialBHComputer/sequentialBHComputer.h"
#include "./Barnes_Hut/ompBHComputer/ompBHComputer.h"
#include "./N_square/ompComputer/ompComputer.h"
#include "Runge_Kuta/ompRKComputer/ompRKComputer.h"
//#include "./N_square/mpiComputer/mpiComputer.h"
#include "ComputerType.h"

std::shared_ptr<Computer> getInstanceOf(ComputerType type, size_t threads);