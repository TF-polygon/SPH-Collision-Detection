#pragma once
#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "Core.h"
#include "Shader.h"
#include "Camera.h"
#include "System.h"
#include "Controller.h"

class Engine {
public:
	int		_WindowHandle;
	int		_WinX, _WinY;

	bool	_LeftDown;
	bool	_MiddleDown;
	bool	_RightDown;

	int		_MouseX, _MouseY;

	int		_prevTime, _currentTime;
	float	_deltaTime;


public:
	ShaderProgram*	InstanceProgram;
	ShaderProgram*	HapticInstanceProgram;

	Camera*			Cam;

	System*			system;
	Controller*		HapticSystem;

public:
	Engine(const char* windowTitle, int argc, char* argv[]);
	~Engine();

public:
	void	Update(void);
	void	Reset(void);
	void	Draw(void);

	void	Quit(void);

	void	Resize(int x, int y);
	void	Keyboard(int key, int x, int y);
	void	MouseButton(int btn, int state, int x, int y);
	void	MouseMotion(int x, int y);
	void	SpecialMouseMode(void);

public:
	void	VerticesLineDraw(void);
};

#endif