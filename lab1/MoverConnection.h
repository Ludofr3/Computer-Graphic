#ifndef __MOVERCONNECTION
#define __MOVERCONNECTION

#include <particle.h>
#include <iostream>
#include <pfgen.h>
#include <MySpring.h>
#include "Object.h"
#include <cmath>

class MoverConnection {
public:
	MoverConnection() {
		movers.push_back(new Mover(cyclone::Vector3(0, 10, 0)));
	}
	~MoverConnection() {}

	cyclone::ParticleGravity* gravity;
	cyclone::ParticleForceRegistry* forces;
	std::vector<Mover*> movers;
	std::vector<cyclone::Myspring*> springs;

	void moverRotate(int num, float angleX_deg, float angleY_deg, float angleZ_deg) {
		cyclone::Quaternion q = createQuaternionFromEulerAngles(angleX_deg, angleY_deg, angleZ_deg);
		movers.at(num)->body->setOrientation(q);
		movers.at(num)->body->getTransform().setOrientationAndPos(q, movers.at(num)->body->getPosition());
	}

	cyclone::Quaternion createQuaternionFromEulerAngles(float angleX_deg, float angleY_deg, float angleZ_deg) {
		const float deg2rad = 3.141592f / 180.0f;

		float halfX = angleX_deg * 0.5f * deg2rad;
		float halfY = angleY_deg * 0.5f * deg2rad;
		float halfZ = angleZ_deg * 0.5f * deg2rad;

		// Quaternion autour de X
		cyclone::Quaternion qx;
		qx.r = std::cos(halfX);
		qx.i = std::sin(halfX);
		qx.j = 0.0f;
		qx.k = 0.0f;

		// Quaternion autour de Y
		cyclone::Quaternion qy;
		qy.r = std::cos(halfY);
		qy.i = 0.0f;
		qy.j = std::sin(halfY);
		qy.k = 0.0f;

		// Quaternion autour de Z
		cyclone::Quaternion qz;
		qz.r = std::cos(halfZ);
		qz.i = 0.0f;
		qz.j = 0.0f;
		qz.k = std::sin(halfZ);

		cyclone::Quaternion q = qz;
		q *= qy;
		q *= qx;

		q.normalise();
		return q;
	}

	void update(float duration) {
		for (int i = 0; i < movers.size(); i++) {
			movers[i]->update(duration);
		}
	}

	void MoverConnection::draw(int shadow)
	{
		for (unsigned int i = 0; i < movers.size(); i++) {
			if (!shadow)
				glLoadName(i + 1);
			movers[i]->draw(shadow);
		}
	}
};

#endif