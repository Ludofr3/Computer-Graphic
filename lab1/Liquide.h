#pragma once

#include <DrawUtils.H>
#include <Object.h>
#include <body.h>
#include <pfgen.h>

class Liquid {
private:
    const cyclone::Vector3 p1;
    const cyclone::Vector3 p2;
    const cyclone::Vector3 p3;
    const cyclone::Vector3 p4;
    const std::vector<GLfloat> color;
    const cyclone::Vector3 n;
    float liquidDensity;  // Densité du liquide (kg/m³)
    float viscosity;      // Viscosité pour la résistance

public:
    Liquid(float density = 1000.0f, float visc = 0.1f) :
        p1(cyclone::Vector3(20., 0., -25.)),
        p2(cyclone::Vector3(20., 0., 25.)),
        p3(cyclone::Vector3(-20., 30., 25.)),
        p4(cyclone::Vector3(-20., 30., -25.)),
        color({ 0.2f, 0.6f, 1.0f, 0.6f }), // Couleur bleue transparente pour l'eau
        n(normal()),
        liquidDensity(density),
        viscosity(visc) {}

    cyclone::Vector3 normal() {
        std::cout << "================ normal ====================" << std::endl;
        cyclone::Vector3 v1 = p2 - p1;
        cyclone::Vector3 v2 = p3 - p1;
        if (v1.magnitude() < 0 || v2.magnitude() < 0) {
            return cyclone::Vector3(0, 1, 0); // Retourne une verticale par défaut
        }
        v1.normalise();
        v2.normalise();
        cyclone::Vector3 nrml = v1.cross(v2) * -1;
        if (nrml.magnitude() < 0)
            return cyclone::Vector3(0, 1, 0);
        nrml.normalise();
        std::cout << "================= normal End =================" << std::endl;
        return nrml;
    }

    ~Liquid() {}

    void draw() {
        // Dessiner le liquide avec transparence
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(color.at(0), color.at(1), color.at(2), color.at(3));
        polygonf(4, p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, p3.x, p3.y, p3.z, p4.x, p4.y, p4.z);
        glDisable(GL_BLEND);
    }

    // Calcule la distance depuis la surface du liquide
    float distanceFromSurface(cyclone::Vector3 pos) {
        cyclone::Vector3 v = pos - p1;
        return v.dot(n);
    }

    // Vérifie si l'objet est dans les limites horizontales du liquide
    bool isInLiquidBounds(cyclone::Vector3 pos) {
        return p1.x >= pos.x && pos.x >= p3.x && p1.z <= pos.z && pos.z <= p3.z;
    }

    // Vérifie si l'objet est submergé (partiellement ou totalement)
    bool isSubmerged(cyclone::Vector3 pos, float objectRadius) {
        return (distanceFromSurface(pos) <= objectRadius && isInLiquidBounds(pos));
    }

    // Calcule le volume submergé d'une sphère
    float getSubmergedVolume(cyclone::Vector3 pos, float radius) {
        if (!isInLiquidBounds(pos)) return 0.0f;

        float h = distanceFromSurface(pos);

        if (h >= radius) {
            // Complètement au-dessus du liquide
            return 0.0f;
        }
        else if (h <= -radius) {
            // Complètement submergé
            return (4.0f / 3.0f) * 3.14159f * radius * radius * radius;
        }
        else {
            // Partiellement submergé - calcul du volume de la calotte sphérique
            float submergedHeight = radius - h;
            return 3.14159f * submergedHeight * submergedHeight * (3.0f * radius - submergedHeight) / 3.0f;
        }
    }

