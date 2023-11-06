#include "Controller.h"

mutex				HapticMtx;
float				fluid_particle_radius;

uint ControllerGetHash(const glm::ivec3& cell) {
	return (
		(uint)(cell.x * 48424590)
		^ (uint)(cell.y * 62146215)
		^ (uint)(cell.z * 86134585)
		) % TABLE_SIZE;
}

Controller::Controller(float h) {
	this->h = h;
	H2 = h * h;

	HapticInitParticle();

	HapticSphere = new Mesh("resources/sphere.obj");
	HapticSphereScale = glm::scale(glm::vec3(h / 15.f));
	HapticSphereModelMtxs = new glm::mat4;
	glm::vec3 scale = glm::vec3(HapticSphereScale[0][0], HapticSphereScale[1][1], HapticSphereScale[2][2]);
	HapticRadius = glm::length(scale) / 2.0f;

	FluidParticlesData.resize(0);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), &HapticSphereModelMtxs[0], GL_DYNAMIC_DRAW);

	glBindVertexArray(HapticSphere->vao);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), 0);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)sizeof(glm::vec4));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(sizeof(glm::vec4) * 2));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(sizeof(glm::vec4) * 3));

	glVertexAttribDivisorARB(2, 1);
	glVertexAttribDivisorARB(3, 1);
	glVertexAttribDivisorARB(4, 1);
	glVertexAttribDivisorARB(5, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Build hash table for block boundaries
	HapticParticleTable = (Particle**)malloc(sizeof(Particle*) * TABLE_SIZE);

	HapticBlockBoundaries[0] = 0;
	int blockSize = THREAD_COUNT;
	for (int i = 1; i < THREAD_COUNT; i++) {
		HapticBlockBoundaries[i] = i * blockSize;
	}
	HapticBlockBoundaries[THREAD_COUNT] = 1;

	HapticTableBlockBoundaries[0] = 0;
	blockSize = TABLE_SIZE / THREAD_COUNT;
	for (int i = 1; i < THREAD_COUNT; i++) {
		HapticTableBlockBoundaries[i] = i * blockSize;
	}
	HapticTableBlockBoundaries[THREAD_COUNT] = TABLE_SIZE;
}

Controller::~Controller() {
	FluidParticlesData.clear();
	delete HapticParticle;
}

void Controller::HapticInitParticle(void) {
	glm::vec3 nParticlePos = glm::vec3(0.f, 0.5f, 0.f);
	Particle* nParticle = new Particle(h, nParticlePos);
	HapticParticle = nParticle;
}

void Controller::ControllerSphereDraw(const glm::mat4& viewProjMtx, uint shader) {
	glm::mat4 translate = glm::translate(HapticParticle->position);
	*HapticSphereModelMtxs = translate * HapticSphereScale;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(data, HapticSphereModelMtxs, sizeof(glm::mat4));
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProjMtx"), 1, false, (float*)&viewProjMtx);
	glBindVertexArray(HapticSphere->vao);
	glDrawElementsInstancedARB(GL_TRIANGLES, HapticSphere->indices.size(), GL_UNSIGNED_INT, 0, 1);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Controller::HapticParticlePosition(float _pos_X, float _pos_Y, float _pos_Z) {
	const float rate = 1.f / 100.f;	
	HapticParticle->position.x += _pos_X * rate;
	HapticParticle->position.z -= _pos_Y * rate;	
}

void Controller::HapticParticleHeightMoving(float _pos_Y) {
	const float rate = 1.f / 100.f;
	HapticParticle->position.y += _pos_Y * rate;
}

void Controller::HapticUpdate(float deltaTime) {
	deltaTime = 0.009f;

	HapticBuildTable();

}

void HapticTableClearHelper(Controller& Controller, int start, int end) {
	for (int i = 0; i < THREAD_COUNT; i++) {
		Controller.HapticParticleTable[i] = nullptr;
	}
}

void HapticBuildTableHelper(Controller& Controller, int start, int end) {
	Particle* pi = Controller.HapticParticle;

	uint index = ControllerGetHash(Controller.HapticGetCell(pi));

	HapticMtx.lock();
	if (Controller.HapticParticleTable[index] == nullptr) {
		pi->next = nullptr;
		Controller.HapticParticleTable[index] = pi;
	}
	else {
		pi->next = Controller.HapticParticleTable[index];
		Controller.HapticParticleTable[index] = pi;
	}
	HapticMtx.unlock();
}


void Controller::HapticBuildTable(void) {
	for (int i = 0; i < THREAD_COUNT; i++) {
		HapticThreads[i] = thread(HapticBuildTableHelper, ref(*this), HapticBlockBoundaries[i], HapticBlockBoundaries[i + 1]);
	}
	for (thread& thread : HapticThreads) {
		thread.join();
	}
	for (int i = 0; i < THREAD_COUNT; i++) {
		HapticThreads[i] = thread(HapticBuildTableHelper, ref(*this), HapticTableBlockBoundaries[i], HapticTableBlockBoundaries[i + 1]);
	}
	for (thread& thread : HapticThreads) {
		thread.join();
	}
}

glm::ivec3 Controller::HapticGetCell(Particle* p) const {
	return glm::ivec3(p->position.x / h, p->position.y / h, p->position.z / h);
}


void Controller::ReceiveDatas(vector<Particle*> ReceiveParticles, float radius) {
	int data_size = ReceiveParticles.size();
	for (int i = 0; i < data_size; i++) {
		FluidParticlesData.emplace_back(ReceiveParticles[i]);
	}
	fluid_particle_radius = radius;
}

void Controller::ReceiveDatas(glm::mat4 scale) {
	FluidParticleScale = scale;
}

void Controller::ReceiveDatas(Particle** datas) {
	*FluidParticleTable = *datas;	
}


void Controller::HapticParticleReset(void) {
	HapticInitParticle();
}