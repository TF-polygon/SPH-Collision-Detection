#pragma once
#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Core.h"
class Particle {
public:
	static int pCount;

	float mass, size, elasticity;
	glm::vec3 position, velocity, acceleration;
	glm::vec3 force;

	Particle* next;

	float density;
	float pressure;
	float id;

	bool  is_detect;

public:
	Particle();
	Particle(float size, glm::vec3 position);
	Particle(float mass, float size, glm::vec3 position, glm::vec3 velocity);
	~Particle();
};

#endif