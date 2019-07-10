#include "sequentialBHComputer.h"

void SequentialBHComputer::init(std::vector<Particle> & _parts, ull N)
{
	parts = _parts;
	Acces = new Vector3D[parts.size()];
}

const std::vector<Particle>& SequentialBHComputer::iterate()
{
	OctTree tree(parts, Domain3D(-10.0, 10.0, -10.0, 10.0, -10.0, 10.0));
	for(int i = 0; i < parts.size(); ++i)
	{
		Vector3D force = tree.computeForce(&parts[i], theta);
		Acces[i] = tree.computeForce(&parts[i], theta) * (1.0 / parts[i].mass);
	}


	for(int i = 0; i < parts.size(); ++i)
		parts[i].vel += Acces[i] * dt;


	for(int i = 0; i < parts.size(); ++i)
		parts[i].coords += parts[i].vel * dt;

	return parts;
}
