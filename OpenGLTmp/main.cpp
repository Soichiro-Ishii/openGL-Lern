#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

int main()
{
	if (!glfwInit())
	{
		std::cout << "GLFW initialization failed.\n";
		return -1;
	}

	// OpenGL 4.6 Core Profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "OpenGL Test", nullptr, nullptr);

	if (!window)
	{
		std::cout << "Window creation failed.\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
	{
		std::cout << "GLAD initialization failed.\n";
		glfwTerminate();
		return -1;
	}

	std::cout << "===== OpenGL Information =====\n";
	std::cout << "Vendor   : " << glGetString(GL_VENDOR) << '\n';
	std::cout << "Renderer : " << glGetString(GL_RENDERER) << '\n';
	std::cout << "Version  : " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL     : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

	while (!glfwWindowShouldClose(window))
	{
		glViewport(0, 0, 1280, 720);

		glClearColor(0.15f, 0.25f, 0.45f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}