//#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>
//#include <ostream>
//#include <string.h>
//#include <fstream>
//#include <ctime>
//#include <random>
//
//int main(int argc, char* argv[])
//{
//	if(argc < 10)
//	{
//		std::cout << "Not enough args"
//			<< "\n1: outfile\n2: num of particles\n3: min coord\n4:"
//	           " max coord\n5: min vel\n6: max vel\n7: min mass\n8: max mass\n9:iteration"
//			<< std::endl;
//		return 0;
//	}
//
//	unsigned long long N = atoll(argv[2]);
//	float minC = atof(argv[3]);
//	float maxC = atof(argv[4]);
//	float minV = atof(argv[5]);
//	float maxV = atof(argv[6]);
//	float minM = atof(argv[7]);
//	float maxM = atof(argv[8]);
//	unsigned long long iter = atoll(argv[9]);
//
//
//	std::ofstream out(argv[1]);
//
//	std::uniform_real_distribution<float> CordGen(minC, maxC);
//	std::uniform_real_distribution<float> MassGen(minM, maxM);
//	std::uniform_real_distribution<float> VeloGen(minV, maxV);
//	std::random_device rd;
//	out << iter << std::endl;
//	out << N << std::endl;
//	for(unsigned long long i = 0; i < N; ++i)
//	{
//		out << MassGen(rd) << " " << CordGen(rd) << " " << CordGen(rd) << " " << CordGen(rd) << " "
//			<< VeloGen(rd) << " " << VeloGen(rd) << " " << VeloGen(rd) << std::endl;
//		out.flush();
// 	}
//	out.close();
//	return 0;
//}
