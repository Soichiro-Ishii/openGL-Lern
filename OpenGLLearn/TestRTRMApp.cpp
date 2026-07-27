#include "pch.h"
#include "TestRTRMApp.h"
#include"ProcMeshGenerator.h"

int TestRTRMApp::onInit() {
	GLMeshData meshData;
	meshData = ProcMeshGenerator::createScreen();
	m_screen.create(meshData);

	m_RTRMshader.load("assets\\shaders\\screenVS.glsl", "assets\\shaders\\blackHoleRayMarch1FS.glsl");
	if (!m_RTRMshader.valid()) return -1;

	m_blurShader.load("assets\\shaders\\screenVS.glsl", "assets\\shaders\\blurFS.glsl");
	if (!m_blurShader.valid()) return -1;

	m_lastShader.load("assets\\shaders\\screenVS.glsl", "assets\\shaders\\renderTexFS.glsl");
	if (!m_lastShader.valid()) return -1;

	m_inverseShader.load("assets\\shaders\\screenVS.glsl", "assets\\shaders\\inverseFS.glsl");
	if (!m_inverseShader.valid()) return -1;


	std::string texPath1 = "assets\\data\\texture\\starmap_2020_8k2.png";
	TEXTURE2DSETTING set = { TEXTURE2DFILTER::LINEAR ,TEXTURE2DWRAP::REPEAT };
	if (!m_sky.load(texPath1, set)) return -1;

	m_BHConsts.resolution.x = widthf();
	m_BHConsts.resolution.y = heightf();
	m_BHConsts.time = 0.0f;
	m_BHCUB.create(nullptr, sizeof(BlackHoleConstants), 0);

	ColorTexSet cSet = ColorTexSet::NORMAL;
	m_BHrt.create(width(), height(), cSet);
	for (auto& rt : m_blurPP)
		rt.create(m_blurShader, width(), height(), cSet);
	int blurRep = 10;
	m_blurPPC.allocate(blurRep);
	for (int i = 0; i < blurRep; i++) {
		m_blurPPC.add(m_blurPP[i % 2]);
	}
	m_inversePP.create(m_inverseShader, width(), height(), cSet);

	return 0;
}
void TestRTRMApp::onUpdate(float delta) {
	//viewPortの設定
	glViewport(0, 0, width(), height());
	m_BHrt.resize(width(), height());
	m_blurPPC.resize(width(), height());
	m_inversePP.resize(width(), height());
	m_BHConsts.time += delta;
	m_BHConsts.resolution.x = widthf();
	m_BHConsts.resolution.y = heightf();
	m_BHCUB.update(&m_BHConsts, sizeof(BlackHoleConstants), 0);
}
void TestRTRMApp::onRender() {
	m_BHrt.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	m_RTRMshader.bind();
	m_sky.bind(0);
	m_screen.draw();

	m_blurPPC.execute(m_BHrt.color(), m_screen);

	m_inversePP.execute(m_blurPPC.output(), m_screen);

	m_lastShader.bind();
	m_inversePP.output().bind(0);
	m_screen.draw();
}
void TestRTRMApp::onShutdown() {

}