#include "computer.h"

std::shared_ptr<Computer> getInstanceOf(ComputerType type, size_t threads)
{
	switch (type)
	{
		case sequentialComputer:
			return std::make_shared<Computer>(new SequentialComputer(type));

		case sequentialBHComputer:
			return std::make_shared<Computer>(new SequentialBHComputer(type));

		case sequentialRKComputer:
			return std::make_shared<Computer>(new SequentialRKComputer(type));

		case ompComputer:
			return std::make_shared<Computer>(new OMPComputer(type, threads));

		case ompRKComputer:
			return std::make_shared<Computer>(new OMPRKComputer(type, threads));
	}
}