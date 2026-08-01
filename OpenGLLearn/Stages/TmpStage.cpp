#include "pch.h"
#include "TmpStage.h"
TmpStage::TmpStage()
{
	setStageName();
}
void TmpStage::onInit() {}
void TmpStage::onUpdate(float delta) {}
void TmpStage::onRender() {}
void TmpStage::onShutdown() {}
void TmpStage::setStageName()
{
	m_name = "tmp";
}