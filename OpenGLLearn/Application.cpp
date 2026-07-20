#include "pch.h"
#include "Application.h"
int Application::run(int width, int height, std::string windowName, bool fullScreen, int VSync, float targetFPS) {
	//垂直同期ありのときはそっちを優先
	if (VSync) m_FPSLimiter.setTargetFPS(static_cast<double>(0.0f));
	else m_FPSLimiter.setTargetFPS(static_cast<double>(targetFPS));
	if (m_context.create(width, height, windowName.c_str(), fullScreen, VSync) != 0) {
		spdlog::critical("faild creating context");
		return -1;
	}
	if (onInit() != 0) {
		spdlog::critical("faild application initialization");
		onShutdown();
		m_context.release();
		return -1;
	}
	m_chrono.timeStart();
	while (!glfwWindowShouldClose(m_context.window())) {
		m_chrono.timeEnd();
		float delta = static_cast<float>(m_chrono.getElapsed());
		m_chrono.timeStart();
		m_FPSLimiter.begin();

		//イベント処理
		glfwPollEvents();

		m_context.update();

		onUpdate(delta);
		onRender();

		//画面スワップ
		glfwSwapBuffers(m_context.window());
		m_FPSLimiter.wait();
		spdlog::info("delta:{}\nfps:{}", delta, 1 / delta);
	}
	onShutdown();
	m_context.release();
	return 0;
}