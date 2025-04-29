#ifndef __MOVERCONNECTION
#define __MOVERCONNECTION

#include <particle.h>
#include <iostream>
#include <pfgen.h>
#include <MySpring.h>
#include "Object.h"

class MoverConnection {
public:
	MoverConnection() {
		forces = new cyclone::ParticleForceRegistry();
		movers = { new Mover(cyclone::Vector3(3, 19, 0)) };
		planes = { new Plane() };
		gravity = new cyclone::ParticleGravity(cyclone::Vector3(0, -10, 0));

		// Initialisation des springs avec ancres distinctes
		//movers[0]->spring->init(cyclone::Vector3(5, 15, 5), 5.0f, 3.0f);
		//movers[1]->spring->init(cyclone::Vector3(15, 15, 10), 5.0f, 3.0f);

		for (int i = 0; i < movers.size(); i++) {
			forces->add(movers[i]->particle, gravity);
			//forces->add(movers[i]->particle, movers[i]->spring);
		}

		//movers[0]->spring = new cyclone::Myspring(movers[0]->particle, 20.0f, 3.0f);
		//movers[1]->spring = new cyclone::Myspring(movers[1]->particle, 20.0f, 3.0f);
		//movers[0]->setConnection(movers[1]);

		//forces->add(movers[0]->particle, movers[1]->spring);
		//forces->add(movers[1]->particle, movers[0]->spring);
	}
	~MoverConnection() {}

	cyclone::ParticleGravity* gravity;
	cyclone::ParticleForceRegistry* forces;
	std::vector<Mover*> movers;
	std::vector<Plane*> planes;

	void update(float duration) {
		forces->updateForces(duration);
		for (int i = 0; i < movers.size(); i++) {
			movers[i]->update(duration, planes);
		}
		checkCollisions();
	}

	void MoverConnection::draw(int shadow)
	{
		for (unsigned int i = 0; i < movers.size(); i++) {
			if (!shadow)
				glLoadName(i + 1);
			movers[i]->draw(shadow);
		}

		//glBegin(GL_LINE_STRIP);
		glColor3f(0, 1, 0);
		glLineWidth(2.0f);
		glBegin(GL_LINES);
		for (unsigned int i = 0; i < movers.size(); i++) {
			cyclone::Vector3 position = movers[i]->particle->getPosition();
			//cyclone::Vector3 anchor = movers[i]->spring->getAnchor();
			//glVertex3f(anchor.x, anchor.y, anchor.z); //Starting point
			glVertex3f(position.x, position.y, position.z); //Ending point
		}
		glEnd();
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