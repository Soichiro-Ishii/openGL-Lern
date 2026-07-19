#include "GLTexture2D.h"
#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>
#include<spdlog/spdlog.h>
#include <filesystem>

GLTexture2D::GLTexture2D(std::string texPath, TEXTURE2DSETTING set) {
	load(texPath, set);
}

GLTexture2D::~GLTexture2D() {
	release();
}

GLTexture2D::GLTexture2D(GLTexture2D&& other) noexcept {
	m_id = std::exchange(other.m_id, 0);
	m_width = std::exchange(other.m_width, 0);
	m_height = std::exchange(other.m_height, 0);
}
GLTexture2D& GLTexture2D::operator=(GLTexture2D&& other) noexcept {
	if (this != &other) {
		release();
		m_id = std::exchange(other.m_id, 0);
		m_width = std::exchange(other.m_width, 0);
		m_height = std::exchange(other.m_height, 0);
	}
	return *this;
}

bool GLTexture2D::load(std::string& texPath, TEXTURE2DSETTING set) {
	release();

	int originalChannels = 0;

	unsigned char* data = stbi_load(
		texPath.c_str(),
		&m_width,
		&m_height,
		&originalChannels,
		4
	);

	if (!data) {
		spdlog::error(
			"Failed to load texture\n"
			"Reason: {}\n"
			"Relative path: {}\n"
			"Absolute path: {}",
			stbi_failure_reason(),
			texPath,
			std::filesystem::absolute(texPath).string()
		);

		m_width = 0;
		m_height = 0;
		return false;
	}
	//テクスチャ作成
	glGenTextures(1, &m_id);
	//バインド
	glBindTexture(GL_TEXTURE_2D, m_id);
	//書き込み
	glTexImage2D(
		GL_TEXTURE_2D,	//2D
		0,				//ミニマップレベル
		GL_RGBA8,		//形式
		m_width,		//幅
		m_height,		//高さ
		0,				//常に0
		GL_RGBA,		//並び
		GL_UNSIGNED_BYTE,//型
		data
	);
	//フィルター設定
	GLuint filter;
	if (set.filter == TEXTURE2DFILTER::NEAREST)
		filter = GL_NEAREST;
	else
		filter = GL_LINEAR;
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER,
		filter
	);

	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER,
		filter
	);
	//uv範囲外設定
	GLuint wrap;
	if (set.wrap == TEXTURE2DWRAP::REPEAT)
		wrap = GL_REPEAT;
	else if (set.wrap == TEXTURE2DWRAP::CLAMP_TO_EDGE)
		wrap = GL_CLAMP_TO_EDGE;
	else
		filter = GL_MIRRORED_REPEAT;
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S,
		wrap
	);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_T,
		wrap
	);
	m_set = set;
	//バインド解除
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);

	spdlog::info(
		"Texture loaded: {} ({}x{}, channels={})",
		texPath,
		m_width,
		m_height,
		originalChannels
	);

	return true;
}
void GLTexture2D::bind(GLuint textureId) {
	glActiveTexture(GL_TEXTURE0 + textureId);
	glBindTexture(GL_TEXTURE_2D, m_id);
}
void GLTexture2D::unBind(GLuint textureId) {
	glActiveTexture(GL_TEXTURE0 + textureId);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTexture2D::release() {
	if (m_id != 0) {
		glDeleteTextures(1, &m_id);
		m_id = 0;
	}
	m_width = 0;
	m_height = 0;
}