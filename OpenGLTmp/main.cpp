#include<glad/gl.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<spdlog/spdlog.h>
#include"LoadStringFile.h"
#include <filesystem>
#include <cstddef>
#include"CHRONO.h"
#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>
#include"GLShader.h"

struct Vertex {
	glm::vec3 position;
	glm::vec2 uv;
};
struct alignas(16) SceneConstants {
	glm::mat4 world;
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec4 eye;
	float time;
	int32_t pad[3];
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

	//頂点座標
	const Vertex vertices[] =
	{
		 {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
		{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
		{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}},
		{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}}
	};

	const unsigned int indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
	GLuint ubo = 0;
	GLuint texture = 0;
	//vao作成
	glGenVertexArrays(1, &vao);
	//バッファ生成
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenBuffers(1, &ubo);
	//テクスチャ作成
	glGenTextures(1, &texture);

	//vaoバインド
	glBindVertexArray(vao);

	//vboをバインド
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//バッファにデータコピー
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(vertices),
		vertices,
		GL_STATIC_DRAW
	);

	//eboバインド
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//コピー
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		sizeof(indices),
		indices,
		GL_STATIC_DRAW
	);

	//uboバインド
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	//コピー
	glBufferData(
		GL_UNIFORM_BUFFER,
		sizeof(SceneConstants),
		nullptr,
		GL_DYNAMIC_DRAW
	);

	//レイアウト設定
	glBindBufferBase(
		GL_UNIFORM_BUFFER,
		0,
		ubo
	);
	//頂点属性0登録
	glVertexAttribPointer(
		0,							//属性変数の位置
		3,							//成分数(float3)
		GL_FLOAT,					//型
		GL_FALSE,					//整数データの正規化
		sizeof(Vertex),				// 次の頂点までの間隔（stride）
		reinterpret_cast<void*>(
			offsetof(Vertex, position)
			)						//VBO内のバイトオフセット
	);
	//属性0を有効
	glEnableVertexAttribArray(0);
	//頂点属性1登録
	glVertexAttribPointer(
		1,							//属性変数の位置
		2,							//成分数(float2)
		GL_FLOAT,					//型
		GL_FALSE,					//整数データの正規化
		sizeof(Vertex),				// 次の頂点までの間隔（stride）
		reinterpret_cast<void*>(
			offsetof(Vertex, uv)
			)						//VBO内のバイトオフセット
	);
	//属性1を有効
	glEnableVertexAttribArray(1);
	//バインド解除
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//テクスチャ読み込み
	int w, h, c;
	std::string texPath = "assets\\data\\texture\\y.jpg";
	unsigned char* data = stbi_load(texPath.c_str(), &w, &h, &c, STBI_rgb_alpha);
	if (!data) {
		spdlog::critical(
			"Failed to load texture file\n"
			"Relative path: {}\n"
			"Absolute path: {}",
			texPath,
			std::filesystem::absolute(texPath).string()
		);
	}
	//バインド
	glBindTexture(GL_TEXTURE_2D, texture);
	//書き込み
	glTexImage2D(
		GL_TEXTURE_2D,	//2D
		0,				//ミニマップレベル
		GL_RGBA8,		//形式
		w,				//幅
		h,				//高さ
		0,				//常に0
		GL_RGBA,		//並び
		GL_UNSIGNED_BYTE,//型
		data
	);
	stbi_image_free(data);
	//拡大縮小のときは近いピクセルを使う
	//glTexParameteri(
	//	GL_TEXTURE_2D,
	//	GL_TEXTURE_MIN_FILTER,
	//	GL_NEAREST
	//);
	//glTexParameteri(
	//	GL_TEXTURE_2D,
	//	GL_TEXTURE_MAG_FILTER,
	//	GL_NEAREST
	//);
	//拡大縮小では滑らかに補完します
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER,
		GL_LINEAR
	);

	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER,
		GL_LINEAR
	);
	//uv範囲外のときはリピート
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S,
		GL_REPEAT
	);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_T,
		GL_REPEAT
	);
	//バインド解除
	glBindTexture(GL_TEXTURE_2D, 0);

	//深度バッファ有効&比較関数指定
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);

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
		delta = chrono.getElapsed();
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

		//uboバインド
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		//uboにコピー
		glBufferSubData(
			GL_UNIFORM_BUFFER,
			0,
			sizeof(SceneConstants),
			&constants
		);

		//シェーダーをセット
		shaderProgram.bind();
		//テクスチャセット
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		//頂点をセット
		glBindVertexArray(vao);
		//描画
		glDrawElements(
			GL_TRIANGLES,
			std::size(indices),
			GL_UNSIGNED_INT,
			reinterpret_cast<void*>(0)
		);
		//バインド解除
		glBindVertexArray(0);

		//画面スワップ
		glfwSwapBuffers(window);
		//イベント処理
		glfwPollEvents();
	}
	//削除
	glDeleteTextures(1, &texture);
	glDeleteBuffers(1, &ubo);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	glfwDestroyWindow(window);
	spdlog::info("destroied window");
	glfwTerminate();
	spdlog::info("terminated glfw");
}