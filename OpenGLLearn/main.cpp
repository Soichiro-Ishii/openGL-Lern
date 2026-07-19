#include "pch.h"

#include"CHRONO.h"
#include"GLShader.h"
#include"GLTexture2D.h"
#include"GLUniformBuffer.h"
#include"GLMesh.h"
#include"ProcMeshGenerator.h"

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
	//glfw初期化
	if (!glfwInit()) {
		spdlog::critical("faild initialization glfw");
		return -1;
	}
	spdlog::info("glfw initialized");
	//バージョン指定
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//coreにする
	glfwWindowHint(GLFW_OPENGL_PROFILE,
		GLFW_OPENGL_CORE_PROFILE);
	//デバッグ出力有効
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	//ウィンドウ作成
	GLFWwindow* window = glfwCreateWindow(1280, 720, "OpenGL", nullptr, nullptr);
	if (!window) {
		spdlog::critical("faild creating window");
		glfwTerminate();
		spdlog::info("terminated glfw");
		return -1;
	}
	spdlog::info("window created");
	//コンテキスト作成
	glfwMakeContextCurrent(window);
	//gladのglロード
	if (!gladLoadGL(glfwGetProcAddress)) {
		spdlog::critical("faild loading gl");
		glfwDestroyWindow(window);
		spdlog::info("destroied window");
		glfwTerminate();
		spdlog::info("terminated glfw");
		return -1;
	}
	spdlog::info("gl loded");
	//デバッグ出力有効
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glDebugMessageCallback(
		OpenGLDebugCallback,
		nullptr
	);
	//シェーダー読み込み
	GLShader shaderProgram("assets\\shaders\\vs.glsl", "assets\\shaders\\fs.glsl");
	if (!shaderProgram.valid())
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	//頂点データ
	GLMeshData meshData = ProcMeshGenerator::createSphere();
	//メッシュ作成
	GLMesh mesh(&meshData);

	GLUniformBuffer ubo(nullptr, sizeof(SceneConstants), 0);

	GLTexture2D texture;
	std::string texPath = "assets\\data\\texture\\8k_earth_daymap.jpg";
	TEXTURE2DSETTING set = { TEXTURE2DFILTER::LINER ,TEXTURE2DWRAP::REPEAT };
	if (!texture.load(texPath, set)) {
		glfwDestroyWindow(window);
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
	while (!glfwWindowShouldClose(window)) {
		chrono.timeEnd();
		delta = static_cast<float>(chrono.getElapsed());
		chrono.timeStart();

		int framebufferWidth = 0;
		int framebufferHeight = 0;
		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
		//viewPortの設定
		glViewport(0, 0, framebufferWidth, framebufferHeight);
		//画面クリア色設定&深度クリアの値設定
		glClearColor(0.4f, 0.7f, 0.7f, 1.0f);
		//画面クリア
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//一応毎回更新
		float aspect = static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight);
		constants.time += delta;
		constants.eye = glm::vec4(0.0f, 0.0f, -5.0f, 0.0f);
		constants.world = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin(glm::radians(constants.time * 520.0f)), 0.0f));
		constants.world *= glm::rotate(glm::mat4(1.0f), glm::radians(constants.time * 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		constants.world *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 3.0f));
		constants.world *= glm::rotate(glm::mat4(1.0f), glm::radians(constants.time * 150.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		constants.world *= glm::rotate(glm::mat4(1.0f), glm::radians(constants.time * 200.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		constants.view = glm::lookAt(
			glm::vec3(constants.eye.x, constants.eye.y, constants.eye.z),
			glm::vec3(0, 0, 1),
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
		glfwSwapBuffers(window);
		//イベント処理
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	spdlog::info("destroied window");
	glfwTerminate();
	spdlog::info("terminated glfw");
}