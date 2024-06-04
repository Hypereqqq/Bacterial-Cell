#include"Camera.h"

Camera::Camera(const glm::vec3 &pos, double ang_x, double ang_y)	//konstruktor, pozycja, kat nachylenia
{
	m_pos = pos;

	setAngles(ang_x, ang_y);

	m_up = glm::vec3(0, 1, 0);
}

void Camera::setPos(const glm::vec3 &p)
{
	m_pos = p;
}

void Camera::setAngleX(double ang)
{
	m_angle_x = ang;
}

void Camera::setAngleY(double ang)
{
	m_angle_y = ang;
}

void Camera::setAngles(double ang_x, double ang_y)
{
	m_angle_x = ang_x;
	m_angle_y = ang_y;

	m_x_cos = cos(glm::radians(m_angle_x));
	m_x_sin = sin(glm::radians(m_angle_x));
	m_y_cos = cos(glm::radians(m_angle_y));
	m_y_sin = sin(glm::radians(m_angle_y));

	m_front = glm::vec3(	// wektor patrzenia
		m_x_cos * m_y_cos,	
		m_y_sin,
		m_x_sin * m_y_cos
	);

	m_front = glm::normalize(m_front);	
}

glm::vec3 Camera::getPos() const
{
	return m_pos;
}

glm::vec3 Camera::getFront() const
{
	return m_front;
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(m_pos, m_pos + m_front, m_up);
}

void Camera::update(GLFWwindow *window, double time)
{
	double speed = 4.;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_pos.x += m_x_sin * time * speed;
		m_pos.z -= m_x_cos * time * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_pos.x -= m_x_sin * time * speed;
		m_pos.z += m_x_cos * time * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		m_pos.x += m_x_cos * time * speed;
		m_pos.z += m_x_sin * time * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_pos.x -= m_x_cos * time * speed;
		m_pos.z -= m_x_sin * time * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		m_pos.y += time * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		m_pos.y -= time * speed;
	}

	int up = glfwGetKey(window, GLFW_KEY_UP),		// sprawdza czy nacisniete
		down = glfwGetKey(window, GLFW_KEY_DOWN),
		left = glfwGetKey(window, GLFW_KEY_LEFT),
		right = glfwGetKey(window, GLFW_KEY_RIGHT);

	m_angle_x += 90. * time * (right - left);		// w lewo prawo 
	m_angle_y += 90. * time * (up - down);			// gora dol

	const float maxCameraAngle = 89.875;			// nie moze byc 90 bo potem cos sie moze wyzerowaæ, bo tangens 90 = nieskonczonosc

	if (m_angle_x >= 360.)							// ograniczenie kata
		m_angle_x -= 360.;
	else if (m_angle_x < 0.)
		m_angle_x += 360.;

	if (m_angle_y > maxCameraAngle)					// ograniczenie kata
		m_angle_y = maxCameraAngle;
	else if (m_angle_y < -maxCameraAngle)
		m_angle_y = -maxCameraAngle;

	setAngles(m_angle_x, m_angle_y);				// ustawienie nowych katow
}