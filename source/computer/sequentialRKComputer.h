#pragma once
#include "abstractComputer.h"
#include <memory>

class SequentialRKComputer : public Computer
{
private:
	Vector3D** forces;
	Vector3D getAcc(ull i);
	void fillForces();

public:
	void init(std::vector<Particle> &, ull);
	const std::vector<Particle> &iterate();
	~SequentialRKComputer();

	void fillForces(std::vector<Vector3D> coords);
};