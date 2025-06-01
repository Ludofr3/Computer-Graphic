// SphereMover.h
#pragma once

#include "Sphere.h"

class SphereMover : public Sphere {
public:
    /// Construit une sphère de centre 'position' et de rayon 'r'
    SphereMover(const cyclone::Vector3& position, float r)
        : radius(r)
    {
        body = new cyclone::RigidBody();
        // On considère que halfSize.x = halfSize.y = halfSize.z = r pour la bounding box
        halfSize = cyclone::Vector3(r, r, r);
        density = 500.0f;  // exemple de densité
        setState(position, cyclone::Quaternion(1, 0, 0, 0), halfSize, cyclone::Vector3(0, 0, 0));
    }

    ~SphereMover() override {
        delete body;
    }

    // Implémentation de getRadius / setRadius
    float getRadius() const override {
        return radius;
    }
    void setRadius(float r) override {
        radius = r;
        halfSize = cyclone::Vector3(r, r, r);
        // (on pourrait mettre à jour le rigidbody si nécessaire)
    }

    // Intégration physique
    void update(float duration) override {
        if (body) {
            body->integrate(duration);
            body->calculateDerivedData();
        }
    }

    // Dessin OpenGL d’une sphère
    void draw(int shadow) override {
        if (!body) return;

        GLfloat mat[16];
        body->getGLTransform(mat);

        if (shadow) {
            glColor4f(0.2f, 0.2f, 0.2f, 0.5f);
        }
        else if (body->getAwake()) {
            // On colorie en fonction de la densité, comme pour CubeMover
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
        // On dessine la sphère de rayon 'radius'
        glutSolidSphere(radius, 20, 20);
        glPopMatrix();
    }

    // Initialisation du corps rigide pour une sphère
    void setState(const cyclone::Vector3& position,
        const cyclone::Quaternion& orientation,
        const cyclone::Vector3& /*unused extents pour une sphère*/,
        const cyclone::Vector3& velocity) override
    {
        // Pour une sphère, on n’utilise que 'radius' plutôt que 'extents'
        float volume = (4.0f / 3.0f) * 3.1415926f * radius * radius * radius;
        float mass = volume * density;

        body->setPosition(position);
        body->setOrientation(orientation);
        body->setVelocity(velocity);
        body->setMass(mass);
        body->setInverseMass(1.0f / mass);

        // Inertie pour une sphère uniforme : (2/5) m r^2 * identité
        float I = (2.0f / 5.0f) * mass * radius * radius;
        cyclone::Matrix3 inertiaTensor;
        inertiaTensor.clear();
        inertiaTensor.data[0] = I;
        inertiaTensor.data[5] = I;
        inertiaTensor.data[10] = I;
        body->setInertiaTensor(inertiaTensor);

        // Inverse de la matrice d’inertie
        cyclone::Matrix3 invInertia;
        invInertia.clear();
        invInertia.data[0] = 1.0f / I;
        invInertia.data[5] = 1.0f / I;
        invInertia.data[10] = 1.0f / I;
        body->setInverseInertiaTensor(invInertia);

        body->setLinearDamping(0.2f);
        body->setAngularDamping(0.5f);
        body->setAcceleration(cyclone::Vector3(0, -9.81f, 0));
        body->setAwake(true);
        body->calculateDerivedData();
    }

private:
    float radius;
};
