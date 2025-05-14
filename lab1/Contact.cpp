#include "Contact.h"
#include <iostream>

using namespace cyclone;

MyGroundContact::MyGroundContact(){}

MyGroundContact::~MyGroundContact(){}

void MyGroundContact::init(cyclone::Particle* p,
	double size)
{
	if (p)
		particles.push_back(p);
	this->size = size;
}

unsigned MyGroundContact::addContact(cyclone::ParticleContact* contact, unsigned limit) const {
    if (contact == nullptr) {
        std::cerr << "Erreur : contact est nullptr" << std::endl;
        return 0;
    }
    unsigned count = 0;
    for (const auto& p : particles) {
        cyclone::real y = p->getPosition().y;
        cyclone::real groundLevel = 0.0;
        if (y < groundLevel + size) {
            if (count >= limit) {
                return count;
            }

            //Contact configuration
            contact->contactNormal = cyclone::Vector3(0, 1, 0);
            contact->particle[0] = p;
            contact->particle[1] = nullptr;
            contact->penetration = (groundLevel + size) - y;
            contact->restitution = 1.0;
            count++;
            contact++;
        }
    }
    return count;
}


MyPlaneContact::MyPlaneContact(std::shared_ptr<Plane> p) : plane(p) {}

MyPlaneContact::~MyPlaneContact() {}

void MyPlaneContact::init(cyclone::Particle* p, double size) {
    if (p)
        particles.push_back(p);
    this->size = size;
}

unsigned MyPlaneContact::addContact(cyclone::ParticleContact* contact, unsigned limit) const {
    if (contact == nullptr) {
        std::cerr << "Erreur : contact est nullptr" << std::endl;
        return 0;
    }
    unsigned count = 0;
    for (const auto& p : particles) {
        cyclone::Vector3 pos = p->getPosition();
        if (plane->is_shortest_distance(pos, size)) {
            if (count >= limit) {
                return count;
            }

            //Contact configuration
            contact->contactNormal = plane->get_normal();
            contact->particle[0] = p;
            contact->particle[1] = nullptr;
            contact->penetration = size - plane->shortest_distance(pos);
            contact->restitution = 1.0;
            count++;
            contact++;
        }
    }
    return count;
}

ParticleCollision::ParticleCollision(cyclone::Particle* p1, cyclone::Particle* p2, double size) {
    particle[0] = p1;
    particle[1] = p2;
    this->size = size;
}

unsigned ParticleCollision::addContact(cyclone::ParticleContact* contact, unsigned limit) const
{
    contact->particle[0] = particle[0];
    contact->particle[1] = particle[1];
    cyclone::Vector3 pos0 = particle[0]->getPosition();
    cyclone::Vector3 pos1 = particle[1]->getPosition();
    cyclone::Vector3 delta = pos0 - pos1;
    double distance = delta.magnitude();
    double y = size * 2;

    if (distance < y) {
        contact->particle[0] = particle[0];
        contact->particle[1] = particle[1];
        cyclone::Vector3 normal = delta;
        normal.normalise();
        contact->contactNormal = normal;
        contact->penetration = y - distance;
        contact->restitution = 1.0;

        return 1;
    }
    
    return 0;
}