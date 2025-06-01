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
	cyclone::Matrix4 transformMatrix;  // Matrice de transformation
	cyclone::RigidBody* body;
	cyclone::Vector3 halfSize;
	cyclone::ParticleDrag* drag;
	cyclone::ParticleForceRegistry* force;
	float size = 1.0f;
	float density;

	Mover(cyclone::Vector3& p) {

		body = new cyclone::RigidBody();
		halfSize = cyclone::Vector3(1.0f, 1.0f, 2.0f);
		density = 500.0f;
		setState(p, cyclone::Quaternion(1, 0, 0, 0), halfSize, cyclone::Vector3(0, 0, 0));
	}

	void addTorque(cyclone::Vector3 force, cyclone::Vector3 point) {
		cyclone::Vector3 com = body->getPosition();
		cyclone::Vector3 d = point - com;
		cyclone::Vector3 torque = d.cross(force);
		body->addTorque(torque);
	}

	void update(float duration) {
		if (body) {
			body->integrate(duration);
			body->calculateDerivedData();
		}
	}

	~Mover() {
		delete body;
	}

	void stop() {}

	void draw(int shadow) {
		GLfloat mat[16];
		body->getGLTransform(mat);

		if (shadow) {
			glColor4f(0.2f, 0.2f, 0.2f, 0.5f);
		}
		else if (body->getAwake()) {
			// Couleur différente selon la densité (visualisation)
			if (density < 1000.0f) {
				glColor3f(0.9f, 0.7f, 0.3f);  // Orange pour objets qui flottent
			}
			else {
				glColor3f(0.7f, 0.7f, 1.0f);  // Bleu pour objets lourds
			}
		}
		else {
			glColor3f(1.0f, 0.7f, 0.7f);
		}

		glPushMatrix();
		glMultMatrixf(mat);
		glScalef(halfSize.x * 2, halfSize.y * 2, halfSize.z * 2);
		glutSolidCube(1.0f);
		//drawAxes(0.0f, 0.0f, 0.0f, size * 10.0f, 2.0f);
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

	void setState(const cyclone::Vector3& position, const cyclone::Quaternion& orientation,
		const cyclone::Vector3& extents, const cyclone::Vector3& velocity) {
		body->setPosition(position);
		body->setOrientation(orientation);
		body->setVelocity(velocity);

		float volume = extents.x * extents.y * extents.z * 8.0;
		float mass = volume * density;
		body->setMass(mass);

		// S'assurer que la masse inverse est calculée correctement
		if (mass > 0.0f) {
			body->setInverseMass(1.0f / mass);
		}

		cyclone::Matrix3 inertiaTensor;
		inertiaTensor.setBlockInertiaTensor(extents, mass);
		body->setInertiaTensor(inertiaTensor);

		// Calculer la matrice d'inertie inverse
		cyclone::Matrix3 inverseInertiaTensor;
		inverseInertiaTensor.setInverse(inertiaTensor);
		body->setInverseInertiaTensor(inverseInertiaTensor);

		body->setLinearDamping(0.2f);
		body->setAngularDamping(0.5f);
		body->setAcceleration(cyclone::Vector3(0, -9.81f, 0)); // Gravité

		// Important : s'assurer que le corps est réveillé
		body->setAwake(true);
		body->calculateDerivedData();
	}
	
	void reset() {
		body->clearAccumulators();
		body->setVelocity(0, 0, 0);
		body->setRotation(0, 0, 0);
		body->setAwake(true);
	}
};