#include <fstream>
#include <iostream>
#include "../computer.h"

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		std::cout << "Not enough args" << std::endl;
	}
	std::ifstream in(argv[1]);

	ull N;
	ull iterations;
	in >> iterations >> N;

	std::vector<Particle> array(N);
	for(ull i = 0; i < N; ++i)
	{
		float m, x, y, z, vx, vy, vz;
		in >> m >> x >> y >> z >> vx >> vy >> vz;
		array[i] = Particle(m, x, y, z, vx, vy, vz);
		std::cout << array[i] << std::endl;
	}
	in.close();

	Task ptrTask(N, array);



	return 0;
}