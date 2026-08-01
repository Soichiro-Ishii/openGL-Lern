#pragma once
#include "Stage.h"
#include "GLShader.h"
#include "GLTexture2D.h"
#include "GLUniformBuffer.h"
#include"GLRenderTarget.h"
#include "GLMesh.h"
#include"PostProcessPass.h"
#include"PostProcessChain.h"

struct alignas(16) BlackHoleConstants
{
	glm::vec2 resolution;
	float time;
	float padding;
};

class BlackHoleStage :
	public Stage
{
private:
	GLMesh m_screen;
	GLShader m_RTRMshader;
	GLShader m_blurShader;
	GLShader m_lastShader;
	GLTexture2D m_sky;
	GLUniformBuffer	m_BHCUB;
	BlackHoleConstants m_BHConsts;
	GLRenderTarget m_BHrt;
	PostProcessPass m_inversePP;
	PostProcessPass m_blurPP[2];
	PostProcessChain m_blurPPC;
	GLShader m_inverseShader;
public:
	BlackHoleStage();
	bool onInit() override;
	void onUpdate(float delta) override;
	void onRender() override;
	void onShutdown() override;
private:
	void setStageName() override;
};

