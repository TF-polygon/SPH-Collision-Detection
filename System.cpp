#include "System.h"

Controller* HapticSystem;
Serial*		ser;
float		boxWidth = 2.f;
float		radius = 0;
mutex		mtx;

uint getHash(const glm::ivec3& cell) {
	return (
		(uint)(cell.x * 48424590)
		^ (uint)(cell.y * 62146215)
		^ (uint)(cell.z * 86134585)
		) % TABLE_SIZE;
}

System::System() {

}

System::System(uint numParticles, float mass, float restDensity, float gasConst, float viscosity, float h, float g, float tension, Controller& control) {
	this->numParticles = numParticles;
	this->restDensity = restDensity;
	this->viscosity = viscosity;
	this->h = h;
	this->g = g;
	this->tension = tension;
	this->HapticMode = false;

	POLY6 = 315.0f / (64.0f * PI * pow(h, 9));
	SPIKY_GRAD = -45.0f / (PI * pow(h, 6));
	SPIKY_LAP = 45.0f / (PI * pow(h, 6));
	MASS = mass;
	GAS_CONSTANT = gasConst;
	H2 = h * h;
	SELF_DENS = MASS * POLY6 * pow(h, 6);

	int cbNumParticles = numParticles * numParticles * numParticles;
	neighbouringParticles.resize(cbNumParticles);
	particles.resize(cbNumParticles);

	initParticles();

	sphere = new Mesh("resources/sphere.obj");
	sphereScale = glm::scale(glm::vec3(h / 15.f));
	sphereModelMtxs = new glm::mat4[cbNumParticles];

	glGenBuffers(1, &vbo);	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(glm::mat4), &sphereModelMtxs[0], GL_DYNAMIC_DRAW);

	glBindVertexArray(sphere->vao);
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

	started = false;

	particleTable = (Particle**)malloc(sizeof(Particle*) * TABLE_SIZE);

	blockBoundaries[0] = 0;
	size_t blockSize = particles.size() / THREAD_COUNT;
	for (int i = 1; i < THREAD_COUNT; i++) {
		blockBoundaries[i] = i * blockSize;
	}
	blockBoundaries[THREAD_COUNT] = particles.size();

	tableBlockBoundaries[0] = 0;
	blockSize = TABLE_SIZE / THREAD_COUNT;
	for (int i = 1; i < THREAD_COUNT; i++) {
		tableBlockBoundaries[i] = i * blockSize;
	}
	tableBlockBoundaries[THREAD_COUNT] = TABLE_SIZE;

	HapticSystem = &control;

	ser = new Serial("\\\\.\\COM7");

	if (ser->IsConnected()) {
		cout << "We're connected!" << endl;
		cout << "*** Arduino ";
		char receiveData[25];
		int len = ser->ReadData(receiveData, 25);
		receiveData[len] = 0;
		cout << receiveData << " *** " << endl;
	}
	else {
		cout << " :: failed to connect" << endl;
	}
}

System::~System() {
	free(particleTable);
	free(sphereModelMtxs);

	particles.clear();
	particles.shrink_to_fit();

	neighbouringParticles.clear();
	neighbouringParticles.shrink_to_fit();
}


void System::initParticles() {
	srand(1024);
	float particleSeperation = h + 0.01f;
	for (unsigned int i = 0; i < numParticles; i++) {
		for (unsigned int j = 0; j < numParticles; j++) {
			for (unsigned int k = 0; k < numParticles; k++) {
				float ranX = (float(rand()) / float((RAND_MAX)) * 0.5f - 1) * h / 10;
				float ranY = (float(rand()) / float((RAND_MAX)) * 0.5f - 1) * h / 10;
				float ranZ = (float(rand()) / float((RAND_MAX)) * 0.5f - 1) * h / 10;
				glm::vec3 nParticlePos = glm::vec3(i * particleSeperation + ranX - 1.5f, j * particleSeperation + ranY + h + 0.1f, k * particleSeperation + ranZ - 1.5f);

				Particle* nParticle = new Particle(MASS, h, nParticlePos, glm::vec3(0));

				particles[i + (j + numParticles * k) * numParticles] = nParticle;
			}
		}
	}
}

