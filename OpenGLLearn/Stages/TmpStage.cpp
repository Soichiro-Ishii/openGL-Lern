#include "pch.h"
#include "TmpStage.h"
TmpStage::TmpStage()
{
	setStageName();
}
bool TmpStage::onInit() {}
void TmpStage::onUpdate(float delta) {}
void TmpStage::onRender() {}
void TmpStage::onShutdown() {}
void TmpStage::setStageName()
{
	m_name = "tmp";
}