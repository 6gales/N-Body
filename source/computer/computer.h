#pragma once
#include "abstractComputer.h"
#include "sequentialComputer.h"
#include "ompComputer.h"

Computer* getInstanceOf(size_t threads)
{
	return new SequentialComputer();
}