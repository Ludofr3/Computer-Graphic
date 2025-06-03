#pragma once
#include <cyclone.h>

struct BuoyancyResult {
    cyclone::Vector3 force;      // vecteur de pouss�e
    cyclone::Vector3 applicationPoint; // centre de pouss�e en world
};

class Fluid {
public:
    double density; // Densit� du fluide en kg/m�
    double level;   // Niveau du fluide (hauteur y)
    double dragCoefficient; // Coefficient de tra�n�e dans le fluide
    double size;

    Fluid(double density = 1000.0, double level = 10.0, double drag = 0.5, double size = 50.0)
        : density(density), level(level), dragCoefficient(drag), size(size) {}

    // M�thode pour obtenir la force d'Archim�de agissant sur un objet
    cyclone::Vector3 getArchimedesForce(const class Mover& mover) const;

    // M�thode pour obtenir la force de tra�n�e dans le fluide
    cyclone::Vector3 getDragForce(const class Mover& mover) const;

    BuoyancyResult getBuoyancy(const Mover& m) const;
};