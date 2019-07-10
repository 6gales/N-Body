#pragma once
#include "../abstractComputer.h"
#include <memory>

class SequentialComputer : public Computer
{
private:
	Vector3D** forces;

	void fillForces();

public:
	void init(std::vector<Particle> &, ull);
	const std::vector<Particle> &iterate();
	~SequentialComputer();
};