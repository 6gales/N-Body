#pragma once
#include <iostream>
#include <vector>
#include "Particle.h"

using ull = unsigned long long;

class Computer
{
protected:
	double gravity = 6.67408e-11;
	double dt = 0.001;
	ull N;
	std::vector<Particle> particleVectors[2];
	char previous = 1,
		current = 0;

public:
	virtual void init(std::vector<Particle> &, ull) = 0;
	virtual const std::vector <Particle>& iterate() = 0;
	virtual ~Computer() {};
	void setGravity(double _gravity){ gravity = _gravity; }
	ull getSize() const { return N; }
};