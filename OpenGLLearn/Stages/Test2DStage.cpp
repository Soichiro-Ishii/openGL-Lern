#include "pch.h"
#include "Test2DStage.h"
#include"Proc2DMeshGenerator.h"

Test2DStage::Test2DStage()
{
	setStageName();
}
bool Test2DStage::onInit() {
	GLuint instanceCount = 10000;
	GL2DMeshData data = Proc2DMeshGenerator::createCircle();
	m_mesh.create(data, instanceCount);
	m_shader.load("assets\\shaders\\vs2D.glsl", "assets\\shaders\\fs2D.glsl");
	if (!m_shader.valid()) {
		spdlog::critical("Faild to load shader.");
		return false;
	}
	m_ID.resize(instanceCount);
	m_SCUB.create(nullptr, sizeof(SceneConstants2D), 0);
	m_IDSSB.create(nullptr, sizeof(InstanceData2D) * instanceCount, 0);
	m_camera.changeDim(CAMERA_DIMENSION::DIM_2D);
	m_SC.time = 0.0f;
	m_SC.lightPos = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_SC.res = glm::vec2(widthf(), heightf());
	return true;
}
void Test2DStage::onUpdate(float delta) {
	m_SC.time += delta;

	if (ImGui::Begin("Test2DStage")) {
		ImGui::SliderFloat("scale", &m_scale, 0.1f, 100.0f);
		ImGui::SliderFloat("ang", &m_angle, 0.0f, glm::pi<float>() * 2);
		ImGui::SliderFloat("speed", &m_speed, 0.0f, 50.0f);
	}
	ImGui::End();

	glm::vec2 vel = glm::vec2(0.0f, 0.0f);
	if (isPress(GLFW_KEY_RIGHT)) {
		vel.x += 1;
	}
	if (isPress(GLFW_KEY_LEFT)) {
		vel.x += -1;
	}
	if (isPress(GLFW_KEY_UP)) {
		vel.y += 1;
	}
	if (isPress(GLFW_KEY_DOWN)) {
		vel.y += -1;
	}
	vel = glm::rotate(glm::mat4(1.0f), m_angle, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4(vel, 0.0f, 0.0f);
	m_pos += vel * delta * m_speed;
	m_SC.eye = glm::vec4(m_pos, 0.0f, 0.0f);
	m_camera.setPos2D(m_pos);
	m_camera.setAng2D(m_angle);
	m_camera.setScale2D(m_scale);
	m_SC.view = m_camera.getView2D();
	const float aspect = height() > 0 ? widthf() / heightf() : 1.0f;
	m_SC.proj = glm::ortho(
		-aspect, aspect,
		-1.0f, 1.0f,
		-1.0f, 1.0f
	);
	int size = 100;
	float space = 1.5f;
	for (int i = 0; i < m_ID.size(); i++) {
		int ix = i % size;
		int iy = i / size;

		float x = static_cast<float>(ix);
		float y = static_cast<float>(iy);

		glm::vec2 uv(
			x / float(size - 1),
			y / float(size - 1)
		);

		m_ID[i].world = glm::translate(
			glm::mat4(1.0f),
			glm::vec3(x, y, 0.0f) * space
		);

		m_ID[i].col = glm::vec4(
			uv.x,
			uv.y,
			1.0f - uv.x,
			1.0f
		);
	}
	m_SC.lightPos = m_SC.eye;


	m_SCUB.update(&m_SC, sizeof(SceneConstants2D), 0);
	m_IDSSB.update(m_ID.data(), sizeof(InstanceData2D) * m_ID.size(), 0);
}
void Test2DStage::onRender() {
	glViewport(0, 0, width(), height());
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//画面クリア色設定&深度クリアの値設定
	glClearColor(0.2f, 0.8f, 1.0f, 0.0f);
	glClearDepth(1.0f);
	//画面クリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_shader.bind();
	m_mesh.draw();
}
void Test2DStage::onShutdown() {}
void Test2DStage::setStageName()
{
	m_name = "test2DStage";
}