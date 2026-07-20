#include "pch.h"
#include "OpenGLLearnApp.h"
#include"ProcMeshGenerator.h"
int OpenGLLearnApp::onInit() {
	//シェーダー
	m_shader.load("assets\\shaders\\vs.glsl", "assets\\shaders\\fs.glsl");
	if (!m_shader.valid()) return -1;

	//メッシュ
	GLMeshData meshData = ProcMeshGenerator::createSphere();
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
	return 0;
}
void OpenGLLearnApp::onUpdate(float delta) {
	//viewPortの設定
	glViewport(0, 0, width(), height());
	//画面クリア色設定&深度クリアの値設定
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//画面クリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//一応毎回更新
	float aspect = widthf() / heightf();
	m_constants.time += delta;
	m_constants.eye = glm::vec4(cos(glm::radians(m_constants.time * 45.0f)), 0.0f, sin(glm::radians(m_constants.time * 45.0f)), 0.0f) * 5.0f;
	m_constants.world = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin(glm::radians(m_constants.time * 360.0f)), 0.0f));
	m_constants.world *= glm::rotate(glm::mat4(1.0f), glm::radians(m_constants.time * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_constants.world *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 3.0f));
	m_constants.world *= glm::rotate(glm::mat4(1.0f), glm::radians(m_constants.time * 75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_constants.world *= glm::rotate(glm::mat4(1.0f), glm::radians(m_constants.time * 100.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	m_constants.view = glm::lookAt(
		glm::vec3(m_constants.eye.x, m_constants.eye.y, m_constants.eye.z),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);
	m_constants.proj = glm::perspective(
		glm::radians(90.0f),
		aspect,
		0.1f,
		1000.0f
	);

	m_ubo.update(&m_constants, sizeof(SceneConstants), 0);
}
void OpenGLLearnApp::onRender() {
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