#pragma once
#ifndef __MESH_H__
#define __MESH_H__

#include "Core.h"

class Mesh {
public:
	vector<glm::vec3>	points;
	vector<glm::vec3>	normals;
	vector<glm::uvec3>	triangles;
	vector<glm::uvec3>	normalIndices;

	vector<uint>		indices;
	uint				vao, vbo, vbo_n, ebo;
	// vao  (vertex  array  objects)
	// vbo  (vertex  buffer objects)
	// vbo_n(vertex	 buffer	objects n .. )
	// ebo  (element buffer objects)


public:
	Mesh(string filename);
	~Mesh();

public:
	void				draw(const glm::mat4& modelMtx, const glm::mat4& viewProjMtx, uint shader);
	void				init(string filename);

};

#endif