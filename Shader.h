#pragma once
#ifndef __SHADER_H__
#define __SHADER_H__

#include "Core.h"

class Shader {
public:
	enum ShaderType {
		eGeometry,
		eVertex,
		eFragment
		,eCompute
	};
	uint ShaderID;

public:
	Shader(const char* filename, ShaderType type);
	~Shader();

public:
	uint GetShaderID(void) { return ShaderID; }
};

class ShaderProgram {
public:
	enum ProgramType {
		eGeometry,
		eRender,
		eCompute
	};
	enum {
		eMaxShaders = 3
	};

	ProgramType		Type;
	Shader*			Shaders[eMaxShaders];
	uint			ProgramID;

public:
	ShaderProgram(const char* filename, ProgramType type);
	~ShaderProgram();

public:
	uint GetProgramID(void) const { return ProgramID; }
};

#endif