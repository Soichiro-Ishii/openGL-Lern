#include "pch.h"
#include"OpenGLLearnApp.h"


int main() {
	OpenGLLearnApp* app = new OpenGLLearnApp;
	return app->run(1920, 1080, "openGLLearn", true, 0, 120.0f);
}