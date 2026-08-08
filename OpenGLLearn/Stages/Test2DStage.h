#pragma once
#include "Stage.h"
#include"GL2DMesh.h"
#include"GLShader.h"
#include"GlUniformBuffer.h"
#include"GLShaderStorageBuffer.h"
#include"GLCompute.h"
#include"Camera.h"
#include<vector>
struct alignas(16) SceneConstants2D {
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec4 eye;
	glm::vec4 lightPos;
	float time;
	glm::vec2 res;
	int32_t pad;
};
struct alignas(16) InstanceData2D {
	glm::mat4 world;
	glm::vec4 col;
};

class Test2DStage final :
	public Stage
{
private:
	GL2DMesh m_mesh;
	GLShader m_shader;
	GLUniformBuffer m_SCUB;
	GLShaderStorageBuffer m_IDSSB;
	SceneConstants2D m_SC;
	std::vector<InstanceData2D> m_ID;
	Camera m_camera;
	glm::vec2 m_pos = glm::vec2(0.0f, 0.0f);
	float m_angle = 0.0f;
	float m_scale = 1.0f;
	float m_speed = 1.0f;
public:
	Test2DStage();
	bool onInit() override;
	void onUpdate(float delta) override;
	void onRender() override;
	void onShutdown() override;
private:
	void setStageName() override;
};

