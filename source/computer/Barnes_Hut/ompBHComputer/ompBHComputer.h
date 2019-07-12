#pragma once
#include "../OctTree.h"
#include "../../abstractComputer.h"
#include <memory>
#include <omp.h>


class OMPBHComputer : public Computer
{
private:
	const double theta = 0.1;
	Vector3D* Acces = nullptr;
	Task *t;

public:
	OMPBHComputer(ComputerType type, size_t threadsNum) : Computer(type, threadsNum)
	{
		omp_set_num_threads(threadsNum);
	}
	std::vector<Particle> iterate(int);
};
