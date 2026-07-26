#include "pch.h"
#include "TestRTRMApp.h"
#include"ProcMeshGenerator.h"

int TestRTRMApp::onInit() {
	GLMeshData meshData;
	meshData = ProcMeshGenerator::createScreen();
	m_screen.create(meshData);

	m_shader.load("assets\\shaders\\screenVS.glsl", "assets\\shaders\\blackHoleRayMarch1FS.glsl");
	if (!m_shader.valid()) return -1;

	std::string texPath1 = "assets\\data\\texture\\starmap_2020_8k2.png";
	TEXTURE2DSETTING set = { TEXTURE2DFILTER::LINEAR ,TEXTURE2DWRAP::REPEAT };
	if (!m_sky.load(texPath1, set)) return -1;

	m_BHConsts.resolution.x = widthf();
	m_BHConsts.resolution.y = heightf();
	m_BHConsts.time = 0.0f;
	m_BHCUB.create(nullptr, sizeof(BlackHoleConstants), 0);
	return 0;
}
void TestRTRMApp::onUpdate(float delta) {
	m_BHConsts.time += delta;
	m_BHCUB.update(&m_BHConsts, sizeof(BlackHoleConstants), 0);
}
void TestRTRMApp::onRender() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_shader.bind();
	m_sky.bind(0);
	m_screen.draw();
}
void TestRTRMApp::onShutdown() {

}