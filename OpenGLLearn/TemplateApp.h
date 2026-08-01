#pragma once
#include "Application.h"
class TemplateApp final :
	public Application
{
protected:
	int onInit() override;
	void onUpdate(float delta) override;
	void onRender() override;
	void onShutdown() override;
};

