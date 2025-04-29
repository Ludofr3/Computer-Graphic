#include "MySpring.h"
#include <particle.h>

using namespace cyclone;

Myspring::Myspring(cyclone::Particle* p, double springConstant, double restLenght)
{
	other = p;
	this->springConstant = springConstant;
	this->restLength = restLenght;
}

void cyclone::Myspring::updateForce(cyclone::Particle* p, double duration)
{
	Vector3 force;
	Vector3 d = p->getPosition() - other->getPosition();
	int magnitude = d.magnitude();
	d.normalise();

	force = -springConstant * (magnitude - restLength) * d;
	p->addForce(force);
}

MyAnchoredSpring::MyAnchoredSpring() {
	this->anchor = cyclone::Vector3(3, 30, 0);
	this->springConstant = 5.0f;
	this->restLength = 3.0f;
}

MyAnchoredSpring::MyAnchoredSpring(cyclone::Vector3 anchor, double springConstant,
	double restLength)
{
	this->anchor = anchor;
	this->springConstant = springConstant;
	this->restLength = restLength;
}

void MyAnchoredSpring::updateForce(Particle* particle, real duration)
{
	Vector3 force;
	//calculate the force here (using particles current position, anchor position, spring constant
	Vector3 d = particle->getPosition() - this->getAnchor();
	int magnitude = d.magnitude();
	d.normalise();

	force = -this->springConstant * (magnitude - this->restLength) * d;

	particle->addForce(force);
}

void MyAnchoredSpring::init(cyclone::Vector3 anchor, double springConstant, double restLength) {
	this->anchor = anchor;
	this->springConstant = springConstant;
	this->restLength = restLength;
}
