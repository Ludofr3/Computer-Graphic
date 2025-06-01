#pragma once

#include "Mover.h"

class CubeMover : public Mover {
public:
    CubeMover(const cyclone::Vector3& position, cyclone::Vector3& halfSize) {
        std::cout << "CubeMover created at " << position.x << ", " << position.y << ", " << position.z << std::endl;
        body = new cyclone::RigidBody();
        halfSize = halfSize;
        setState(position, cyclone::Quaternion(1, 0, 0, 0), halfSize, cyclone::Vector3(0, 0, 0));
    }

    ~CubeMover() override {
        delete body;
    }

    void update(float duration) override {
        if (body) {
            body->integrate(duration);
            body->calculateDerivedData();
        }
    }

    void draw(int shadow) override {
        if (!body) {
            std::cerr << "CubeMover::draw: body is null!" << std::endl;
            return;
        }

        GLfloat mat[16];
        body->getGLTransform(mat);

        glColor3f(1.0f, 1.0f, 1.0f);  // Couleur par défaut
        if (shadow) {
            glColor4f(0.2f, 0.2f, 0.2f, 0.5f);
        }
        else if (body->getAwake()) {
            glColor3f(density < 1000.0f ? 0.9f : 0.7f, 0.7f, density < 1000.0f ? 0.3f : 1.0f);
        }
        else {
            glColor3f(1.0f, 0.7f, 0.7f);
        }

        glPushMatrix();
        glMultMatrixf(mat);
        glScalef(halfSize.x * 2, halfSize.y * 2, halfSize.z * 2);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    void setState(const cyclone::Vector3& position, const cyclone::Quaternion& orientation,
        const cyclone::Vector3& extents, const cyclone::Vector3& velocity) override {
        halfSize = extents;
        float volume = extents.x * extents.y * extents.z * 8.0;
        float mass = volume * density;

        body->setPosition(position);
        body->setOrientation(orientation);
        body->setVelocity(velocity);
        body->setMass(mass);
        body->setInverseMass(1.0f / mass);

        cyclone::Matrix3 inertiaTensor;
        inertiaTensor.setBlockInertiaTensor(extents, mass);
        body->setInertiaTensor(inertiaTensor);

        cyclone::Matrix3 inverseInertiaTensor;
        inverseInertiaTensor.setInverse(inertiaTensor);
        body->setInverseInertiaTensor(inverseInertiaTensor);

        body->setLinearDamping(0.2f);
        body->setAngularDamping(0.5f);
        body->setAcceleration(cyclone::Vector3(0, -9.81f, 0));
        body->setAwake(true);
        body->calculateDerivedData();
    }
};
