#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

using namespace std;

class Camera
{
	glm::vec3 m_pos;		// pozycja kamery
	glm::vec3 m_front;		// wektor patrzenia
	glm::vec3 m_up;			// to jest to coœ dziwne, zawsze 1 na y	

	double m_angle_x;		// obrót kamery w prawo i w lewo
	double m_angle_y;		// obrót kamery w górê i w dó³

	double m_x_cos;			
	double m_x_sin;
	double m_y_cos;
	double m_y_sin;

public:
	Camera(const glm::vec3 &pos, double ang_x, double ang_y); 
	void setPos(const glm::vec3 &p);
	void setAngleX(double ang);
	void setAngleY(double ang);
	void setAngles(double ang_x, double ang_y);
	glm::vec3 getPos() const;
	glm::vec3 getFront() const;
	glm::mat4 getViewMatrix() const;
	void update(GLFWwindow *window, double time);
};

#endif