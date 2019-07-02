#pragma once
#include "abstractComputer.h"
#include "sequentialComputer.h"

Computer* getInstanceOf()
{
	return new SequentialComputer();
}