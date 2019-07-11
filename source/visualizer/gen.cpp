#include <fstream>

double r() {
	return (double)(rand())/RAND_MAX;
}

int main(int argc, char const *argv[]) {
	size_t count = 10000;
	std::ofstream file("./in.txt");
	file << count << "\n";
	srand(time(0));
	for (int i = 0; i < count; ++i) {
		file << r()*1e20 << " " << r()/r() << " " << r()/(r()*r()) << " "
		<< r()*r() << " " << r() << " " << r() << " " << r() << "\n";
	}
	return 0;
}