#include "Shader.h"

Shader::Shader(const char* filename, ShaderType type) {
	string fileData = "";
	fstream stream(filename, ios::in);
	if (!stream.is_open()) {
		printf("ERROR: Can't open shader file '%s'\n", filename);
		ShaderID = 0;
		return;
	}

	string line = "";
	while (getline(stream, line)) {
		fileData += "\n" + line;
	}

	stream.close();

	string shaderSource;
	switch (type) {
	case eGeometry:
		shaderSource = "#define GEOMETRY_SHADER" + fileData;
		ShaderID = glCreateShader(GL_GEOMETRY_SHADER_ARB);
		break;
	case eVertex:
		shaderSource = "#define VERTEX_SHADER" + fileData;
		ShaderID = glCreateShader(GL_VERTEX_SHADER_ARB);
		break;
	case eFragment:
		shaderSource = "#define FRAGMENT_SHADER" + fileData;
		ShaderID = glCreateShader(GL_FRAGMENT_SHADER_ARB);
		break;
	// case eCompute: break;
	default:
		break;
	}
	shaderSource = "#version 430\n" + shaderSource;
	const char* rawShaderSource = shaderSource.c_str();
	glShaderSource(ShaderID, 1, &rawShaderSource, NULL);
	glCompileShader(ShaderID);

	int isCompiled = 0;
	glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == false) {
		printf("ERROR: Can't compile shader '%s'\n", filename);

		int maxLength = 0;
		glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &maxLength);
		vector<char> errorLog(maxLength);
		glGetShaderInfoLog(ShaderID, maxLength, &maxLength, &errorLog[0]);
		printf("GL ERRORS:\n%s\n", &errorLog[0]);

		glDeleteShader(ShaderID);
		ShaderID = 0;
	}
}

Shader::~Shader() {
	glDeleteShader(ShaderID);
}

ShaderProgram::ShaderProgram(const char* filename, ProgramType type) {
	Type = type;
	for (int i = 0; i < eMaxShaders; i++) {
		Shaders[i] = 0;
	}
	if (Type == eGeometry) {
		Shaders[0] = new Shader(filename, Shader::eVertex);
		Shaders[1] = new Shader(filename, Shader::eGeometry);
		Shaders[2] = new Shader(filename, Shader::eFragment);
	}
	else if (Type == eRender) {
		Shaders[0] = new Shader(filename, Shader::eVertex);
		Shaders[1] = new Shader(filename, Shader::eFragment);
	}
	else if (Type == eCompute) {
		Shaders[0] = new Shader(filename, Shader::eCompute);
	}

	ProgramID = glCreateProgram();
	for (int i = 0; i < eMaxShaders; i++) {
		if (Shaders[i] == 0)
			break;
		glAttachShader(ProgramID, Shaders[i]->GetShaderID());
	}
	glLinkProgram(ProgramID);
}

ShaderProgram::~ShaderProgram() {
	for (int i = 0; i < eMaxShaders; i++) {
		delete Shaders[i];
	}
	glDeleteProgram(ProgramID);
}