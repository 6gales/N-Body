#include "Vector3D.h"

Vector3D operator*(const Vector3D &v, double mul)
{
	return Vector3D(v.x * mul, v.y * mul, v.z * mul);
}

Vector3D operator*(double mul, const Vector3D &v)
{
	return v * mul;
}

Vector3D operator+(const Vector3D &v1, const Vector3D &v2)
{
	return Vector3D(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vector3D operator-(const Vector3D &v1, const Vector3D &v2)
{
	return Vector3D(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> server
=======
>>>>>>> computer
}


std::ostream& operator<<(std::ostream &os, const Vector3D &v)
{
	os << "(" << v.x << "," << v.y << "," << v.z << ")";
	return os;
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> server
=======
>>>>>>> server
=======
>>>>>>> computer
}