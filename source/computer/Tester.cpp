//#include "computer.h"
#include <fstream>
#include <memory>
#include <iostream>
#include "sequentialBHComputer.h"


int main()
{
	std::ifstream in("in.txt");
	std::ofstream out("out.txt");

	Computer* computer = new SequentialBHComputer();//new ompComputer(4);
	int N;
	in >> N;

	std::vector<Particle> parts(N);


	for(int i = 0; i < N; ++i)
	{
		double mass, x, y, z, vx, vy, vz;
		in >> mass >> x >> y >> z >> vx >> vy >> vz;
		parts[i] = Particle(mass, x, y, z, vx, vy, vz);
		std::cout << parts[i] << std::endl;
	}
	in.close();

	computer->init(parts, N);
	std::cout << " after init" << std::endl;

	for(int iter = 0; iter < 1000; ++iter)
	{
		out << "====================ITERATION "<< iter <<"====================="<< std::endl;
		std::cout << "before iterate" << std::endl;
		auto buff = computer->iterate();
		std::cout << "after iterate" << std::endl;
		for(int i = 0; i < N; ++i)
		{
			//std::cout << buff[i] << std::endl;
			out << buff[i] << std::endl;
		}
		out << "====================ITERATION "<< iter <<"====================="<< std::endl << std::endl;
	}

	out.close();
}