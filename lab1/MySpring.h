#ifndef __MySpring
#define __MySpring

#include <pfgen.h>
#include "particle.h"

namespace cyclone {
	class Myspring : public cyclone::ParticleForceGenerator //Generic Force generating class
	{
		cyclone::Particle* other;
		double springConstant;
		double restLength;
	public:
		Myspring(cyclone::Particle* p, double springConstant, double restLenght);
		virtual void updateForce(cyclone::Particle* p, double duration);
	};
}


class MyAnchoredSpring : public cyclone::ParticleForceGenerator {
protected:
	cyclone::Vector3 anchor;
	double springConstant;
	double restLength;
public:
	MyAnchoredSpring();
	MyAnchoredSpring(cyclone::Vector3 anchor, double springConstant, double restLength);
	const cyclone::Vector3 getAnchor() const { return anchor; }
	void init(cyclone::Vector3 anchor, double springConstant, double restLength);
	virtual void updateForce(cyclone::Particle* particle, cyclone::real duration);
};

#endif // !__MySpring