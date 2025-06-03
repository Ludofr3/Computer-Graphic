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
    cyclone::Matrix4 transformMatrix;
    cyclone::RigidBody* body;
    cyclone::Vector3 halfSize;
    cyclone::ParticleDrag* drag;
    cyclone::ParticleForceRegistry* force;
    float size = 1.0f;
    float density;

    Mover() : body(nullptr), drag(nullptr), force(nullptr), size(1.0f), density(0.0f) {}

    virtual ~Mover() = 0; // Classe abstraite

    virtual void addTorque(cyclone::Vector3 force, cyclone::Vector3 point);
    virtual void update(float duration) = 0;
    virtual void draw(int shadow) = 0;
    virtual void setState(const cyclone::Vector3& position, const cyclone::Quaternion& orientation,
        const cyclone::Vector3& extents, const cyclone::Vector3& velocity) = 0;

    void reset();
    static void drawAxes(float originX, float originY, float originZ, float length, float lineWidth = 3.0f);
};

inline Mover::~Mover() {}  // Définition nécessaire même si pure

inline void Mover::addTorque(cyclone::Vector3 force, cyclone::Vector3 point) {
    cyclone::Vector3 com = body->getPosition();
    cyclone::Vector3 d = point - com;
    cyclone::Vector3 torque = d.cross(force);
    body->addTorque(torque);
}

inline void Mover::reset() {
    body->clearAccumulators();
    body->setVelocity(0, 0, 0);
    body->setRotation(0, 0, 0);
    body->setAwake(true);
}
