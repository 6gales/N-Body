#pragma once
#include "OctTree.h"
#include "../abstractComputer.h"
#include <memory>


class SequentialBHComputer : public Computer
{
private:
	const double theta = 0.1;

	Vector3D* Acces = nullptr;
	Task *t;

public:
	SequentialBHComputer(ComputerType type) : Computer(type) {}
	std::vector<Particle> iterate(int);
};
