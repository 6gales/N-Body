#pragma once
#include <ostream>
#include <math.h>

class Vector3D
{
public:
	double  x,
			y,
			z;

	Vector3D() : x(0.0), y(0.0), z(0.0) {}
	Vector3D(double _x, double _y, double _z) : x(_x), y(_y), z(_z){}
	~Vector3D() {}

	void set(double _x, double _y, double _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	const Vector3D& operator=(double val)
	{
		x = val;
		y = val;
		z = val;
		return *this;
	}

	const Vector3D& operator=(const Vector3D &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	const Vector3D& operator+=(const Vector3D &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	const Vector3D& operator-=(const Vector3D &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	double module()
	{
		return sqrt(x * x + y * y + z * z);
	}

};


Vector3D operator*(const Vector3D &v, double mul);

Vector3D operator*(double mul, const Vector3D &v);

Vector3D operator+(const Vector3D &v1, const Vector3D &v2);

Vector3D operator-(const Vector3D &v1, const Vector3D &v2);


std::ostream& operator<<(std::ostream &, const Vector3D&);
