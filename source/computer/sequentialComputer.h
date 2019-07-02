#pragma once
#include "abstractComputer.h"
#include <memory>

class SequentialComputer : public Computer
{
private:
	Vector3D** forces;

	void fillForces();

public:
	void init(std::shared_ptr<WideParticle>, size_t);
	Particle* iterate();
	~SequentialComputer();
};