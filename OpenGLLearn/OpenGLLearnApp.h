#pragma once
#include "Application.h"
#include "GLShader.h"
#include "GLTexture2D.h"
#include "GLUniformBuffer.h"
#include "GLMesh.h"

struct alignas(16) SceneConstants {
	glm::mat4 world;
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec4 eye;
	float time;
	glm::vec2 res;
	int32_t pad;
};

class OpenGLLearnApp final :
	public Application
{
private:
	GLShader m_shader;
	GLMesh m_mesh;
	GLUniformBuffer m_ubo;
	GLTexture2D m_texture;
	SceneConstants m_constants{};
protected:
	int onInit() override;
	void onUpdate(float delta) override;
	void onRender() override;
	void onShutdown() override;
};

