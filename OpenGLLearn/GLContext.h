#pragma once
#include<GLFW/glfw3.h>
#include<glad/gl.h>
#include<string>

class GLContext
{
private:
	GLFWwindow* m_window = nullptr;
	float m_width = 0;
	float m_height = 0;
	bool m_fullScreen = false;
	int m_VSync = 1;
public:
	GLContext() = default;
	~GLContext();
	GLContext(GLContext&) = delete;
	GLContext& operator=(GLContext&) = delete;
	GLContext(GLContext&& other) noexcept;
	GLContext& operator=(GLContext&& other) noexcept;
	int create(int width, int height, std::string windowName, bool fullScreen = false, int VSync = 1);
	void release();
	[[nodiscard]] GLFWwindow* window() {
		return m_window;
	}
	[[nodiscard]] float width() {
		return m_width;
	}
	[[nodiscard]] float height() {
		return m_height;
	}
};

static void GLAPIENTRY OpenGLDebugCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam);

