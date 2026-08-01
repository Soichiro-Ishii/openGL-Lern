#pragma once
#include "Stage.h"
class TmpStage final :
	public Stage
{
public:
	TmpStage();
	void onInit() override;
	void onUpdate(float delta) override;
	void onRender() override;
	void onShutdown() override;
private:
	void setStageName() override;
};