void parallelDensityAndPressures(const System& System, int start, int end) {
	float massPoly6Product = System.MASS * System.POLY6;

	for (int i = start; i < end; i++) {
		float pDensity = 0;
		Particle* pi = System.particles[i];
		glm::ivec3 cell = System.getCell(pi);

		for (int x = -1; x <= 1; x++) {
			for (int y = -1; y <= 1; y++) {
				for (int z = -1; z <= 1; z++) {
					glm::ivec3 near_cell = cell + glm::ivec3(x, y, z);
					uint index = getHash(near_cell);
					Particle* pj = System.particleTable[index];

					while (pj != nullptr) {
						float dist2 = glm::length2(pj->position - pi->position);
						if (dist2 < System.H2 && pi != pj) {
							pDensity += massPoly6Product * glm::pow(System.H2 - dist2, 3);
						}
						pj = pj->next;
					}
				}
			}
		}

		pi->density = pDensity + System.SELF_DENS;

		float pPressure = System.GAS_CONSTANT * (pi->density - System.restDensity);
		pi->pressure = pPressure;
	}
}

void parallelForces(const System& System, int start, int end) {
	for (int i = start; i < end; i++) {
		Particle* pi = System.particles[i];
		pi->force = glm::vec3(0);
		glm::ivec3 cell = System.getCell(pi);

		for (int x = -1; x <= 1; x++) {
			for (int y = -1; y <= 1; y++) {
				for (int z = -1; z <= 1; z++) {
					glm::ivec3 near_cell = cell + glm::ivec3(x, y, z);
					uint index = getHash(near_cell);
					Particle* pj = System.particleTable[index];
					
					while (pj != nullptr) {
						float dist2 = glm::length2(pj->position - pi->position);
						float HapticDist = glm::length2(pj->position - HapticSystem->HapticParticle->position);
						if (System.HapticMode) {
							if (HapticDist < HapticSystem->H2) {
								pj->is_detect = true;
								float dist = sqrt(HapticDist);
								glm::vec3 dir = glm::normalize(pj->position - HapticSystem->HapticParticle->position);

								glm::vec3 pressureForce = dir * ((pi->pressure + pj->pressure) / 10000) / (2 * pj->density) * System.SPIKY_GRAD;
								pressureForce *= pow(HapticSystem->h - dist, 2.0);
								pi->force += pressureForce;

								glm::vec3 velocityDif = pj->velocity - pi->velocity;
								glm::vec3 viscoForce = System.viscosity * System.MASS * (velocityDif / pj->density) * System.SPIKY_LAP * (HapticSystem->h - HapticDist);
								//pi->force += viscoForce;
							}
						}
						if (dist2 < System.H2 && pi != pj) {
							pj->is_detect = false;
							float dist = sqrt(dist2);
							glm::vec3 dir = glm::normalize(pj->position - pi->position);

							glm::vec3 pressureForce = -dir * System.MASS * (pi->pressure + pj->pressure) / (2 * pj->density) * System.SPIKY_GRAD;
							pressureForce *= pow(System.h - dist, 2);
							pi->force += pressureForce;

							glm::vec3 velocityDif = pj->velocity - pi->velocity;
							glm::vec3 viscoForce = System.viscosity * System.MASS * (velocityDif / pj->density) * System.SPIKY_LAP * (System.h - dist);
							pi->force += viscoForce;
						}
						pj = pj->next;
					}
				}
			}
		}
	}
}

void parallelHapticDensityAndPressure(const System& System, int start, int end) {
	parallelDensityAndPressures(System, start, end);
}

void parallelUpdateParticlePositions(const System& System, float deltaTime, int start, int end) {
	for (int i = start; i < end; i++) {
		Particle* p = System.particles[i];

		glm::vec3 acceleration = p->force / p->density + glm::vec3(0, System.g * 5, 0);
		p->velocity += acceleration * deltaTime;

		p->position += p->velocity * deltaTime;

		//float boxWidth = 2.f;
		float elasticity = 0.5f;
		if (p->position.y < p->size) {
			p->position.y = -p->position.y + 2 * p->size + 0.0001f;
			p->velocity.y = -p->velocity.y * elasticity;
		}
		// Top collision detection and wall constraints simulation condition
		/*if (p->position.y > -p->size + boxWidth) {
			p->position.y = -p->position.y + 2 * -(p->size - boxWidth) - 0.0001f;
			p->velocity.y = -p->velocity.y * elasticity;
		}*/

		if (p->position.x < p->size - boxWidth) {
			p->position.x = -p->position.x + 2 * (p->size - boxWidth) + 0.0001f;
			p->velocity.x = -p->velocity.x * elasticity;
		}

		if (p->position.x > -p->size + boxWidth) {
			p->position.x = -p->position.x + 2 * -(p->size - boxWidth) - 0.0001f;
			p->velocity.x = -p->velocity.x * elasticity;
		}


		if (p->position.z < p->size - boxWidth) {
			p->position.z = -p->position.z + 2 * (p->size - boxWidth) + 0.0001f;
			p->velocity.z = -p->velocity.z * elasticity;
		}

		if (p->position.z > -p->size + boxWidth) {
			p->position.z = -p->position.z + 2 * -(p->size - boxWidth) - 0.0001f;
			p->velocity.z = -p->velocity.z * elasticity;
		}
	}
}

