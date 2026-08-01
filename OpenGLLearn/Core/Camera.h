#pragma once
#include<glm/glm.hpp>
class Camera
{
private:
	glm::vec3 m_pos;
	glm::vec3 m_ang;
public:
	void setPos(glm::vec3 pos);
	void setAng(glm::vec3 ang);
	glm::mat4 getView();
};

