#pragma once
#include "abstractComputer.h"
#include <memory>

class SequentialComputer : public Computer
{
private:
	Vector3D** forces;

	void fillForces();

public:
<<<<<<< HEAD
	void init(std::vector<Particle> &, ull);
=======
	void init(std::vector<Particle> &, size_t);
>>>>>>> computer
	const std::vector<Particle> &iterate();
	~SequentialComputer();
};