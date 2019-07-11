#pragma once

#include "OctTreeNode.h"
#include <vector>
#include <bits/shared_ptr.h>

class OctTree
{
private:
	std::shared_ptr<OctTreeNode> root;

	void populate(std::vector<Particle> & parts, const Domain3D &domain);
	Domain3D getExpandedDomain(const std::vector<Particle> &parts, const Domain3D &domain) const;

public:
	OctTree(std::vector<Particle>& parts, const Domain3D &domain) : root(0)
	{
		populate(parts, domain);
	}
	~OctTree(){}

	int getHeight() const
	{
		return root->getHeight();
	}

	int getSize() const
	{
		return root->getSize();
	}

	Vector3D computeForce(Particle* part, const double& theta) const;
};