#include "sceneLoader.h"
HalfEdgeModel *hemModel;
GLSLProgram *SilhouetteProg;
GLuint dispID;

void LoadSilModel()
{
	hemModel = new HalfEdgeModel( "./models/triceratops.hem", TYPE_HEM_FILE );
	if (!hemModel->IsValid()) printf( "triceratops.hem not loaded!\n" );
	dispID = hemModel->CreateOpenGLDisplayList(USE_LINES | WITH_ADJACENT_FACE_NORMS);

	PathList path("./shaders/silhouetteShaders/");
	SilhouetteProg = new GLSLProgram(true,&path);
	SilhouetteProg->SetVertexShader("SilVert.glsl");
	SilhouetteProg->SetGeometryShader("SilGeom.glsl");
	SilhouetteProg->GeometryShaderSettings(GL_LINES,2,GL_LINE_STRIP);
	SilhouetteProg->LinkProgram();
}

void DrawSilModel()
{
	glPushMatrix();
	glTranslatef( -2, -1.55, -4 );
	SilhouetteProg->EnableShader();

	GLuint Uniforms;
	Uniforms=glGetUniformLocation(SilhouetteProg->GetProgramID(),"SilCol");
	glUniform4f(Uniforms,1,1,1,1);
	glLineWidth(5);
	hemModel->CallList(USE_LINES);
	glLineWidth(1);

	SilhouetteProg->DisableShader();
	glPopMatrix();
}

