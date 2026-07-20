#include "pch.h"
#include"OpenGLLearnApp.h"


int main() {
	OpenGLLearnApp* app = new OpenGLLearnApp;
	int result = app->run(1920, 1080, "openGLLearn");
	delete app;
	return result;
}