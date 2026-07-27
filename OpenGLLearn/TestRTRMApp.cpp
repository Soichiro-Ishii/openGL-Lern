#include "pch.h"
#include "TestRTRMApp.h"
#include"ProcMeshGenerator.h"

int TestRTRMApp::onInit() {
	GLMeshData meshData;
	meshData = ProcMeshGenerator::createScreen();
	m_screen.create(meshData);

	m_RTRMshader.load("assets\\shaders\\screenVS.glsl", "assets\\shaders\\blackHoleRayMarch1FS.glsl");
	if (!m_RTRMshader.valid()) return -1;

	m_postShader.load("assets\\shaders\\screenVS.glsl", "assets\\shaders\\inverseFS.glsl");
	if (!m_postShader.valid()) return -1;

	std::string texPath1 = "assets\\data\\texture\\starmap_2020_8k2.png";
	TEXTURE2DSETTING set = { TEXTURE2DFILTER::LINEAR ,TEXTURE2DWRAP::REPEAT };
	if (!m_sky.load(texPath1, set)) return -1;

	m_BHConsts.resolution.x = widthf();
	m_BHConsts.resolution.y = heightf();
	m_BHConsts.time = 0.0f;
	m_BHCUB.create(nullptr, sizeof(BlackHoleConstants), 0);

	ColorTexSet cSet = ColorTexSet::NORMAL;
	m_rt.create(width(), height(), cSet);
	return 0;
}
void TestRTRMApp::onUpdate(float delta) {
	//viewPortの設定
	glViewport(0, 0, width(), height());
	if (width() != 0 && height() != 0)
		m_rt.resize(width(), height());
	m_BHConsts.time += delta;
	m_BHConsts.resolution.x = widthf();
	m_BHConsts.resolution.y = heightf();
	m_BHCUB.update(&m_BHConsts, sizeof(BlackHoleConstants), 0);
}
void TestRTRMApp::onRender() {
	m_rt.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	m_RTRMshader.bind();
	m_sky.bind(0);
	m_screen.draw();

	m_rt.unbind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	m_postShader.bind();
	m_rt.color().bind(0);
	m_screen.draw();
}
void TestRTRMApp::onShutdown() {

}