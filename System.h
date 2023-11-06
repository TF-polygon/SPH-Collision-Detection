#pragma once

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "Core.h"
#include "Shader.h"
#include "Camera.h"
#include "Particle.h"
#include "Mesh.h"
#include "Controller.h"
#include "SerialClass.h"
#include "NetworkClient.h"

class System {
public:
	Network*					netClient;

public:
	uint						numParticles;
	uint						vbo;

	Mesh*						sphere;

	Particle**					particleTable;

	glm::mat4					sphereScale;
	glm::mat4*					sphereModelMtxs;

	vector<vector<Particle*>>	neighbouringParticles;
	vector<Particle*>			particles;

	thread						threads[THREAD_COUNT];

	bool		started;
	bool		HapticMode;
	float		POLY6, SPIKY_GRAD, SPIKY_LAP, GAS_CONSTANT, MASS, H2, SELF_DENS;
	float		restDensity;
	float		viscosity, h, g, tension;

	size_t		blockBoundaries[THREAD_COUNT + 1];
	int			tableBlockBoundaries[THREAD_COUNT + 1];

public:
	System();
	System(uint numParticles, float mass, float restDensity, float gasConst, float viscosity, float h, float g, float tension, Controller& control);
	~System();

public:
	// Fluid Particle Calculating Functions
	void	initParticles(void);
	void	buildTable(void);
	void	update(float deltaTime);
	void	draw(const glm::mat4& viewProjMtx, uint shader);

	void	reset(Controller& control);
	void	startSimulation(void);

	// Raspberry PI Network Connection Function

	void	ConnectingNetwork(const char* ipAddr, int port);
	void    Send_SignalOfCD(const char* cd_signal) const;

	glm::ivec3	getCell(Particle* p) const;
};

#endif