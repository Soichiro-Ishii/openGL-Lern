#include "pch.h"
#include "StageManager.h"

void StageManager::setStageManager(const Application* app)
{
	m_app = app;
}

void StageManager::change(std::unique_ptr<Stage> stage)
{
	if (m_app == nullptr)
		return;
	if (m_active && m_stage->name() == stage->name())
		return;
	if (m_active) {
		onShutdown();
	}
	m_stage = std::move(stage);
	onInit();
	spdlog::info("active stage : {}", m_stage->name());
	m_active = true;
}

void StageManager::onInit()
{
	if (m_app == nullptr)
		return;
	m_stage->setApp(m_app);
	m_stage->onInit();
}

void StageManager::onUpdate(float delta)
{
	if (!m_active)
		return;
	m_stage->onUpdate(delta);
}

void StageManager::onRender()
{
	if (!m_active)
		return;
	m_stage->onRender();
}

void StageManager::onShutdown()
{
	if (!m_active)
		return;
	std::string stageName = m_stage->name();
	m_stage->onShutdown();
	spdlog::info("shutdown stage : {}", stageName);
	m_active = false;
}
