#pragma once
#include <glad/gl.h>
#include <string>

class GLShader
{
	GLuint id = 0;
	static GLuint CompileShader(GLenum shaderType, const char* source);	//シェーダーコンパイル
	void release();//解放
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
	GLuint getId() const
	{
		return id;
	}

	bool valid() const
	{
		return id != 0;
	}
};

