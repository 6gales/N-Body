#pragma once

#include <memory>
#include "abstractComputer.h"
#include "sequentialComputer/sequentialComputer.h"
#include "Runge_Kuta/sequentialRKComputer/sequentialRKComputer.h"
#include "sequentialBHComputer/sequentialBHComputer.h"
#include "ompComputer/ompComputer.h"
#include "Runge_Kuta/ompRKComputer/ompRKComputer.h"
//#include "mpiComputer/mpiComputer.h"
#include "ComputerType.h"

std::shared_ptr<Computer> getInstanceOf(ComputerType type, size_t threads);