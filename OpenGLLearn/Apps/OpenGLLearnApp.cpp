#include "pch.h"
#include "OpenGLLearnApp.h"
#include"ProcMeshGenerator.h"
#include"EarthStage.h"
#include"BlackHoleStage.h"

int OpenGLLearnApp::onInit() {
	m_stageManager.setStageManager(this);
	m_stageManager.change(std::make_unique<EarthStage>());
	return 0;
}
void OpenGLLearnApp::onUpdate(float delta) {
	m_stageManager.onUpdate(delta);
	if (input().isTrigger(GLFW_KEY_F1))
		m_stageManager.change(std::make_unique<EarthStage>());
	if (input().isTrigger(GLFW_KEY_F2))
		m_stageManager.change(std::make_unique<BlackHoleStage>());
}
void OpenGLLearnApp::onRender() {
	m_stageManager.onRender();
}
void OpenGLLearnApp::onShutdown() {
	spdlog::info("Application shutdown");
}