#include "Camera.h"

Camera::Camera() {
	Reset();
}

void Camera::Update(void) {
	glm::mat4 world(1);
	world[3][2] = Distance;
	world = glm::eulerAngleY(glm::radians(-Azimuth)) * glm::eulerAngleX(glm::radians(-Incline)) * world;

	glm::mat4 view = glm::inverse(world);

	glm::mat4 project = glm::perspective(glm::radians(FOV), Aspect, NearClip, FarClip);

	ViewProjectMtx = project * view;
}

void Camera::Reset(void) {
	FOV = 45.0f;
	Aspect = 1.33f;
	NearClip = 0.1f;
	FarClip = 100.0f;

	Distance = 10.0f;
	Azimuth = 0.0f;
	Incline = 20.0f;
}