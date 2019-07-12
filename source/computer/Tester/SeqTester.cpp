#include <fstream>
#include <iostream>
#include <sys/time.h>
//#include "../computer.h"
#include "../N_square/sequentialComputer/sequentialComputer.h"

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		std::cout << "Not enough args" << std::endl;
		return 0;
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
	}
	in.close();
	struct timeval t1, t2;

	std::shared_ptr<Computer> computer{new SequentialComputer(sequentialComputer)};
	computer.get()->add(0, array, N);
	gettimeofday(&t1, nullptr);
	for(ull i = 0; i < iterations; ++i)
	{
		if(i % 100 == 0)
			std::cout << "PROCCESSING: "<< i << std::endl; 
		computer.get()->iterate(0);
	}
	gettimeofday(&t2, nullptr);

	double time = (t2.tv_sec - t1.tv_sec) + ((double)(t2.tv_usec - t1.tv_usec))/1000000;

	std::cout << "Name: " << argv[0] << std::endl
			<< "Iterations: " << iterations << std::endl
			<< "Num of parts: " << N << std::endl
			<< "Time: " << time << " sec." << std::endl;

	return 0;
}
