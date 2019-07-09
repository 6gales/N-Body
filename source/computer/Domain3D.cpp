#include "Domain3D.h"

Direction Domain3D::detectOctant(const Vector3D &point) const
{
	const Vector3D center(left + width() / 2.0, bottom + height() / 2.0, down + depth() / 2.0);

	const bool west = (point.x <= center.x);
	const bool north = (center.y <= point.y);
	const bool up = (center.z <= point.z);

	if(north)
	{
		if(west)
			return up ? NORTH_WEST_UP : NORTH_WEST_DOWN;
		else
			return up ? NORTH_EAST_UP : NORTH_EAST_DOWN;
	}
	else
	{
		if(west)
			return up ? SOUTH_WEST_UP : SOUTH_WEST_DOWN;
		else
			return up ? SOUTH_EAST_UP : SOUTH_EAST_DOWN;
	}
}

Domain3D Domain3D::getOctant(Direction &d) const
{
	const Vector3D center(left + width()/2.0, bottom + height() / 2.0, down + depth() / 2.0);

	switch(d)
	{
		case NORTH_WEST_UP:		return Domain3D(left, center.x, center.y, top, center.z, up);
		case NORTH_WEST_DOWN:	return Domain3D(left, center.x, center.y, top, down, center.z);
		case NORTH_EAST_UP:		return Domain3D(center.x, right, center.y, top, center.z, up);
		case NORTH_EAST_DOWN:	return Domain3D(center.x, right, center.y, top, down, center.z);
		case SOUTH_WEST_UP:		return Domain3D(left, center.x, bottom, center.y, center.z, up);
		case SOUTH_WEST_DOWN:	return Domain3D(left, center.x, bottom, center.y, down, center.z);
		case SOUTH_EAST_UP:		return Domain3D(center.x, right, bottom, center.y, center.z, up);
		case SOUTH_EAST_DOWN:	return Domain3D(center.x, right, bottom, center.y, down, center.z);
		default: 				return Domain3D(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	}
}


void Domain3D::expand(const Vector3D &point)
{
	if(point.x < left)
		left = point.x;
	if(point.x > right)
		right = point.x;

	if(point.y < bottom)
		bottom = point.y;
	if(point.y > top)
		top = point.y;

	if(point.z < down)
		down = point.z;
	if(point.z > up)
		up = point.z;
}