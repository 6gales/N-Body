#include <fstream>
#include <iostream>
#include <sys/time.h>
//#include "../computer.h"
#include "../Barnes_Hut/ompBHComputer/ompBHComputer.h"

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

	struct timeval t1, t2;

	std::shared_ptr<Computer> computer{new OMPBHComputer(ompBHComputer, 8)};
	computer.get()->add(0, array, N);

	gettimeofday(&t1, nullptr);

	for(ull i = 0; i < iterations; ++i)
	{
		computer.get()->iterate(0);
	}
	gettimeofday(&t2, nullptr);

	double time = (t2.tv_sec - t1.tv_sec) + ((double)(t2.tv_usec - t1.tv_usec))/1000000;

	std::cout << "Name: " << argv[0] << std::endl
			<< "Iterations: " << iterations << std::endl
			<< "Time: " << time << " sec." << std::endl;

	return 0;
}
