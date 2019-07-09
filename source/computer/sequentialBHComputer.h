#pragma once
#include "OctTree.h"
#include "abstractComputer.h"
#include <memory>


class SequentialBHComputer : public Computer
{
private:
	Vector3D* Acces = NULL;
	std::vector<Particle> parts;
	const double theta = 0.1;

public:
	void init(std::vector<Particle> &, ull);
	const std::vector<Particle> & iterate();
	~SequentialBHComputer()
	{
		if(Acces != nullptr)
		{
			delete[] Acces;
			Acces = NULL;
		}
		return;
	};
};