    // Calcule le volume submergé d'un cube (approximation)
    float getSubmergedVolumeBox(cyclone::Vector3 pos, cyclone::Vector3 halfSize) {
        if (!isInLiquidBounds(pos)) return 0.0f;

        float h = distanceFromSurface(pos);
        float boxHeight = halfSize.y * 2.0f;
        float boxTop = pos.y + halfSize.y;
        float boxBottom = pos.y - halfSize.y;

        // Surface du liquide (approximation - on suppose que la surface est à y = p1.y + h)
        float liquidSurface = p1.y;

        if (boxBottom >= liquidSurface) {
            // Complètement au-dessus
            return 0.0f;
        }
        else if (boxTop <= liquidSurface) {
            // Complètement submergé
            return halfSize.x * halfSize.y * halfSize.z * 8.0f; // Volume total
        }
        else {
            // Partiellement submergé
            float submergedHeight = liquidSurface - boxBottom;
            float submergedRatio = submergedHeight / boxHeight;
            return halfSize.x * halfSize.y * halfSize.z * 8.0f * submergedRatio;
        }
    }

    float getLiquidDensity() const { return liquidDensity; }
    float getViscosity() const { return viscosity; }
    cyclone::Vector3 getNormal() const { return n; }
};

// Générateur de force pour la poussée d'Archimède
class ArchimedesForceGenerator : public cyclone::ForceGenerator {
private:
    Liquid* liquid;

public:
    ArchimedesForceGenerator(Liquid* l) : liquid(l) {}

    virtual void updateForce(cyclone::RigidBody* body, cyclone::real duration) {
        if (!body || !liquid) return;

        cyclone::Vector3 pos = body->getPosition();
        cyclone::Vector3 velocity = body->getVelocity();

        // Approximation: on utilise la racine cubique de la masse pour estimer la taille
        float mass = body->getMass();
        float estimatedRadius = std::pow(mass / (8.0f * 1000.0f), 1.0f / 3.0f); // Densité approximative de 1000 kg/m³

        if (liquid->isSubmerged(pos, estimatedRadius)) {
            // Calcul du volume submergé
            float submergedVolume = liquid->getSubmergedVolume(pos, estimatedRadius);

            if (submergedVolume > 0.0f) {
                // Force de poussée d'Archimède : F = ρ_liquide * V_submergé * g
                float buoyantForce = liquid->getLiquidDensity() * submergedVolume * 9.81f;
                cyclone::Vector3 buoyancy(0, buoyantForce, 0);

                // Force de traînée dans le liquide (résistance visqueuse)
                cyclone::Vector3 drag = velocity * (-liquid->getViscosity() * submergedVolume);

                // Application des forces
                body->addForce(buoyancy);
                body->addForce(drag);
            }
        }
    }
};

// Version pour les objets avec halfSize (comme vos Mover)
class ArchimedesForceGeneratorBox : public cyclone::ForceGenerator {
private:
    Liquid* liquid;
    cyclone::Vector3 halfSize;

public:
    ArchimedesForceGeneratorBox(Liquid* l, cyclone::Vector3 hs) : liquid(l), halfSize(hs) {}

    virtual void updateForce(cyclone::RigidBody* body, cyclone::real duration) {
        if (!body || !liquid) return;

        cyclone::Vector3 pos = body->getPosition();
        cyclone::Vector3 velocity = body->getVelocity();

        if (liquid->isInLiquidBounds(pos)) {
            // Calcul du volume submergé pour un cube
            float submergedVolume = liquid->getSubmergedVolumeBox(pos, halfSize);

            if (submergedVolume > 0.0f) {
                // Force de poussée d'Archimède : F = ρ_liquide * V_submergé * g
                float buoyantForce = liquid->getLiquidDensity() * submergedVolume * 9.81f;
                cyclone::Vector3 buoyancy(0, buoyantForce, 0);

                // Force de traînée dans le liquide
                cyclone::Vector3 drag = velocity * (-liquid->getViscosity() * submergedVolume * 0.01f);

                // Application des forces
                body->addForce(buoyancy);
                body->addForce(drag);

                // Debug output (optionnel)
                // std::cout << "Submerged volume: " << submergedVolume << ", Buoyant force: " << buoyantForce << std::endl;
            }
        }
    }

    void setHalfSize(cyclone::Vector3 hs) { halfSize = hs; }
};