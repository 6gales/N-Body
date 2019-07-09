#include "sequentialBHComputer.h"

void SequentialBHComputer::init(std::vector<Particle> & _parts, ull N)
{
	parts = _parts;
	Acces = new Vector3D[parts.size()];
}

const std::vector<Particle>& SequentialBHComputer::iterate()
{
	std::cout << "before build tree" << std::endl;
	OctTree tree(parts, Domain3D(-10.0, 10.0, -10.0, 10.0, -10.0, 10.0));
	std::cout << "after build" << std::endl;
	for(int i = 0; i < parts.size(); ++i)
	{
		std::cout << "scoring force for " << i << " elem" << std::endl;
		Vector3D force = tree.computeForce(&parts[i], theta);
		std::cout << "after force" << std::endl;
		Acces[i] = tree.computeForce(&parts[i], theta) * (1.0 / parts[i].mass);
		std::cout << force << " " << Acces[i] << std::endl;
	}


	std::cout << "after computing acc" << std::endl;
	for(int i = 0; i < parts.size(); ++i)
		parts[i].vel += Acces[i] * dt;


	std::cout << "after vel" << std::endl;
	for(int i = 0; i < parts.size(); ++i)
		parts[i].coords += parts[i].vel * dt;

	return parts;
}
