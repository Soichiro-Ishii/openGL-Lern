#include "pch.h"

#include"CHRONO.h"
#include"GLShader.h"
#include"GLTexture2D.h"
#include"GLUniformBuffer.h"
#include"GLMesh.h"
#include"ProcMeshGenerator.h"
#include"GLContext.h"

struct alignas(16) SceneConstants {
	glm::mat4 world;
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec4 eye;
	float time;
	glm::vec2 res;
	int32_t pad;
};

static void GLAPIENTRY OpenGLDebugCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		spdlog::critical(
			"OpenGL HIGH error [{}]: {}",
			id,
			message
		);
		break;

	case GL_DEBUG_SEVERITY_MEDIUM:
		spdlog::error(
			"OpenGL MEDIUM error [{}]: {}",
			id,
			message
		);
		break;

	case GL_DEBUG_SEVERITY_LOW:
		spdlog::warn(
			"OpenGL LOW error [{}]: {}",
			id,
			message
		);
		break;

	case GL_DEBUG_SEVERITY_NOTIFICATION:
		spdlog::debug(
			"OpenGL notification [{}]: {}",
			id,
			message
		);
		break;
	}
}

int main() {
	GLContext context;
	if (!context.create(1920, 1080, "openGL-Learn", true)) {
		spdlog::critical("faild creating context");
		return -1;
	}
	//シェーダー読み込み
	GLShader shaderProgram("assets\\shaders\\vs.glsl", "assets\\shaders\\fs.glsl");
	if (!shaderProgram.valid())
	{
		glfwDestroyWindow(context.window());
		glfwTerminate();
		return -1;
	}

	//頂点データ
	GLMeshData meshData = ProcMeshGenerator::createSphere();
	//メッシュ作成
	GLMesh mesh(meshData);
	//ubo
	GLUniformBuffer ubo(nullptr, sizeof(SceneConstants), 0);

	GLTexture2D texture;
	std::string texPath = "assets\\data\\texture\\8k_earth_daymap.jpg";
	TEXTURE2DSETTING set = { TEXTURE2DFILTER::LINEAR ,TEXTURE2DWRAP::REPEAT };
	if (!texture.load(texPath, set)) {
		glfwDestroyWindow(context.window());
		spdlog::info("destroied window");
		glfwTerminate();
		spdlog::info("terminated glfw");
		return -1;
	}
	//深度バッファ有効&比較関数指定
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);
	glEnable(GL_CULL_FACE); // カリングを有効化
	glCullFace(GL_BACK);

	//UBO用の値
	SceneConstants constants{};
	constants.world = glm::mat4(1.0f);
	constants.view = glm::mat4(1.0f);
	constants.proj = glm::mat4(1.0f);
	constants.eye = glm::vec4(0.0f, 0.0f, -5.0f, 1.0f);
	constants.time = 0.0f;
	float delta = 0.0f;
	CHRONO chrono;
	chrono.timeStart();
	while (!glfwWindowShouldClose(context.window())) {
		chrono.timeEnd();
		delta = static_cast<float>(chrono.getElapsed());
		chrono.timeStart();

		int framebufferWidth = 0;
		int framebufferHeight = 0;
		glfwGetFramebufferSize(context.window(), &framebufferWidth, &framebufferHeight);
		//viewPortの設定
		glViewport(0, 0, framebufferWidth, framebufferHeight);
		//画面クリア色設定&深度クリアの値設定
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//画面クリア
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//一応毎回更新
		float aspect = static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight);
		constants.time += delta;
		constants.eye = glm::vec4(cos(glm::radians(constants.time * 45.0f)), 0.0f, sin(glm::radians(constants.time * 45.0f)), 0.0f) * 5.0f;
		constants.world = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin(glm::radians(constants.time * 360.0f)), 0.0f));
		constants.world *= glm::rotate(glm::mat4(1.0f), glm::radians(constants.time * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		constants.world *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 3.0f));
		constants.world *= glm::rotate(glm::mat4(1.0f), glm::radians(constants.time * 75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		constants.world *= glm::rotate(glm::mat4(1.0f), glm::radians(constants.time * 100.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		constants.view = glm::lookAt(
			glm::vec3(constants.eye.x, constants.eye.y, constants.eye.z),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);
		constants.proj = glm::perspective(
			glm::radians(90.0f),
			aspect,
			0.1f,
			1000.0f
		);

		ubo.update(&constants, sizeof(SceneConstants), 0);

		//シェーダーをセット
		shaderProgram.bind();
		//テクスチャセット
		texture.bind(0);
		//メッシュ描画
		mesh.draw();

		//画面スワップ
		glfwSwapBuffers(context.window());
		//イベント処理
		glfwPollEvents();
	}
	texture.release();
	ubo.release();
	mesh.release();
	shaderProgram.release();

	context.release();
}