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
#include <body.h>

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
		transformMatrix = cyclone::Matrix4();
	}
	~Mover() {}

	cyclone::Particle *particle;
	//MyAnchoredSpring *spring;
	
	//cyclone::ParticleGravity *gravity;
	cyclone::ParticleDrag *drag;
	cyclone::ParticleForceRegistry *force;
	float size = 0.2f;
	cyclone::Matrix4 transformMatrix;
	cyclone::Quaternion orientation;
		
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
		GLfloat glMat[16];
		getGLTransform(glMat);
		if (shadow == 1) {
			glColor3f(0.1f, 0.1f, 0.1f);
		}
		else {
			glColor3f(1.0f, 0.0f, 0.0f);
		}
		glPushMatrix();
		glMultMatrixf(glMat);
		glutSolidCube(1.0f); //size=1
		drawAxes(0.0f, 0.0f, 0.0f, size * 10.0f, 2.0f);
		glPopMatrix();

	}

	static void drawAxes(float originX, float originY, float originZ,
		float length, float lineWidth = 3.0f)
	{
		glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT);
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glLineWidth(lineWidth);

		glBegin(GL_LINES);
		glColor3f(1, 0, 0);

		glVertex3f(originX, originY, originZ);
		glVertex3f(originX + length, originY, originZ);

		glColor3f(0, 1, 0);

		glVertex3f(originX, originY, originZ);
		glVertex3f(originX, originY + length, originZ);

		glColor3f(0, 0, 1);

		glVertex3f(originX, originY, originZ);
		glVertex3f(originX, originY, originZ + length);
		glEnd();

		glPopAttrib();
	}

	void getGLTransform(float matrix[16])
	{
		matrix[0] = (float)transformMatrix.data[0];
		matrix[1] = (float)transformMatrix.data[4];
		matrix[2] = (float)transformMatrix.data[8];
		matrix[3] = 0;
		matrix[4] = (float)transformMatrix.data[1];
		matrix[5] = (float)transformMatrix.data[5];
		matrix[6] = (float)transformMatrix.data[9];
		matrix[7] = 0;
		matrix[8] = (float)transformMatrix.data[2];
		matrix[9] = (float)transformMatrix.data[6];
		matrix[10] = (float)transformMatrix.data[10];
		matrix[11] = 0;
		matrix[12] = (float)transformMatrix.data[3];
		matrix[13] = (float)transformMatrix.data[7];
		matrix[14] = (float)transformMatrix.data[11];
		matrix[15] = 1;
	}
};
