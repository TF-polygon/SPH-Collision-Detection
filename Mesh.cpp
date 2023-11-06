#include "Mesh.h"

Mesh::Mesh(string filename) {
	init(filename);
}

void Mesh::init(string filename) {
	vector<glm::vec3> inputNormals;
	vector<glm::vec3> inputPoints;
	ifstream objStream(filename);
	string line;
	if (objStream.is_open()) {
		while (getline(objStream, line)) {
			stringstream ss;
			ss << line;

			string label;
			ss >> label;
			if (label == "v") {
				glm::vec3 point;
				ss >> point.x;
				ss >> point.y;
				ss >> point.z;

				inputPoints.emplace_back(point);
			}
			if (label == "vn") {
				glm::vec3 normal;
				ss >> normal.x;
				ss >> normal.y;
				ss >> normal.z;

				inputNormals.emplace_back(normal);
			}
			if (label == "f") {
				glm::uvec3 triangle;
				glm::uvec3 normalIndex;
				string s;
				size_t pos;
				string v;

				ss >> s;
				pos = s.find("/");
				v = s.substr(0, pos);
				triangle.x = std::stoi(v) - 1;
				s.erase(0, pos + 1);
				pos = s.find("/");
				s.erase(0, pos + 1);
				normalIndex.x = std::stoi(s) - 1;

				ss >> s;
				pos = s.find("/");
				v = s.substr(0, pos);
				triangle.y = std::stoi(v) - 1;
				s.erase(0, pos + 1);
				pos = s.find("/");
				s.erase(0, pos + 1);
				normalIndex.y = std::stoi(s) - 1;

				ss >> s;
				pos = s.find("/");
				v = s.substr(0, pos);
				triangle.z = std::stoi(v) - 1;
				s.erase(0, pos + 1);
				pos = s.find("/");
				s.erase(0, pos + 1);
				normalIndex.z = std::stoi(s) - 1;

				triangles.emplace_back(triangle);
				normalIndices.emplace_back(normalIndex);
			}
		}
		objStream.close();
	}
	else {
		cout << "File cannot be read or does not exist: " << filename;
	}
	for (unsigned int i = 0; i < triangles.size(); i++) {
		points.emplace_back(inputPoints[triangles[i].x]);
		points.emplace_back(inputPoints[triangles[i].y]);
		points.emplace_back(inputPoints[triangles[i].z]);
		normals.emplace_back(inputNormals[normalIndices[i].x]);
		normals.emplace_back(inputNormals[normalIndices[i].y]);
		normals.emplace_back(inputNormals[normalIndices[i].z]);
		indices.emplace_back(i * 3);
		indices.emplace_back(i * 3 + 1);
		indices.emplace_back(i * 3 + 2);
	}
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vbo_n);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
		points.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_n);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void Mesh::draw(const glm::mat4& modelMtx, const glm::mat4& viewProjMtx, uint shader) {
	glUseProgram(shader);

	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMtx"), 1, false, (float*)&modelMtx);
	glm::mat4 mvpMtx = viewProjMtx;
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProjMtx"), 1, false, (float*)&mvpMtx);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	
	glUseProgram(0);
}