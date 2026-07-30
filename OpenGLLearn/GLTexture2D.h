#pragma once
#include <glad/gl.h>
#include <string>

enum class TEXTURE2DFILTER {
	NEAREST,
	LINEAR,
};
enum class TEXTURE2DWRAP {
	REPEAT,
	CLAMP_TO_EDGE,
	MIRRORED_REPEAT
};
struct TEXTURE2DSETTING {
	TEXTURE2DFILTER filter = TEXTURE2DFILTER::NEAREST;
	TEXTURE2DWRAP wrap = TEXTURE2DWRAP::REPEAT;
};

struct TEXTURE2DDESC {
	TEXTURE2DSETTING set;
	GLenum internalFormat = GL_RGBA8;
	GLenum format = GL_RGBA;
	GLenum type = GL_UNSIGNED_BYTE;
	int width = 0;
	int height = 0;
};

class GLTexture2D
{
	GLuint m_id = 0;
	TEXTURE2DDESC m_desc{};
public:
	GLTexture2D() = default;
	GLTexture2D(std::string texPath, TEXTURE2DSETTING set);
	GLTexture2D(TEXTURE2DDESC& desc);
	~GLTexture2D();

	GLTexture2D(GLTexture2D&) = delete;
	GLTexture2D& operator=(GLTexture2D&) = delete;

	GLTexture2D(GLTexture2D&& other) noexcept;
	GLTexture2D& operator=(GLTexture2D&& other) noexcept;

	bool load(std::string& texPath, TEXTURE2DSETTING set);
	bool load(std::string& texPath, TEXTURE2DDESC& desc);
	void create(TEXTURE2DDESC& desc);
	void bind(GLuint textureUnit) const;
	void unBind(GLuint textureUnit) const;

	void resize(int width, int height);

	[[nodiscard]] GLuint id() const {
		return m_id;
	}

	[[nodiscard]] bool valid() const {
		return m_id != 0;
	}
	[[nodiscard]] int width() const {
		return m_desc.width;
	}
	[[nodiscard]] int height() const {
		return m_desc.height;
	}
	void release();
};