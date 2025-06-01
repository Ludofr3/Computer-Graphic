#pragma once

#include "Mover.h"

#define NOMINMAX
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

#include "core.h"

/// Classe concrète dérivée de Mover pour dessiner et simuler
/// un ellipsoïde (ou une sphère si rx=ry=rz).
class EllipsoidMover : public Mover {
public:
    EllipsoidMover(const cyclone::Vector3& position,
        float rx, float ry, float rz,
        float densityArg)
        : radiusX(rx)
        , radiusY(ry)
        , radiusZ(rz)
    {
        body = new cyclone::RigidBody();

        // halfSize sert seulement pour le bounding box dans Mover (si besoin)
        halfSize = cyclone::Vector3(rx, ry, rz);

        // On assigne la densité passée
        density = densityArg;

        // Initialiser le corps rigide
        setState(position,
            cyclone::Quaternion(1, 0, 0, 0),
            halfSize,                    // extents (inutile ici pour le calcul inertie)
            cyclone::Vector3(0, 0, 0));  // vitesse initiale nulle
    }

    /// Surcharge pour cas « sphère simple » si on passe rayon unique
    EllipsoidMover(const cyclone::Vector3& position,
        float radius,
        float densityArg)
        : EllipsoidMover(position, radius, radius, radius, densityArg)
    {}

    ~EllipsoidMover() override {
        delete body;
    }

    // --- Accesseurs sur les demi‐axes ---
    float getRadiusX() const { return radiusX; }
    float getRadiusY() const { return radiusY; }
    float getRadiusZ() const { return radiusZ; }

    void setRadiusX(float rx) {
        radiusX = rx;
        halfSize.x = rx;
        // Si on veut appliquer la mise à jour du corps rigide :
        // setState(body->getPosition(), body->getOrientation(), halfSize, body->getVelocity());
    }
    void setRadiusY(float ry) {
        radiusY = ry;
        halfSize.y = ry;
    }
    void setRadiusZ(float rz) {
        radiusZ = rz;
        halfSize.z = rz;
    }

    /// Mise à jour physique à chaque frame
    void update(float duration) override {
        if (!body) return;
        body->integrate(duration);
        body->calculateDerivedData();
    }

    /// Dessin OpenGL : on dessine un ellipsoïde en
    /// appliquant d’abord une matrice de transformation,
    /// puis un scaled‐sphere (glutSolidSphere).
    void draw(int shadow) override {
        if (!body) return;

        GLfloat mat[16];
        body->getGLTransform(mat);

        // Choix de la couleur (identique au CubeMover ou SphereMover)
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
        // 1) Appliquer la matrice de transformation du rigidbody
        glMultMatrixf(mat);

        // 2) Échelle non uniforme pour passer d'une sphère unité à notre ellipsoïde
        glScalef(radiusX, radiusY, radiusZ);

        // 3) Dessiner une sphère de rayon 1 (la mise à l’échelle fait le reste)
        //    Avec 20 slices/stacks pour avoir une bonne résolution.
        glutSolidSphere(1.0f, 20, 20);

        glPopMatrix();
    }

    /// Initialisation du rigidbody : position, inertie, masse, etc.
    void setState(const cyclone::Vector3& position,
        const cyclone::Quaternion& orientation,
        const cyclone::Vector3& /*extents non utilisé ici*/,
        const cyclone::Vector3& velocity) override
    {
        // 1) Calcul de la masse à partir du volume d'un ellipsoïde :
        //    Volume = (4/3) * π * rx * ry * rz
        float volume = (4.0f / 3.0f) * static_cast<float>(M_PI)
            * radiusX * radiusY * radiusZ;
        float mass = volume * density;

        body->setPosition(position);
        body->setOrientation(orientation);
        body->setVelocity(velocity);
        body->setMass(mass);

        // Définir l'inverse de la masse
        if (mass > 0.0f) {
            body->setInverseMass(1.0f / mass);
        }
        else {
            body->setInverseMass(0.0f);
        }

        // 2) Calcul du tenseur d'inertie pour un ellipsoïde homogène :
        //
        //    I_x = (1/5) m (ry^2 + rz^2)
        //    I_y = (1/5) m (rx^2 + rz^2)
        //    I_z = (1/5) m (rx^2 + ry^2)
        float Ixx = (1.0f / 5.0f) * mass * (radiusY * radiusY + radiusZ * radiusZ);
        float Iyy = (1.0f / 5.0f) * mass * (radiusX * radiusX + radiusZ * radiusZ);
        float Izz = (1.0f / 5.0f) * mass * (radiusX * radiusX + radiusY * radiusY);

        // 3) On crée une Matrix3 vide (toutes cases = 0), puis on place les valeurs diagonales
        cyclone::Matrix3 inertiaTensor;
        for (int i = 0; i < 9; i++) {
            inertiaTensor.data[i] = 0.0f;
        }
        inertiaTensor.data[0] = Ixx; // (0,0)
        inertiaTensor.data[4] = Iyy; // (1,1)
        inertiaTensor.data[8] = Izz; // (2,2)

        body->setInertiaTensor(inertiaTensor);

        // 4) Calcul du tenseur d'inertie inverse
        //    Comme la matrice est diagonale, il suffit d'inverser Ixx, Iyy, Izz
        cyclone::Matrix3 invInertia;
        for (int i = 0; i < 9; i++) {
            invInertia.data[i] = 0.0f;
        }
        if (Ixx > 0.0f) invInertia.data[0] = 1.0f / Ixx;
        if (Iyy > 0.0f) invInertia.data[4] = 1.0f / Iyy;
        if (Izz > 0.0f) invInertia.data[8] = 1.0f / Izz;
        body->setInverseInertiaTensor(invInertia);

        // 5) Paramètres additionnels (damping, gravité, etc.)
        body->setLinearDamping(0.2f);
        body->setAngularDamping(0.5f);
        body->setAcceleration(cyclone::Vector3(0, -9.81f, 0));
        body->setAwake(true);
        body->calculateDerivedData();
    }

private:
    float radiusX, radiusY, radiusZ;
};
