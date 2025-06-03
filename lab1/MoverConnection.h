#ifndef __MOVERCONNECTION
#define __MOVERCONNECTION

#include <particle.h>
#include <iostream>
#include <pfgen.h>
#include <MySpring.h>
#include "Mover.h"
#include <cmath>
#include "CubeMover.h"
#include "ElipsoidMover.h"
#include "PyramidMover.h"

class MoverConnection {
public:
	MoverConnection() {
		// Exemple : trois cubes ayant chacun une densité différente
		movers.push_back(new CubeMover(cyclone::Vector3(0, 15, 0),
			cyclone::Vector3(1, 1, 2),
			1000.0f));  // densité = 300 kg/m³
		movers.push_back(new CubeMover(cyclone::Vector3(5, 5, 4),
			cyclone::Vector3(0.5f, 0.5f, 0.5f),
			800.0f)); // densité = 800 kg/m³
		movers.push_back(new CubeMover(cyclone::Vector3(-5, 15, 2),
			cyclone::Vector3(2, 2, 1),
			1200.0f)); // densité = 1200 kg/m³

		// Exemple : deux ellipsoïdes (ovales) à densité propre
		movers.push_back(new EllipsoidMover(cyclone::Vector3(10, 13, 0),
			1.0f, 2.0f, 3.0f,
			500.0f)); // densité = 500
		movers.push_back(new EllipsoidMover(cyclone::Vector3(-8, 12, 4),
			2.5f, 1.5f, 1.0f,
			900.0f)); // densité = 900
		movers.push_back(new EllipsoidMover(cyclone::Vector3(0, 0, 2),
			1.0f, 1.0f, 1.0f,
			100.0f)); // densité = 900
		movers.push_back(
			new PyramidMover(cyclone::Vector3(10, 20, 10),
				1.0f,   // halfWidth
				2.0f,   // halfDepth
				3.0f,   // heightY
				750.0f  // density
			)
		);
		movers.push_back(
			new PyramidMover(cyclone::Vector3(-10, 15, -10),
				3.0f,   // halfWidth
				3.0f,   // halfDepth
				3.0f,   // heightY
				1005.0f  // density
			)
		);
	}

	MoverConnection(const MoverConnection&) = delete;
	MoverConnection& operator=(const MoverConnection&) = delete;

	~MoverConnection() {
		std::cout << "Destroying MoverConnection" << std::endl;
		for (Mover* mover : movers) {
			delete mover;
		}
	}

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

			if (movers[i]) {
				movers[i]->draw(shadow);
			}
			else {
				std::cerr << "Warning: mover[" << i << "] is null!" << std::endl;
			}
		}
	}
};

#endif