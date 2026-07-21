#include "pch.h"
#include "Application.h"
int Application::run(int _width, int _height, std::string windowName, bool fullScreen, int VSync, float targetFPS) {
	//垂直同期ありのときはそっちを優先
	if (VSync) m_FPSLimiter.setTargetFPS(static_cast<double>(0.0f));
	else m_FPSLimiter.setTargetFPS(static_cast<double>(targetFPS));
	if (m_context.create(_width, _height, windowName.c_str(), fullScreen, VSync) != 0) {
		spdlog::critical("faild creating context");
		return -1;
	}
	if (onInit() != 0) {
		spdlog::critical("faild application initialization");
		onShutdown();
		m_context.release();
		return -1;
	}
	//delta平均取る用配列
	std::vector<float> deltas;
	size_t deltaIdx = 0;
	size_t maxNumDelta = 0;
	if (VSync) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		maxNumDelta = mode->refreshRate;
	}
	else {
		maxNumDelta = static_cast<size_t>(targetFPS);
		if (maxNumDelta <= 0) maxNumDelta = 60;
	}
	deltas.resize(maxNumDelta);
	m_chrono.timeStart();
	while (!glfwWindowShouldClose(m_context.window())) {
		m_chrono.timeEnd();
		float delta = static_cast<float>(m_chrono.getElapsed());
		deltas[deltaIdx] = delta;
		deltaIdx++;
		m_chrono.timeStart();
		m_FPSLimiter.begin();

		//イベント処理
		glfwPollEvents();

		m_context.update();

		onUpdate(delta);
		//最小化モードのときは描画しない
		if (width() <= 0 || height() <= 0) {
			glfwWaitEvents();
			continue;
		}
		onRender();

		//画面スワップ
		glfwSwapBuffers(m_context.window());
		m_FPSLimiter.wait();
		if (deltaIdx >= maxNumDelta) {
			float sumDelta = 0.0f;
			for (auto d : deltas) {
				sumDelta += d;
			}
			float avgDelta = sumDelta / maxNumDelta;
			spdlog::info("delta:{}\nfps:{}", avgDelta, 1 / avgDelta);
			deltaIdx = 0;
		}
	}
	onShutdown();
	m_context.release();
	return 0;
}