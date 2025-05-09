#pragma once

#ifndef __Contact__

#include <vector>
#include <pcontacts.h>
#include <Plane.h>

namespace cyclone {
	class MyGroundContact : public cyclone::ParticleContactGenerator {
	public:
		MyGroundContact();
		~MyGroundContact();
		void init(cyclone::Particle* p, double size); //add particle p in the container
		//Detect collision. If there are collisions, write info in the contact (limit=max # of contacts)
		//If there are collision, return the number of collisions
		virtual unsigned addContact(cyclone::ParticleContact* contact, unsigned limit) const;

	private:
		std::vector<cyclone::Particle*> particles; //Container for all particles
		double size; //particle size(all particles have a same size)
	};
}


class MyPlaneContact :public cyclone::ParticleContactGenerator
{
	std::vector<cyclone::Particle*> particles;
	double size; //size of the particle
	std::shared_ptr<Plane> plane;
public:
	MyPlaneContact(std::shared_ptr<Plane>);
	~MyPlaneContact();
	void init(cyclone::Particle* p, double size);
	virtual unsigned addContact(cyclone::ParticleContact* contact, unsigned limit) const;
};

class ParticleLink : public cyclone::ParticleContactGenerator
{
public:
	cyclone::Particle* particle[2];
protected:
	cyclone::real currentLength() const;
public:
	virtual unsigned addContact(cyclone::ParticleContact* contact,
		unsigned limit) const = 0;
};

class ParticleCollision : public ParticleLink
{
public:
	double size; //particle size
public:
	ParticleCollision(cyclone::Particle* p1, cyclone::Particle* p2, double size);
	virtual unsigned addContact(cyclone::ParticleContact* contact, unsigned limit) const;
};

#endif // !__Contact__
