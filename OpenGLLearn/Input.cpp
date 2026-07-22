#include "pch.h"
#include "Input.h"
void Input::init(GLFWwindow* window) {
	m_window = window;
	for (int i = 0; i <= 1; i++) {
		for (int j = 0; j <= GLFW_KEY_END; j++) {
			m_keys[i][j] = false;
		}
	}
}
void Input::update() {
	m_now = (m_now + 1) % 2;
	if (!m_window) {
		spdlog::error("Input:GLFWWindow is not set.");
		return;
	}
	for (int i = 0; i <= GLFW_KEY_END; i++) {
		m_keys[m_now][i] = glfwGetKey(m_window, i);
	}
}
bool Input::isPress(int key) const {
	if (!m_window) {
		spdlog::error("Input:GLFWWindow is not set.");
		return false;
	}
	if (key > GLFW_KEY_END) {
		spdlog::error("Input:That key does not exist.\n key:{}", key);
		return false;
	}
	return m_keys[m_now][key];
}
bool Input::isTrigger(int key) const {
	if (!m_window) {
		spdlog::error("Input:GLFWWindow is not set.");
		return false;
	}
	if (key > GLFW_KEY_END) {
		spdlog::error("Input:That key does not exist.\n key:{}", key);
		return false;
	}
	char before = (m_now + 1) % 2;
	//前のキーは押されていなくて、今は押されている
	return m_keys[m_now][key] && !m_keys[before][key];
}