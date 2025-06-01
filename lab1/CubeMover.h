#pragma once

#include "Mover.h"

class CubeMover : public Mover {
public:
    CubeMover(const cyclone::Vector3& position,
        const cyclone::Vector3& halfSizeXYZ,
        float densityArg) {
        // 1) Allouer le RigidBody
        body = new cyclone::RigidBody();

        // 2) Remplir halfSize, density
        halfSize = halfSizeXYZ;
        density = densityArg;

        // 3) Initialisation du corps rigide (masse, inertie, etc.)
        setState(position,
            cyclone::Quaternion(1, 0, 0, 0),
            halfSizeXYZ,
            cyclone::Vector3(0, 0, 0));
    }

    /// Surcharge : si on veut appeler sans préciser la densité,
    /// on met par défaut 500 kg/m³ (pour rétrocompatibilité).
    CubeMover(const cyclone::Vector3& position,
        const cyclone::Vector3& halfSizeXYZ)
        : CubeMover(position, halfSizeXYZ, 500.0f)
    {}

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
        if (!body) return;

        // Récupérer la matrice de transformation (4×4) pour l’objet
        GLfloat mat[16];
        body->getGLTransform(mat);

        // Choix de la couleur selon l’état et la densité
        if (shadow) {
            glColor4f(0.2f, 0.2f, 0.2f, 0.5f);
        }
        else if (body->getAwake()) {
            if (density < 1000.0f) {
                glColor3f(0.9f, 0.7f, 0.3f);
            }
            else {
                glColor3f(0.7f, 0.7f, 1.0f);
            }
        }
        else {
            glColor3f(1.0f, 0.7f, 0.7f);
        }

        glPushMatrix();
        glMultMatrixf(mat);
        glScalef(halfSize.x * 2.0f,
            halfSize.y * 2.0f,
            halfSize.z * 2.0f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    void setState(const cyclone::Vector3& position, const cyclone::Quaternion& orientation,
        const cyclone::Vector3& extents, const cyclone::Vector3& velocity) override {
        halfSize = extents;
        float volume = extents.x * extents.y * extents.z * 8.0;
        std::cout << "Density: " << density << std::endl;
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
