#pragma once
#include "Stage.h"
#include "GLShader.h"
#include "GLTexture2D.h"
#include "GLUniformBuffer.h"
#include"GLShaderStorageBuffer.h"
#include "GLMesh.h"
#include"GLCompute.h"
#include"Camera.h"
#include"GLRenderTarget.h"
#include"PostProcessChain.h"

struct alignas(16) SceneConstants {
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec4 eye;
	glm::vec4 lightPos;
	float time;
	glm::vec2 res;
	int32_t pad;
};
struct alignas(16) InstanceData {
	glm::mat4 world;
};

struct alignas(16) InstanceCount
{
	uint32_t value;
	uint32_t padding[3];
};

class EarthStage final :
	public Stage
{
private:
	GLShader m_shader;
	GLShader m_blurShader;
	GLShader m_lastShader;
	GLMesh m_mesh;
	GLUniformBuffer m_ubo1;
	GLUniformBuffer m_ubo2;
	SceneConstants m_constants{};
	GLShaderStorageBuffer m_ssbo;
	std::vector<InstanceData> m_instanceData;
	GLTexture2D m_texture1;
	GLTexture2D m_texture2;
	GLTexture2D m_texture3;
	GLTexture2D m_texture4;
	Camera m_camera;
	glm::vec3 m_pos;
	glm::vec3 m_ang;
	GLCompute m_compute;
	GLRenderTarget m_normalRT;
	PostProcessPass m_blurPP[2];
	PostProcessChain m_blurPPC;
	GLMesh m_screen;
	InstanceCount m_instanceCount;
public:
	EarthStage();
	bool onInit() override;
	void onUpdate(float delta) override;
	void onRender() override;
	void onShutdown() override;
private:
	void setStageName() override;
};

