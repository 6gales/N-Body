#include "computer.h"

std::shared_ptr<Computer> getInstanceOf(ComputerType type, size_t threads)
{
	switch (type)
	{
		case sequentialComputer:
			return std::shared_ptr<Computer>(new SequentialComputer(type));

		case sequentialBHComputer:
			return std::shared_ptr<Computer>(new SequentialBHComputer(type));

		case sequentialRKComputer:
			return std::shared_ptr<Computer>(new SequentialRKComputer(type));

		case ompComputer:
			return std::shared_ptr<Computer>(new OMPComputer(type, threads));

		case ompRKComputer:
			return std::shared_ptr<Computer>(new OMPRKComputer(type, threads));

        case ompBHComputer:
            return std::shared_ptr<Computer>(new OMPBHComputer(type, threads));
	}
}