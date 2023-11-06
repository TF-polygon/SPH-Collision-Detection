#include "Particle.h"

int Particle::pCount = 0;

Particle::Particle() { }

Particle::Particle(float size, glm::vec3 position) {
	this->size = size;
	this->position = position;
	
	id = pCount++;
}

Particle::Particle(float mass, float size, glm::vec3 position, glm::vec3 velocity) {
	this->mass = mass;
	this->size = size;
	this->position = position;
	this->velocity = velocity;

	is_detect = false;
	force = glm::vec3(0);
	acceleration = glm::vec3(0);

	density = 0.0f;
	pressure = 0.0f;
	id = pCount++;


	next = nullptr;
}
Particle::~Particle() { }
