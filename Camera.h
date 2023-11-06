#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Core.h"

class Camera {
public:
	Camera();

public:
	void				Update(void);
	void				Reset(void);

	void				SetAspect(float a)		{ Aspect = a; }
	void				SetDistance(float d)	{ Distance = d; }
	void				SetAzimuth(float a)		{ Azimuth = a; }
	void				SetIncline(float i)		{ Incline = i; }

	float				GetDistance(void)		{ return Distance; }
	float				GetAzimuth(void)		{ return Azimuth; }
	float				GetIncline(void)		{ return Incline; }

	const glm::mat4&	GetViewProjectMtx(void) { return ViewProjectMtx; }
public:
	float				FOV;
	float				Aspect;
	float				NearClip;
	float				FarClip;

	float				Distance;
	float				Azimuth;
	float				Incline;

	glm::mat4			ViewProjectMtx;
};

#endif