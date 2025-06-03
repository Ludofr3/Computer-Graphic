// PyramidMover.h
#pragma once

#include "Mover.h"
#include <cmath>  // pour M_PI

/// PyramidMover : un objet pyramide à base rectangulaire.
/// Hérite de Mover et implémente directement toutes les méthodes pures.
class PyramidMover : public Mover {
public:
    /// Constructeur :
    /// - position   : centre de la pyramide dans l’espace monde
    /// - baseWidth  : demi‐largeur de la base (axe X)
    /// - baseDepth  : demi‐profondeur de la base (axe Z)
    /// - height     : hauteur totale de la pyramide (axe Y)
    /// - densityArg : densité en kg/m³
    PyramidMover(const cyclone::Vector3& position,
        float baseWidth,
        float baseDepth,
        float height,
        float densityArg)
        : halfWidth(baseWidth)
        , halfDepth(baseDepth)
        , heightY(height)
    {
        body = new cyclone::RigidBody();

        // halfSize peut servir pour une boîte englobante, par exemple dans d'autres calculs
        halfSize = cyclone::Vector3(baseWidth, height / 2.0f, baseDepth);

        density = densityArg;

        // Initialisation du corps rigide
        setState(position,
            cyclone::Quaternion(1, 0, 0, 0),  // orientation neutre
            halfSize,                         // extents (inutile pour le calcul, mais on le passe)
            cyclone::Vector3(0, 0, 0));       // vitesse initiale nulle
    }

    ~PyramidMover() override {
        delete body;
    }

    // ------------------------------------------------------------
    // Hérité de Mover : intégration physique
    void update(float duration) override {
        if (!body) return;
        body->integrate(duration);
        body->calculateDerivedData();
    }

    // ------------------------------------------------------------
    // Hérité de Mover : dessin OpenGL de la pyramide
    void draw(int shadow) override {
        if (!body) return;

        GLfloat mat[16];
        body->getGLTransform(mat);

        // Choix de la couleur selon l’état du rigidbody et la densité
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

        // 1) Faces latérales (4 triangles)
        glBegin(GL_TRIANGLES);
        // Face avant
        glVertex3f(-halfWidth, 0.0f, -halfDepth);
        glVertex3f(halfWidth, 0.0f, -halfDepth);
        glVertex3f(0.0f, heightY, 0.0f);
        // Face droite
        glVertex3f(halfWidth, 0.0f, -halfDepth);
        glVertex3f(halfWidth, 0.0f, halfDepth);
        glVertex3f(0.0f, heightY, 0.0f);
        // Face arrière
        glVertex3f(halfWidth, 0.0f, halfDepth);
        glVertex3f(-halfWidth, 0.0f, halfDepth);
        glVertex3f(0.0f, heightY, 0.0f);
        // Face gauche
        glVertex3f(-halfWidth, 0.0f, halfDepth);
        glVertex3f(-halfWidth, 0.0f, -halfDepth);
        glVertex3f(0.0f, heightY, 0.0f);
        glEnd();

        // 2) Base (quad)
        glBegin(GL_QUADS);
        glVertex3f(-halfWidth, 0.0f, -halfDepth);
        glVertex3f(halfWidth, 0.0f, -halfDepth);
        glVertex3f(halfWidth, 0.0f, halfDepth);
        glVertex3f(-halfWidth, 0.0f, halfDepth);
        glEnd();

        glPopMatrix();
    }

    // ------------------------------------------------------------
    // Hérité de Mover : initialisation du rigidbody (masse + inertie)
    void setState(const cyclone::Vector3& position,
        const cyclone::Quaternion& orientation,
        const cyclone::Vector3& /*extents non utilisé pour pyramide*/,
        const cyclone::Vector3& velocity) override
    {
        // 1) Calcul du volume d’une pyramide à base rectangulaire :
        //    Volume = (1/3) × (largeur totale) × (profondeur totale) × (hauteur)
        //           = (1/3) × (2a) × (2b) × h
        float a = halfWidth;
        float b = halfDepth;
        float h = heightY;
        float volume = (1.0f / 3.0f) * (2.0f * a) * (2.0f * b) * h;
        float mass = volume * density;

        body->setPosition(position);
        body->setOrientation(orientation);
        body->setVelocity(velocity);
        body->setMass(mass);
        if (mass > 0.0f) {
            body->setInverseMass(1.0f / mass);
        }
        else {
            body->setInverseMass(0.0f);
        }

        // 2) Tenseur d’inertie pour la pyramide rectangulaire homogène :
        //    I_x = (1/20) m (b² + h²)
        //    I_y = (1/10) m (a² + b²)
        //    I_z = (1/20) m (a² + h²)
        float Ixx = (1.0f / 20.0f) * mass * (b * b + h * h);
        float Iyy = (1.0f / 10.0f) * mass * (a * a + b * b);
        float Izz = (1.0f / 20.0f) * mass * (a * a + h * h);

        cyclone::Matrix3 inertiaTensor;
        for (int i = 0; i < 9; i++) inertiaTensor.data[i] = 0.0f;
        inertiaTensor.data[0] = Ixx;  // (0,0)
        inertiaTensor.data[4] = Iyy;  // (1,1)
        inertiaTensor.data[8] = Izz;  // (2,2)
        body->setInertiaTensor(inertiaTensor);

        // 3) Tenseur d’inertie inverse (diagonale)
        cyclone::Matrix3 invInertia;
        for (int i = 0; i < 9; i++) invInertia.data[i] = 0.0f;
        if (Ixx > 0.0f) invInertia.data[0] = 1.0f / Ixx;
        if (Iyy > 0.0f) invInertia.data[4] = 1.0f / Iyy;
        if (Izz > 0.0f) invInertia.data[8] = 1.0f / Izz;
        body->setInverseInertiaTensor(invInertia);

        // 4) Paramètres additionnels
        body->setLinearDamping(0.2f);
        body->setAngularDamping(0.5f);
        body->setAcceleration(cyclone::Vector3(0, -9.81f, 0));
        body->setAwake(true);
        body->calculateDerivedData();
    }

private:
    float halfWidth;   // demi‐largeur de la base (axe X)
    float halfDepth;   // demi‐profondeur de la base (axe Z)
    float heightY;     // hauteur (axe Y)
};
