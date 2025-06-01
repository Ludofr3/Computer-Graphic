#include "Fluid.h"
#include "Mover.h"

static const int BUOY_SAMPLES_PER_AXIS = 6;

BuoyancyResult Fluid::getBuoyancy(const Mover& m) const {
    BuoyancyResult result;
    result.force = cyclone::Vector3(0, 0, 0);
    result.applicationPoint = cyclone::Vector3(0, 0, 0);

    const cyclone::RigidBody* body = m.body;
    if (!body) return result;

    cyclone::Vector3 halfSize = m.halfSize;
    float dx = (halfSize.x * 2.0f) / (BUOY_SAMPLES_PER_AXIS - 1);
    float dy = (halfSize.y * 2.0f) / (BUOY_SAMPLES_PER_AXIS - 1);
    float dz = (halfSize.z * 2.0f) / (BUOY_SAMPLES_PER_AXIS - 1);

    cyclone::Vector3 positionBody = body->getPosition();
    cyclone::Quaternion orientation = body->getOrientation();
    float glMat[16];
    body->getGLTransform(glMat);

    int countInside = 0;
    cyclone::Vector3 sumPositions(0, 0, 0);

    for (int ix = 0; ix < BUOY_SAMPLES_PER_AXIS; ix++) {
        float lx = -halfSize.x + ix * dx; // coord. locale X
        for (int iy = 0; iy < BUOY_SAMPLES_PER_AXIS; iy++) {
            float ly = -halfSize.y + iy * dy; // coord. locale Y
            for (int iz = 0; iz < BUOY_SAMPLES_PER_AXIS; iz++) {
                float lz = -halfSize.z + iz * dz; // coord. locale Z

                cyclone::Vector3 localPoint(lx, ly, lz);

                cyclone::Vector3 worldPoint;
                worldPoint.x = glMat[0]  * lx + glMat[4]  * ly + glMat[8]  * lz + glMat[12];
                worldPoint.y = glMat[1]  * lx + glMat[5]  * ly + glMat[9]  * lz + glMat[13];
                worldPoint.z = glMat[2]  * lx + glMat[6]  * ly + glMat[10] * lz + glMat[14];

                if (worldPoint.y < level) {
                    countInside++;
                    sumPositions += worldPoint;
                }
            }
        }
    }

    if (countInside == 0) {
        return result;
    }

    cyclone::Vector3 centerBuoy = sumPositions / (float)countInside;

    float volumeCube = (2.0f * halfSize.x) * (2.0f * halfSize.y) * (2.0f * halfSize.z);
    float totalSamples = (float)(BUOY_SAMPLES_PER_AXIS *
        BUOY_SAMPLES_PER_AXIS *
        BUOY_SAMPLES_PER_AXIS);
    float sampleVolume = volumeCube / totalSamples;

    float volImmerge = (float)countInside * sampleVolume;

    cyclone::Vector3 grav = body->getAcceleration();
    float gMag = grav.magnitude();
    cyclone::Vector3 gDir = grav;
    if (gMag > 0.0f) gDir *= (1.0f / gMag);
    cyclone::Vector3 forceDir = gDir * -1.0f;

    float magnitudeBuoy = density * volImmerge * gMag;

    cyclone::Vector3 buoyForce = forceDir * magnitudeBuoy;

    result.force = buoyForce;
    result.applicationPoint = centerBuoy;
    return result;
}

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