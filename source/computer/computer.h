#pragma once

#include "abstractComputer.h"
#include "sequentialComputer/sequentialComputer.h"
#include "sequentialRKComputer/sequentialRKComputer.h"
#include "sequentialBHComputer/sequentialBHComputer.h"
#include "ompComputer/ompComputer.h"
#include "ompRKComputer/ompRKComputer.h"

Computer* getInstanceOf(size_t threads)
{
	return new SequentialComputer();
}