#include "sceneLoader.h"

GLSLProgram *GoochProg;

void LoadGoochShader()
{
	PathList path("./shaders/goochShaders/");
	GoochProg = new GLSLProgram(true,&path);
	GoochProg->SetVertexShader("GoochVert.glsl");
	GoochProg->LinkProgram();
}
