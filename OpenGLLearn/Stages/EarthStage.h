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
#include"GLMultiRenderTarget.h"
#include"PostProcessChain.h"
#include<Bloom.h>

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

enum class EARTH_RENDER_TYPE {
	FINALCOLORTEXTURE,
	NORMALTEXTURE,
	MAINTEXTURE,
	NIGHTTEXTURE,
	NORMALMAPTEXTURE,
	SOECMAPTEXTURE,
	DEPTHTEXTURE,
	NUM_RENDER_TYPE
};

class EarthStage final :
	public Stage
{
private:
	GLShader m_shader;
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
	GLMultiRenderTarget m_normalRT;
	GLMesh m_screen;
	InstanceCount m_instanceCount;
	float m_speed = 0.0f;
	float m_angSpeed = 0.0f;
	bool m_enableBoom = true;
	bool m_showHello = false;
	EARTH_RENDER_TYPE m_renderType = EARTH_RENDER_TYPE::FINALCOLORTEXTURE;
	Bloom m_bloom;
public:
	EarthStage();
	bool onInit() override;
	void onUpdate(float delta) override;
	void onRender() override;
	void onShutdown() override;
private:
	void setStageName() override;
};

