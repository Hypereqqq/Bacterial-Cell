#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

using namespace std;

struct ObjLoader
{
	string path;
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices; // f - wszystkie zapisane w jednej tablicy
	vector<glm::vec3> out_vertices;		// v
	vector<glm::vec2> out_uvs;			// vt
	vector<glm::vec3> out_normals;		// vn

	vector<float> finalTab; // calosc - atrybuty vertexa zapisane w odpowiednim porzadku (v, vt, vn)

	unsigned int triangles_cnt; 
	unsigned int m_VBO, m_VAO; // VBO - Vertex Buffer Object, VAO - Vertex Array Object

	ObjLoader();
	ObjLoader(string str);
	void load(string str);
	void fillTab();
	void draw();
};

#endif