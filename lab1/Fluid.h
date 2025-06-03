#pragma once
#include <cyclone.h>

struct BuoyancyResult {
    cyclone::Vector3 force;      // vecteur de poussée
    cyclone::Vector3 applicationPoint; // centre de poussée en world
};

class Fluid {
public:
    double density; // Densité du fluide en kg/m³
    double level;   // Niveau du fluide (hauteur y)
    double dragCoefficient; // Coefficient de traînée dans le fluide
    double size;

    Fluid(double density = 1000.0, double level = 10.0, double drag = 0.5, double size = 50.0)
        : density(density), level(level), dragCoefficient(drag), size(size) {}

    // Méthode pour obtenir la force d'Archimède agissant sur un objet
    cyclone::Vector3 getArchimedesForce(const class Mover& mover) const;

    // Méthode pour obtenir la force de traînée dans le fluide
    cyclone::Vector3 getDragForce(const class Mover& mover) const;

    BuoyancyResult getBuoyancy(const Mover& m) const;
};