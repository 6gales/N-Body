#pragma once
#include <vector>
#include <memory>
#include "Particle.h"
#include "Domain3D.h"

#define NUM_OF_CHILDREN 8

class OctTreeNode
{
private:
	Domain3D domain;

	bool isEmpty;

	std::unique_ptr<OctTreeNode> children[NUM_OF_CHILDREN];

	Particle* part;
	Particle massCenter;

	OctTreeNode* getChild(Direction d);
	OctTreeNode* getChild(const Vector3D &coord);

public:

	OctTreeNode(const Domain3D &dm) : domain(dm), isEmpty(true), part(nullptr){}
	~OctTreeNode() {}

	const Particle& getMassCenter() const
	{
		return massCenter;
	}

	void insertPart(Particle* part);
	bool computeMassCenter();

	Vector3D computeForce(Particle* part, const double& theta) const;

	Vector3D computeForce(Particle *part1, const Particle *part2) const;

	int getHeight() const;
	int getSize() const;


};
