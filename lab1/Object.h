#pragma once

#include "windows.h"
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glut.h"
#include <particle.h>
#include <iostream>
#include <pfgen.h>
#include <MySpring.h>
#include "Plane.h"

class Mover {
public:
	Mover(cyclone::Vector3 &p) {
		particle = new cyclone::Particle();
		particle->setPosition(p);
		particle->setVelocity(0.0f, 0.0f, 0.0f);
		particle->setDamping(0.9f);
		particle->setMass(1.0f);
		particle->setAcceleration(cyclone::Vector3::GRAVITY);
		particle->clearAccumulator();	

		//spring = new MyAnchoredSpring();
		drag = new cyclone::ParticleDrag(0.1, 0.01);
		force = new cyclone::ParticleForceRegistry();
		force->add(particle, drag);
	}
	~Mover() {}

	cyclone::Particle *particle;
	//MyAnchoredSpring *spring;
	
	//cyclone::ParticleGravity *gravity;
	cyclone::ParticleDrag *drag;
	cyclone::ParticleForceRegistry *force;
	float size = 0.2f;
		
	void setConnection(Mover * a) {
	//	a->spring = spring;
	}

	void update(float duration) {
		//checkEdges();
		force->updateForces(duration);
		particle->integrate(duration);
	}

	void checkEdges() {
		cyclone::Vector3 pos = particle->getPosition();
		cyclone::Vector3 vel = particle->getVelocity();
		if (pos.x - size >= 100.0f) {
			pos.x = pos.x - size;
			vel.x = -vel.x;
		}
		if (pos.z - size >= 100.0f) {
			pos.z = pos.z - size;
			vel.z = -vel.z;
		}
		if (pos.x - size <= -100.0f) {
			pos.x = pos.x + size;
			vel.x = -vel.x;
		}
		if (pos.z - size <= -100.0f) {
			pos.z = pos.z + size;
			vel.z = -vel.z;
		}
		particle->setVelocity(vel);
		particle->setPosition(pos);
	}

	void stop() {}

	void draw(int shadow) {
		cyclone::Vector3 position;
		particle->getPosition(&position);
		if (shadow == 1) {
			glColor3f(0.1f, 0.1f, 0.1f);
		}
		else {
			glColor3f(1.0f, 0.0f, 0.0f);
		}
		glPushMatrix();
		glTranslated(position.x, position.y, position.z);
		glutSolidSphere(size, 30, 30);
		glPopMatrix();
	}
};