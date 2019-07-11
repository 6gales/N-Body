#include "OctTree.h"

void OctTree::populate(std::vector<Particle>& parts, const Domain3D &domain)
{
	const Domain3D expandedDomain = getExpandedDomain(parts, domain);
	root.reset(new OctTreeNode(expandedDomain));
	for(int i = 0; i < parts.size(); ++i)
	{
		root->insertPart(&parts[i]);
	}
	root->computeMassCenter();
}

Vector3D OctTree::computeForce(Particle* _part, const double &theta) const
{
	return root->computeForce(_part, theta);
}

Domain3D OctTree::getExpandedDomain(const std::vector<Particle> & parts, const Domain3D &domain) const
{
	Domain3D expandedDomain = domain;
	for(int i = 0; i < parts.size(); ++i)
	{
		expandedDomain.expand(parts[i].coords);
	}
	return expandedDomain;
}