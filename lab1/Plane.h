#pragma once

#include <DrawUtils.H>
#include "Mover.h"

class Plane {
private:
	const cyclone::Vector3 p1;
	const cyclone::Vector3 p2;
	const cyclone::Vector3 p3;
	const cyclone::Vector3 p4;
	const std::vector<GLfloat> color;
	const cyclone::Vector3 n;

public:
	Plane() : p1(cyclone::Vector3(20., 0., -25.)), p2(cyclone::Vector3(20., 0., 25.)), p3(cyclone::Vector3(-20., 30., 25.)), p4(cyclone::Vector3(-20., 30., -25.)), color({ 1, 1, 0, 0.5 }), n(normal()) {}

	cyclone::Vector3 normal() {
		cyclone::Vector3 v1 = p2 - p1;
		cyclone::Vector3 v2 = p3 - p1;
		v1.normalise();
		v2.normalise();
		cyclone::Vector3 nrml = v1.cross(v2) * -1;
		nrml.normalise();
		return nrml;
	}

	~Plane() {}
	void draw() {
		glColor4f(color.at(0), color.at(1), color.at(2), color.at(3)); //color
		polygonf(4, p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, p3.x, p3.y, p3.z, p4.x, p4.y, p4.z);

	}

	float shortest_distance(cyclone::Vector3 pos) {
		cyclone::Vector3 v = pos - p1;
		return v.dot(n);
	}

	bool objet_in_plane(cyclone::Vector3 pos) {
		return p1.x >= pos.x && pos.x >= p3.x && p1.z <= pos.z && pos.z <= p3.z;;
	}

	bool is_shortest_distance(cyclone::Vector3 pos, float size) {
		return (shortest_distance(pos) <= size && objet_in_plane(pos));
	}

	cyclone::Vector3 new_object_position(cyclone::Vector3 pos, float size) {
		return pos + size * n;
	}

	cyclone::Vector3 new_object_velocity(cyclone::Vector3 vel) {
		cyclone::Vector3 v1 = vel.dot(n) * n;
		cyclone::Vector3 v2 = vel - v1;
		return -1 * v1 + v2;
	}

	cyclone::Vector3 get_normal() { return n; }
};