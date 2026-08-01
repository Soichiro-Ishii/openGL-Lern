#pragma once
#include<memory>
#include"Stage.h"
class StageManager
{
private:
	std::unique_ptr<Stage> m_stage;
	bool m_active = false;
	const Application* m_app;
public:
	void setStageManager(const Application* app);
	void change(std::unique_ptr<Stage> stage);
	void onInit();
	void onUpdate(float delta);
	void onRender();
	void onShutdown();
};

