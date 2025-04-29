#define NOMINMAX
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

#include "core.h"

int main()
{

}

//int main()
//{
//	cyclone::Vector3 a(1, 1, -1);
//	cyclone::Matrix3 m = { 1, 0, 2, 1, 2, 3, -1, 0, 1 };
//
//	cyclone::Vector3 c = m.transform(a);
//	std::cout << "a * m = " << c.toString() << std::endl;
//}

//int main()
//{
//	cyclone::Vector3 a(1, 2, -1);
//	cyclone::Vector3 b(3, 0, 2);
//	a.normalise();
//	b.normalise();
//
//	cyclone::Vector3 c = a.cross(b);
//	if (c.magnitude() != 0) {
//		c.normalise();
//		b = c.cross(a);
//	}
//
//	std::cout << "Orthonormal basis" << std::endl;
//	std::cout << "a: " << a.toString() << std::endl;
//	std::cout << "b: " << b.toString() << std::endl;
//	std::cout << "c: " << c.toString() << std::endl;
//}

//int main()
//{
//	const double DEGREES_TO_RADIANT = M_PI / 180.0f;
//	const double RADIANT_TO_DEGREES = 180.0f / M_PI;
//	cyclone::Vector3 v(2, -1, 1);
//	cyclone::Vector3 u(1, 1, 2);
//	float d = v.dot(u);
//	float c = d / (u.magnitude() * v.magnitude());
//	double deg = acos(c) * RADIANT_TO_DEGREES;
//
//	std::cout << "v(2, -1, 1) and u(1, 1, 2) = " << deg << std::endl;
//}


//int main()
//{
//	//Move Point
//	cyclone::Vector3 p(1, 4, -5);
//	p = p + cyclone::Vector3(5, 1, 4);
//	std::cout << p.toString() << std::endl;
//
//	//Add Vector
//	cyclone::Vector3 v1(1, 0, 0);
//	cyclone::Vector3 v2(0, 1, 0);
//	cyclone::Vector3 v3 = v1 + v2;
//	std::cout << v3.toString() << std::endl;
//
//	//Multiply
//	cyclone::Vector3 v4(0, 1, 0);
//	v4 *= 5;
//	std::cout << v4.toString() << std::endl;
//
//	//Get Vector
//	cyclone::Vector3 start(8, 2, 5);
//	cyclone::Vector3 end(1, -1, 4);
//	cyclone::Vector3 v5 = end - start;
//	std::cout << v5.toString() << std::endl;
//
//	//Normalize
//	cyclone::Vector3 v6(1, 3, 4);
//	v6.normalise();
//	std::cout << v6.toString() << std::endl;
//
//   return 0;
//}
