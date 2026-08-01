#include "pch.h"
#include "Camera.h"
void Camera::setPos(glm::vec3 pos) {
	m_pos = pos;
}
void Camera::setAng(glm::vec3 ang) {
	m_ang = ang;
}
glm::mat4 Camera::getView() {
	glm::mat4 rotation =
		glm::yawPitchRoll(m_ang.y, m_ang.x, m_ang.z);

	glm::vec3 forward =
		glm::vec3(rotation * glm::vec4(0, 0, 1, 0));

	glm::vec3 up =
		glm::vec3(rotation * glm::vec4(0, 1, 0, 0));

	return glm::lookAt(
		m_pos,
		m_pos + forward,
		up
	);
}