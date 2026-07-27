#include "pch.h"
#include "PostProcessPass.h"

PostProcessPass::PostProcessPass(GLShader& shader, int width, int height, ColorTexSet cSet) {
	create(shader, width, height, cSet);
}

PostProcessPass::PostProcessPass(PostProcessPass&& other) noexcept {
	m_shader = std::exchange(other.m_shader, nullptr);
	m_rt = std::move(other.m_rt);
}
PostProcessPass& PostProcessPass::operator=(PostProcessPass&& other) noexcept {
	if (this != &other) {
		m_shader = std::exchange(other.m_shader, nullptr);
		m_rt = std::move(other.m_rt);
	}
	return *this;
}

void PostProcessPass::create(GLShader& shader, int width, int height, ColorTexSet cSet) {
	m_shader = &shader;
	m_rt.create(width, height, cSet);
}

void PostProcessPass::resize(int width, int height) {
	m_rt.resize(width, height);
}

void PostProcessPass::execute(const GLTexture2D& texture, const GLMesh& screen) {
	m_rt.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	m_shader->bind();
	texture.bind(0);
	screen.draw();

	m_rt.unbind();
}