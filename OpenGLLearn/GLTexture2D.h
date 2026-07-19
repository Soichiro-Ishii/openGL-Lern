#pragma once
#include <glad/gl.h>
#include <string>

enum class TEXTURE2DFILTER {
	NEAREST,
	LINER,
};
enum class TEXTURE2DWRAP {
	REPEAT,
	CLAMP_TO_EDGE,
	MIRRORED_REPEAT
};
struct TEXTURE2DSETTING {
	TEXTURE2DFILTER filter;
	TEXTURE2DWRAP wrap;
};


class GLTexture2D
{
	GLuint m_id = 0;
	int m_width = 0;
	int m_height = 0;
	TEXTURE2DSETTING m_set;
public:
	GLTexture2D() = default;
	GLTexture2D(std::string texPath, TEXTURE2DSETTING set);
	~GLTexture2D();

	GLTexture2D(GLTexture2D&) = delete;
	GLTexture2D operator=(GLTexture2D&) = delete;

	GLTexture2D(GLTexture2D&& other) noexcept;
	GLTexture2D& operator=(GLTexture2D&& other) noexcept;

	bool load(std::string& texPath, TEXTURE2DSETTING set);
	void bind(GLuint textureUnit);
	void unBind(GLuint textureUnit);

	[[nodiscard]] GLuint id() const {
		return m_id;
	}

	[[nodiscard]] bool valid() const {
		return m_id != 0;
	}
	[[nodiscard]] int width() const {
		return m_width;
	}
	[[nodiscard]] int height() const {
		return m_height;
	}
private:
	void release();
};