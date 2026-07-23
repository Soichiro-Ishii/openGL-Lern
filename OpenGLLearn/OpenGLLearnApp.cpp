#include "pch.h"
#include "OpenGLLearnApp.h"
#include"ProcMeshGenerator.h"

// HSB (h: 0-360, s: 0-1, b: 0-1) を RGB (0-1) に変換する関数
glm::vec4 hsbToRgb(const glm::vec3& hsb, float alpha = 1.0f) {
	float h = hsb.x, s = hsb.y, b = hsb.z;
	float c = b * s;
	float h_prime = h / 60.0f;
	float x = c * (1.0f - std::abs(std::fmod(h_prime, 2.0f) - 1.0f));
	float m = b - c;

	glm::vec3 rgb;
	if (h_prime < 1) rgb = { c, x, 0 };
	else if (h_prime < 2) rgb = { x, c, 0 };
	else if (h_prime < 3) rgb = { 0, c, x };
	else if (h_prime < 4) rgb = { 0, x, c };
	else if (h_prime < 5) rgb = { x, 0, c };
	else                  rgb = { c, 0, x };

	return glm::vec4(rgb.r + m, rgb.g + m, rgb.b + m, alpha);
}

int OpenGLLearnApp::onInit() {
	//シェーダー
	m_shader.load("assets\\shaders\\vs.glsl", "assets\\shaders\\fs.glsl");
	if (!m_shader.valid()) return -1;

	//メッシュ
	GLMeshData meshData;
	meshData = ProcMeshGenerator::createSphere(1.0f, 64, 32);
	m_mesh.create(meshData);

	//ubo
	m_ubo.create(nullptr, sizeof(SceneConstants), 0);

	//テクスチャ
	std::string texPath = "assets\\data\\texture\\8k_earth_daymap.jpg";
	TEXTURE2DSETTING set = { TEXTURE2DFILTER::LINEAR ,TEXTURE2DWRAP::REPEAT };
	if (!m_texture.load(texPath, set)) return -1;
	//深度バッファ有効&比較関数指定
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);
	glEnable(GL_CULL_FACE); // カリングを有効化
	glCullFace(GL_BACK);

	//ubo用
	m_constants.world = glm::mat4(1.0f);
	m_constants.view = glm::mat4(1.0f);
	m_constants.proj = glm::mat4(1.0f);
	m_constants.eye = glm::vec4(0.0f, 0.0f, -5.0f, 1.0f);
	m_constants.time = 0.0f;

	m_pos = glm::vec3(0, 0, 0);
	m_ang = glm::vec3(0, 0, 0);
	m_camera.setPos(m_pos);
	m_camera.setAng(m_ang);
	return 0;
}
void OpenGLLearnApp::onUpdate(float delta) {
	//viewPortの設定
	glViewport(0, 0, width(), height());
	//位置
	float speed = 5.0f;
	glm::vec3 velXZ = glm::vec3(0, 0, 0);
	glm::vec3 vel = glm::vec3(0, 0, 0);
	if (input().isPress(GLFW_KEY_W))
		velXZ += glm::vec3(0, 0, 1);
	if (input().isPress(GLFW_KEY_S))
		velXZ -= glm::vec3(0, 0, 1);
	if (input().isPress(GLFW_KEY_A))
		velXZ += glm::vec3(1, 0, 0);
	if (input().isPress(GLFW_KEY_D))
		velXZ -= glm::vec3(1, 0, 0);
	if (input().isPress(GLFW_KEY_R))
		vel += glm::vec3(0, 1, 0);
	if (input().isPress(GLFW_KEY_F))
		vel -= glm::vec3(0, 1, 0);
	velXZ = glm::yawPitchRoll(m_ang.y, 0.0f, 0.0f) * glm::vec4(velXZ, 0.0f);
	vel += velXZ;
	if (glm::dot(vel, vel) > 0.0f) {
		vel = glm::normalize(vel) * speed;
		m_pos += vel * delta;
	}
	m_camera.setPos(m_pos);
	//角度
	glm::vec3 angVel = glm::vec3(0, 0, 0);
	float angSpeed = glm::radians(180.0f);
	if (input().isPress(GLFW_KEY_LEFT))
		angVel += glm::vec3(0, 1, 0);
	if (input().isPress(GLFW_KEY_RIGHT))
		angVel -= glm::vec3(0, 1, 0);
	if (input().isPress(GLFW_KEY_DOWN))
		angVel += glm::vec3(1, 0, 0);
	if (input().isPress(GLFW_KEY_UP))
		angVel -= glm::vec3(1, 0, 0);
	m_ang += angVel * angSpeed * delta;
	m_camera.setAng(m_ang);
	//一応毎回更新
	float aspect = widthf() / heightf();
	if (width() <= 0 || height() <= 0) aspect = 1.0f;
	m_constants.time += delta;
	m_constants.eye = glm::vec4(m_pos, 1.0f);
	m_constants.world = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin(glm::radians(m_constants.time * 360.0f)), 0.0f));
	m_constants.world *= glm::rotate(glm::mat4(1.0f), glm::radians(m_constants.time * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_constants.world *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 3.0f));
	m_constants.world *= glm::rotate(glm::mat4(1.0f), glm::radians(m_constants.time * 75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_constants.world *= glm::rotate(glm::mat4(1.0f), glm::radians(m_constants.time * 100.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	m_constants.view = m_camera.getView();
	m_constants.proj = glm::perspective(
		glm::radians(90.0f),
		aspect,
		0.1f,
		1000.0f
	);

	m_ubo.update(&m_constants, sizeof(SceneConstants), 0);
}
void OpenGLLearnApp::onRender() {
	//画面クリア色設定&深度クリアの値設定
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//画面クリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//シェーダーをセット
	m_shader.bind();
	//テクスチャセット
	m_texture.bind(0);
	//メッシュ描画
	m_mesh.draw();
}
void OpenGLLearnApp::onShutdown() {
	m_texture.release();
	m_ubo.release();
	m_mesh.release();
	m_shader.release();
}