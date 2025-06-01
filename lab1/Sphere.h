// Sphere.h
#pragma once

#include "Mover.h"

class Sphere : public Mover {
public:
    Sphere() = default;
    virtual ~Sphere() = 0;

    virtual float getRadius() const = 0;

    virtual void setRadius(float r) = 0;

};

inline Sphere::~Sphere() {}
