#pragma once
#include <glad/gl.h>
#include <string>

class GLShader
{
	GLuint m_id = 0;
	static GLuint CompileShader(GLenum shaderType, const char* source);	//シェーダーコンパイル
public:
	GLShader() = default;
	GLShader(const char* vsPath, const char* fsPath);
	~GLShader();
	GLShader(const GLShader&) = delete;
	GLShader operator=(const GLShader&) = delete;
	GLShader(GLShader&& other) noexcept;
	GLShader& operator=(GLShader&& other) noexcept;
	void load(const char* vsPath, const char* fsPath);
	void bind() const;
	[[nodiscard]] GLuint id() const
	{
		return m_id;
	}

	[[nodiscard]] bool valid() const
	{
		return m_id != 0;
	}
private:
	void release();//解放
};

