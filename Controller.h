#pragma once
#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "Core.h"
#include "Mesh.h"
#include "Particle.h"
#include "Shader.h"
//#include "System.h"

class Controller {
public:
	Mesh*						HapticSphere;
	glm::mat4					HapticSphereScale;
	glm::mat4*					HapticSphereModelMtxs;
	float						HapticRadius;

	Particle*					HapticParticle;
	Particle**					HapticParticleTable;

	vector<Particle*>			FluidParticlesData;
	glm::mat4					FluidParticleScale;
	Particle**					FluidParticleTable;


	size_t						HapticBlockBoundaries[THREAD_COUNT + 1];
	size_t						HapticTableBlockBoundaries[THREAD_COUNT + 1];

	thread						HapticThreads[THREAD_COUNT];

public:
	uint			vbo;
	float			h;
	float			H2;

public:
	Controller(float h);
	Controller();
	~Controller();

public:
	void		HapticInitParticle(void);
	void		ControllerSphereDraw(const glm::mat4& viewProjMtx, uint shader);
	void		HapticParticlePosition(float _pos_X, float _pos_Y, float _pos_Z);
	void		HapticParticleHeightMoving(float _pos_Y);

public:
	void		HapticUpdate(float deltaTime);
	void		HapticBuildTable(void);
	void		HapticParticleReset(void);
	glm::ivec3  HapticGetCell(Particle* p) const;

public:
	void		ReceiveDatas(vector<Particle*> ReceiveParticles, float radius);
	void		ReceiveDatas(glm::mat4 scale);
	void		ReceiveDatas(Particle** datas);

};

#endif