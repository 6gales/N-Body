#pragma once
#include <memory>
#include <iostream>
#include "Particle.h"

class Computer
{
protected:
	double gravity = 6.67408e-11;
	double dt = 0.001;
	std::shared_ptr<Particle> parts;
	size_t N;

public:
	virtual void init(std::shared_ptr<Particle>, size_t) = 0;
	virtual Particle* iterate() = 0;
	virtual ~Computer() {};
	void setGravity(double _gravity){ gravity = _gravity; }
	size_t getSize() const { return N; }
};