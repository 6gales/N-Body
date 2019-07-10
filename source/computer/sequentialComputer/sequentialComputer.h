#pragma once
#include "../abstractComputer.h"
#include <memory>

class SequentialComputer : public Computer
{
private:
	Vector3D** forces;
	Task *t;
	void fillForces();

public:
	const std::vector<Particle> &iterate(int);
};