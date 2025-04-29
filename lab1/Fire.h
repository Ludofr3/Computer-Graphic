#pragma once
#include <particle.h>
#include <deque>
#include "FireworksRule.h"

class Fire {
public:
	Fire(int type); //Fire type (0=Init Fire, 1=Child fire)
	~Fire();
	float size; // Fire size (0.5)
	int type; //Fire type (set in the constructor)
	float age; //Fire’s current age
	cyclone::Particle * m_particle;
	FireworksRule * m_rule;//Which rule this particle is influenced by?
	cyclone::Vector3 color; //Fire color
	std::deque <cyclone::Vector3> history; //history saving
	bool update(float duration); //return bool
	void draw(int shadow);
	void drawHistory();
	void setRule(FireworksRule * r); //set a particular rule
	void putHistory(); //Save current position in the m_historys
};