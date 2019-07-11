#pragma once
#include "../abstractComputer.h"
#include <memory>

class SequentialRKComputer : public Computer
{
private:
	Vector3D** forces = nullptr;
	Task *t;
	Vector3D getAcc(ull i);
	void fillForces();
	void fillForces(std::vector<Vector3D> coords);

public:
	SequentialRKComputer(ComputerType type) : Computer(type, 1) {}
	std::vector<Particle> iterate(int);
};
