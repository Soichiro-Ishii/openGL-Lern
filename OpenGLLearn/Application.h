#pragma once
#include"GLContext.h"
#include"FPSLimiter.h"
class Application
{
private:
	GLContext m_context;
	CHRONO m_chrono;
	FPSLimiter m_FPSLimiter;
public:
	Application() = default;
	~Application() = default;

	Application(Application&) = delete;
	Application& operator=(Application&) = delete;

	int run(int width, int height, std::string windowName, bool fullScreen = false, int VSync = 1, float targetFPS = 0.0f);
protected:
	virtual int onInit() = 0;
	virtual void onUpdate(float delta) = 0;
	virtual void onRender() = 0;
	virtual void onShutdown() = 0;
	[[nodiscard]] GLFWwindow* window() const {
		return m_context.window();
	}
	[[nodiscard]] int width() const {
		return m_context.width();
	}
	[[nodiscard]] int height() const {
		return m_context.height();
	}
	[[nodiscard]] float widthf() const {
		return m_context.widthf();
	}
	[[nodiscard]] float heightf() const {
		return m_context.heightf();
	}
	void setTargetFPS(float newFPS) {
		m_FPSLimiter.setTargetFPS(static_cast<double>(newFPS));
	}
};

