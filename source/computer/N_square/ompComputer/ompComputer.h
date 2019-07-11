#pragma once

#include "../../abstractComputer.h"
#include "omp.h"

class OMPComputer : public Computer
{
private:
	Vector3D** forces;
	Task* t;
	void fillForces();

public:
	OMPComputer(ComputerType type, size_t threadsNum) : Computer(type, threadsNum)
	{
		omp_set_num_threads(threadsNum);
		forces = nullptr;
	}
	std::vector<Particle> iterate(int key);
};