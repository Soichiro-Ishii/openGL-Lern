#include "pch.h"
#include"OpenGLLearnApp.h"
//#include"TestRTRMApp.h"


int main() {
	OpenGLLearnApp app;
	int result = app.run(1920, 1080, "openGLLearn", false);
	return result;

	//TestRTRMApp app;
	//int result = app.run(1920, 1080, "openGLLearn", false);
	//return result;
}