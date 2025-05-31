#include "Fluid.h"
#include "Object.h" // Pour la classe Mover

cyclone::Vector3 Fluid::getArchimedesForce(const Mover& mover) const {
    cyclone::Vector3 position = mover.body->getPosition();

    // Calculer les dimensions réelles de l'objet
    float width = mover.halfSize.x * 2.0f;
    float height = mover.halfSize.y * 2.0f;
    float depth = mover.halfSize.z * 2.0f;

    // Calculer le volume total de l'objet
    float volume = width * height * depth;

    // Vérifier si l'objet est dans la zone du fluide
    if (fabs(position.x) > size / 2 || fabs(position.z) > size / 2) {
        return cyclone::Vector3(0, 0, 0);
    }

    // Si l'objet est complètement au-dessus du fluide
    if (position.y - mover.halfSize.y >= level) {
        return cyclone::Vector3(0, 0, 0);
    }

    // Si l'objet est complètement sous le fluide
    if (position.y + mover.halfSize.y <= level) {
        float forceMagnitude = density * volume * 9.81f;
        return cyclone::Vector3(0, forceMagnitude, 0);
    }

    // Cas partiellement immergé
    float immersedHeight = level - (position.y - mover.halfSize.y);
    float totalHeight = height;
    float immersedRatio = immersedHeight / totalHeight;
    float immersedVolume = volume * immersedRatio;

    float forceMagnitude = density * immersedVolume * 9.81f;
    return cyclone::Vector3(0, forceMagnitude, 0);
}

cyclone::Vector3 Fluid::getDragForce(const Mover& mover) const {
    cyclone::Vector3 velocity = mover.body->getVelocity();
    double speed = velocity.magnitude();

    if (speed < 0.001) return cyclone::Vector3(0, 0, 0);

    // Force de traînée proportionnelle au carré de la vitesse
    double dragMagnitude = dragCoefficient * speed * speed;
    cyclone::Vector3 dragDirection = -1 * velocity.unit();

    return dragDirection * dragMagnitude;
}