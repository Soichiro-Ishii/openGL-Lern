#pragma once
#include "Application.h"
#include "GLShader.h"
#include "GLTexture2D.h"
#include "GLUniformBuffer.h"
#include "GLMesh.h"

struct alignas(16) BlackHoleConstants
{
	glm::vec2 resolution;
	float time;
	float padding;
};

class TestRTRMApp :
	public Application
{
private:
	GLMesh m_screen;
	GLShader m_shader;
	GLTexture2D m_sky;
	GLUniformBuffer	m_BHCUB;
	BlackHoleConstants m_BHConsts;
protected:
	int onInit() override;
	void onUpdate(float delta) override;
	void onRender() override;
	void onShutdown() override;
};

