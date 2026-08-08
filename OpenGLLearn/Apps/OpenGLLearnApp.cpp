#include "pch.h"
#include "OpenGLLearnApp.h"
#include"ProcMeshGenerator.h"
#include"EarthStage.h"
#include"BlackHoleStage.h"
#include"Test2DStage.h"

bool OpenGLLearnApp::onInit() {
	m_stageManager.setStageManager(this);
	return m_stageManager.change(std::make_unique<EarthStage>());
}
void OpenGLLearnApp::onUpdate(float delta) {
	bool stageChangeResult = true;
	if (isTrigger(GLFW_KEY_F1))
		stageChangeResult = m_stageManager.change(std::make_unique<EarthStage>());
	if (isTrigger(GLFW_KEY_F2))
		stageChangeResult = m_stageManager.change(std::make_unique<BlackHoleStage>());
	if (isTrigger(GLFW_KEY_F3))
		stageChangeResult = m_stageManager.change(std::make_unique<Test2DStage>());
	if (!stageChangeResult)
		quit();

	m_stageManager.onUpdate(delta);
}
void OpenGLLearnApp::onRender() {
	m_stageManager.onRender();
}
void OpenGLLearnApp::onShutdown() {
	spdlog::info("Application shutdown");
}