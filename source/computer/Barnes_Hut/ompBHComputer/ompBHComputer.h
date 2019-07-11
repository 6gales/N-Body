#pragma once
#include "OctTree.h"
#include "../../abstractComputer.h"
#include <memory>


class OMPBHComputer : public Computer
{
private:
	const double theta = 0.1;
	Vector3D* Acces = nullptr;
	Task *t;

public:
	OMPBHComputer(ComputerType type) : Computer(type, 1) {}
	std::vector<Particle> iterate(int);
};
