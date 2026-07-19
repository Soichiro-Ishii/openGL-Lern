#pragma once
#include"GLMeshData.h"
class GLMesh
{
private:
	GLuint m_vao = 0;
	GLuint m_vbo = 0;
	GLuint m_ebo = 0;
	GLsizei m_indexCount = 0;
	GLsizei m_vertexCount = 0;
	GLenum m_primitiveMode = GL_TRIANGLES;
public:
	GLMesh() = default;
	GLMesh(GLMeshData* data);
	~GLMesh();

	GLMesh(GLMesh&) = delete;
	GLMesh& operator=(GLMesh&) = delete;

	GLMesh(GLMesh&& other) noexcept;
	GLMesh& operator=(GLMesh&& other) noexcept;

	void create(GLMeshData* data);
	void draw();

	[[nodiscord]] GLuint vao() {
		return m_vao;
	}
	[[nodiscord]] GLuint vbo() {
		return m_vbo;
	}
	[[nodiscord]] GLuint ebo() {
		return m_ebo;
	}
	[[nodiscord]] GLuint vertexCount() {
		return m_vertexCount;
	}
	[[nodiscord]] GLuint indexCount() {
		return m_indexCount;
	}
private:
	void release();
};