void parallelSendingSignal(const System& System, int start, int end) {
	for (int i = start; i < end; i++) {
		Particle* pi = System.particles[i];
		if (pi->is_detect) {
			System.Send_SignalOfCD("Collision");
		}
		else {
			System.Send_SignalOfCD("NOT!");
		}
	}
}

void System::update(float deltaTime) {
	if (!started) return;
	
	deltaTime = 0.009f;

	buildTable();

	for (int i = 0; i < THREAD_COUNT; i++) {
		threads[i] = thread(parallelDensityAndPressures, ref(*this), blockBoundaries[i], blockBoundaries[i + 1]);
	}
	for (thread& thread : threads) {
		thread.join();
	}
	for (int i = 0; i < THREAD_COUNT; i++) {
		threads[i] = thread(parallelForces, ref(*this), blockBoundaries[i], blockBoundaries[i + 1]);
	}
	for (thread& thread : threads) {
		thread.join();
	}
	for (int i = 0; i < THREAD_COUNT; i++) {
		threads[i] = thread(parallelSendingSignal, ref(*this), blockBoundaries[i], blockBoundaries[i + 1]);
	}
	for (thread& thread : threads) {
		thread.join();
	}
	for (int i = 0; i < THREAD_COUNT; i++) {
		threads[i] = thread(parallelUpdateParticlePositions, ref(*this), deltaTime, blockBoundaries[i], blockBoundaries[i + 1]);
	}
	for (thread& thread : threads) {
		thread.join();
	}
}

void System::draw(const glm::mat4& viewProjMtx, uint shader) {
	for (int i = 0; i < particles.size(); i++) {
		glm::mat4 translate = glm::translate(particles[i]->position);
		if (i % 3 == 0) {
			sphereModelMtxs[i] = translate * (glm::scale(glm::vec3(h / 20.f)));
		}
		sphereModelMtxs[i] = translate * sphereScale;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(data, sphereModelMtxs, sizeof(glm::mat4) * particles.size());
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProjMtx"), 1, false, (float*)&viewProjMtx);
	glBindVertexArray(sphere->vao);
	glDrawElementsInstancedARB(GL_TRIANGLES, sphere->indices.size(), GL_UNSIGNED_INT, 0, particles.size());
	glBindVertexArray(0);
	glUseProgram(0);
}

void tableClearHelper(System& System, int start, int end) {
	for (int i = start; i < end; i++) {
		System.particleTable[i] = nullptr;
	}
}

void buildTableHelper(System& System, int start, int end) {
	for (int i = start; i < end; i++) {
		Particle* pi = System.particles[i];

		uint index = getHash(System.getCell(pi));

		mtx.lock();
		if (System.particleTable[index] == nullptr) {
			pi->next = nullptr;
			System.particleTable[index] = pi;
		}
		else {
			pi->next = System.particleTable[index];
			System.particleTable[index] = pi;
		}
		mtx.unlock();
	}
}

void System::buildTable(void) {
	for (int i = 0; i < THREAD_COUNT; i++) {
		threads[i] = thread(tableClearHelper, ref(*this), tableBlockBoundaries[i], tableBlockBoundaries[i + 1]);
	}
	for (thread& thread : threads) {
		thread.join();
	}

	for (int i = 0; i < THREAD_COUNT; i++) {
		threads[i] = thread(buildTableHelper, ref(*this), blockBoundaries[i], blockBoundaries[i + 1]);
	}
	for (thread& thread : threads) {
		thread.join();
	}
}

glm::ivec3 System::getCell(Particle* p) const {
	return glm::ivec3(p->position.x / h, p->position.y / h, p->position.z / h);
}

void System::reset(Controller& control) {
	control.FluidParticlesData.clear();
	delete control.HapticParticle;
	initParticles();
	started = false;
}

void System::startSimulation(void) {
	started = true;
}

void System::ConnectingNetwork(const char* ipAddr, int port) {
	const char* raspi_ipAddr = ipAddr;
	int raspi_port = port;
	netClient = new RaspberryPi_NetworkClient(raspi_ipAddr, raspi_port);
}

void System::Send_SignalOfCD(const char* cd_signal) const {
	if (!netClient->send_signal(cd_signal)) {
		//cout << "Failed to send signals of collision detection" << endl;
		return;
	}
}