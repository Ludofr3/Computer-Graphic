#pragma once
#include <vector>
#include "Fire.h"
#include "FireworksRule.h"

class Fireworks {
public:
	Fireworks();
	~Fireworks();
	void update(float duration); //Fire create/update
	void create(); //create init Fires
	void create(Fire* parent); //create Child Fires
	void draw(int shadow); //draw
	FireworksRule m_rule[3]; //rules
	std::vector<Fire*> fireworks; //contains Fires

};