#include "pch.h"
#include "EarthStage.h"
#include"ProcMeshGenerator.h"

EarthStage::EarthStage()
{
	setStageName();
}
void EarthStage::onInit() {
	//シェーダー
	m_shader.load("assets\\shaders\\vs.glsl", "assets\\shaders\\earthFS.glsl");
	if (!m_shader.valid()) {
		spdlog::critical("faild to load shader");
	}

	m_blurShader.load("assets\\shaders\\screenVS.glsl", "assets\\shaders\\blurFS.glsl");
	if (!m_blurShader.valid()) {
		spdlog::critical("faild to load blurShader");
	}

	m_lastShader.load("assets\\shaders\\screenVS.glsl", "assets\\shaders\\renderTexFS.glsl");
	if (!m_lastShader.valid()) {
		spdlog::critical("faild to load lastShader");
	}
	GLuint numInstances = 1000;
	//メッシュ
	GLMeshData meshData;
	meshData = ProcMeshGenerator::createSphere(1.0f, 64, 32);
	m_mesh.create(meshData, numInstances);
	meshData = ProcMeshGenerator::createScreen();
	m_screen.create(meshData);

	//ubo
	m_ubo1.create(nullptr, sizeof(SceneConstants), 0);
	m_ubo2.create(nullptr, sizeof(GLuint), 1);
	//ssbo
	m_instanceData.resize(numInstances);
	m_ssbo.create(nullptr, m_instanceData.size() * sizeof(InstanceData), 0);

	//テクスチャ
	std::string dayTexPath = "assets\\data\\texture\\8k_earth_daymap.jpg";
	std::string nightTexPath = "assets\\data\\texture\\8k_earth_nightmap.jpg";
	std::string normalTexPath = "assets\\data\\texture\\8k_earth_normal_map.png";
	std::string specTexPath = "assets\\data\\texture\\8k_earth_specular_map.png";
	TEXTURE2DSETTING linerSet = { TEXTURE2DFILTER::LINEAR ,TEXTURE2DWRAP::REPEAT, COLOR_SPACE::RGB };
	TEXTURE2DSETTING sRGBSet = { TEXTURE2DFILTER::LINEAR ,TEXTURE2DWRAP::REPEAT, COLOR_SPACE::SRGB };
	if (!m_texture1.load(dayTexPath, sRGBSet)) {
		spdlog::critical("faild to load dayTexture");
	}
	if (!m_texture2.load(nightTexPath, sRGBSet)) {
		spdlog::critical("faild to load nightTexture");
	}
	if (!m_texture3.load(normalTexPath, linerSet)) {
		spdlog::critical("faild to load normalTexture");
	}
	if (!m_texture4.load(specTexPath, linerSet)) {
		spdlog::critical("faild to load specTexture");
	}
	//深度バッファ有効&比較関数指定
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);
	glEnable(GL_CULL_FACE); // カリングを有効化
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	//ubo用
	m_constants.view = glm::mat4(1.0f);
	m_constants.proj = glm::mat4(1.0f);
	m_constants.eye = glm::vec4(0.0f, 0.0f, -5.0f, 1.0f);
	m_constants.lightPos = glm::vec4(0.0f, 0.0f, -10.0f, 1.0f);
	m_constants.time = 0.0f;

	m_pos = glm::vec3(0, 0, 0);
	m_ang = glm::vec3(0, 0, 0);
	m_camera.setPos(m_pos);
	m_camera.setAng(m_ang);
	m_ubo1.update(&m_constants, sizeof(SceneConstants), 0);
	m_ubo2.update(&numInstances, sizeof(GLuint), 0);
	//compute
	constexpr GLuint localSizeX = 256;
	GLuint groutCountX = (static_cast<GLuint>(m_instanceData.size()) + localSizeX - 1) / localSizeX;
	m_compute.load("assets\\shaders\\createWorldCS.glsl", groutCountX);
	if (!m_compute.valid()) {
		spdlog::critical("faild to load compute shader");
	}
	m_normalRT.create(width(), height(), ColorTexSet::HDR);
	for (auto& rt : m_blurPP)
		rt.create(m_blurShader, width(), height(), ColorTexSet::NORMAL);
	int blurRep = 3;
	m_blurPPC.allocate(blurRep);
	for (int i = 0; i < blurRep; i++) {
		m_blurPPC.add(m_blurPP[i % 2]);
	}
}
void EarthStage::onUpdate(float delta) {
	m_normalRT.resize(width(), height());
	m_blurPPC.resize(width(), height());
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
	float aspect = 1.0f;
	if (width() <= 0 || height() <= 0) aspect = 1.0f;
	else aspect = widthf() / heightf();
	m_constants.time += delta;
	m_constants.eye = glm::vec4(m_pos, 1.0f);
	//m_constants.world = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin(glm::radians(m_constants.time * 360.0f)), 0.0f));
	//m_constants.world *= glm::rotate(glm::mat4(1.0f), glm::radians(m_constants.time * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//m_constants.world *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 3.0f));
	//m_constants.world *= glm::rotate(glm::mat4(1.0f), glm::radians(m_constants.time * 75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//m_constants.world *= glm::rotate(glm::mat4(1.0f), glm::radians(m_constants.time * 100.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	m_constants.lightPos = glm::vec4(cos(m_constants.time / 4), 0.0f, sin(m_constants.time / 4), 1.0f) * 10.0f;
	m_constants.view = m_camera.getView();
	m_constants.proj = glm::perspective(
		glm::radians(90.0f),
		aspect,
		0.1f,
		1000.0f
	);

	m_ubo1.update(&m_constants, sizeof(SceneConstants), 0);

	m_compute.dispatch();
	//m_ssbo.update(m_instanceData.data(), m_instanceData.size() * sizeof(InstanceData), 0);
}
void EarthStage::onRender() {
	m_normalRT.bind();
	glViewport(0, 0, m_normalRT.width(), m_normalRT.height());
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//画面クリア色設定&深度クリアの値設定
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//画面クリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//シェーダーをセット
	m_shader.bind();
	//テクスチャセット
	m_texture1.bind(0);
	m_texture2.bind(1);
	m_texture3.bind(2);
	m_texture4.bind(3);
	//メッシュ描画
	m_mesh.draw();
	m_normalRT.unbind();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, width(), height());

	m_lastShader.bind();
	m_normalRT.color().bind(0);
	m_screen.draw();
	//テストとしてブラーは切る
	//const GLTexture2D* res = &m_blurPPC.execute(m_normalRT.color(), m_screen);

	//glViewport(0, 0, width(), height());

	//m_lastShader.bind();
	//res->bind(0);
	//m_screen.draw();
}
void EarthStage::onShutdown() {}

void EarthStage::setStageName()
{
	m_name = "earth";
}