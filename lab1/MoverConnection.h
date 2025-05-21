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
		forces = new cyclone::ParticleForceRegistry();
		movers = {
			new Mover(cyclone::Vector3(0, 1.5, 0)),
			new Mover(cyclone::Vector3(0, 1.5, 0)),
			new Mover(cyclone::Vector3(0, 10.5, 0)),

			/*
			new Mover(cyclone::Vector3(-3, 4, 1)),
			new Mover(cyclone::Vector3(-3, 4, -1)),

			new Mover(cyclone::Vector3(-1, 4, 1)),
			new Mover(cyclone::Vector3(-1, 4, -1)),

			new Mover(cyclone::Vector3(1, 4, 1)),
			new Mover(cyclone::Vector3(1, 4, -1)),

			new Mover(cyclone::Vector3(3, 4, 1)),
			new Mover(cyclone::Vector3(3, 4, -1)),

			new Mover(cyclone::Vector3(5, 4, 1)),
			new Mover(cyclone::Vector3(5, 4, -1)),*/
		};

		moverRotate(0, -45., 0., 0.);
		moverRotate(1, -45., 0., 0.);
		moverRotate(2, 45., 45., 0.);

		gravity = new cyclone::ParticleGravity(cyclone::Vector3(0, -10, 0));

		// Initialisation des springs avec ancres distinctes
		//springs.push_back(new cyclone::Myspring(movers[0]->particle, 20, 3));
		//springs.push_back(new cyclone::Myspring(movers[1]->particle, 20, 3));

		//for (int i = 0; i < movers.size(); i++) {
			//forces->add(movers[i]->particle, gravity);
			//forces->add(movers[i]->particle, movers[i]->spring);
		//}
	}
	~MoverConnection() {}

	cyclone::ParticleGravity* gravity;
	cyclone::ParticleForceRegistry* forces;
	std::vector<Mover*> movers;
	std::vector<cyclone::Myspring*> springs;

	void moverRotate(int num, float angleX_deg, float angleY_deg, float angleZ_deg) {
		cyclone::Quaternion q = createQuaternionFromEulerAngles(angleX_deg, angleY_deg, angleZ_deg);
		movers.at(num)->orientation = q;
		movers.at(num)->transformMatrix.setOrientationAndPos(q, movers.at(num)->particle->getPosition());
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
		/*for (int i = 0; i < movers.size(); i++) {
			movers[i]->update(duration);
		}*/
		forces->updateForces(duration);
		//checkCollisions();
	}

	void MoverConnection::draw(int shadow)
	{
		for (unsigned int i = 0; i < movers.size(); i++) {
			if (!shadow)
				glLoadName(i + 1);
			movers[i]->draw(shadow);
		}

		//if (springs.size() > 0) {
		//	glBegin(GL_LINE_STRIP);
		//	glColor3f(0, 1, 0);
		//	glLineWidth(2.0f);
		//	glBegin(GL_LINES);
		//	for (unsigned int i = 0; i < movers.size() - 1; i++) {
		//		cyclone::Vector3 pos1 = movers[i]->particle->getPosition();
		//		cyclone::Vector3 pos2 = movers[i + 1]->particle->getPosition();
		//		//glVertex3f(anchor.x, anchor.y, anchor.z); //Starting point
		//		glVertex3f(pos1.x, pos1.y, pos1.z);
		//		glVertex3f(pos2.x, pos2.y, pos2.z); //Ending point
		//	}
		//	glEnd();
		//}
	}

private:
	// Nouvelle méthode pour vérifier les collisions
	void checkCollisions() {
		for (size_t i = 0; i < movers.size(); ++i) {
			for (size_t j = i + 1; j < movers.size(); ++j) {
				cyclone::Vector3 pos1 = movers[i]->particle->getPosition();
				cyclone::Vector3 pos2 = movers[j]->particle->getPosition();
				cyclone::Vector3 delta = pos1 - pos2;
				double distance = delta.magnitude();
				double sumRadii = movers[i]->size + movers[j]->size; // Somme des rayons
				if (distance < sumRadii) {
					handleCollision(movers[i], movers[j]); // Collision détectée
				}
			}
		}
	}

	// Nouvelle méthode pour gérer les collisions
	void handleCollision(Mover* m1, Mover* m2) {
		cyclone::Vector3 pos1 = m1->particle->getPosition();
		cyclone::Vector3 pos2 = m2->particle->getPosition();
		cyclone::Vector3 vel1 = m1->particle->getVelocity();
		cyclone::Vector3 vel2 = m2->particle->getVelocity();

		// Normalisation de la direction de la collision
		cyclone::Vector3 delta = pos1 - pos2;
		delta.normalise();

		// Projection des vitesses sur la direction de la collision
		double v1 = vel1 * delta;
		double v2 = vel2 * delta;

		// Calcul des nouvelles vitesses (collision élastique simplifiée)
		cyclone::Vector3 newVel1 = vel1 + (v2 - v1) * delta;
		cyclone::Vector3 newVel2 = vel2 + (v1 - v2) * delta;

		// Mise à jour des vitesses
		m1->particle->setVelocity(newVel1);
		m2->particle->setVelocity(newVel2);

		// Correction des positions pour éviter la pénétration
		double overlap = (m1->size + m2->size) - (pos1 - pos2).magnitude();
		if (overlap > 0) {
			cyclone::Vector3 correction = delta * (overlap / 2.0);
			m1->particle->setPosition(pos1 + correction);
			m2->particle->setPosition(pos2 - correction);
		}
	}
};

#endif