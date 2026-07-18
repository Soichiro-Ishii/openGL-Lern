#include<glad/gl.h>
#include<GLFW/glfw3.h>
#include<spdlog/spdlog.h>

int main() {
	//glfw初期化
	if (!glfwInit()) {
		spdlog::critical("faild initialization glfw");
		return -1;
	}
	spdlog::info("glfw initialized");
	//バージョン指定
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	glfwWindowHint(GLFW_OPENGL_PROFILE,
		GLFW_OPENGL_CORE_PROFILE);
	//ウィンドウ作成
	GLFWwindow* window = glfwCreateWindow(1280, 720, "OpenGL", nullptr, nullptr);
	if (!window) {
		spdlog::critical("faild creating window");
		glfwTerminate();
		spdlog::info("terminated glfw");
		return -1;
	}
	spdlog::info("window created");
	//コンテキスト作成
	glfwMakeContextCurrent(window);
	//gladのglロード
	if (!gladLoadGL(glfwGetProcAddress)) {
		spdlog::critical("faild loading gl");
		glfwDestroyWindow(window);
		spdlog::info("destroied window");
		glfwTerminate();
		spdlog::info("terminated glfw");
		return -1;
	}
	spdlog::info("gl loded");
	while (!glfwWindowShouldClose(window)) {
		//viewPortの設定
		glViewport(0, 0, 1280, 720);
		//画面クリア色設定
		glClearColor(0.4f, 0.7f, 0.7f, 1.0f);
		//画面クリア
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//画面スワップ
		glfwSwapBuffers(window);
		//イベント処理
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	spdlog::info("destroied window");
	glfwTerminate();
	spdlog::info("terminated glfw");
}