#include "sceneLoader.h"

float reflectmatrix[16];
float reflectmatrix2[16];
float reflectmatrixF[16];
extern FrameBuffer *Reflect;
extern FrameBuffer *Reflect2;
extern FrameBuffer *ReflectFloor;
extern FrameBuffer *Temp1;
extern FrameBuffer *Temp2;
extern Scene *scene;
bool Mirror2Drawn = 0;
extern GLSLProgram *FloorProg;
int texwidth,texheight;
unsigned char* FloorTexPattern;
GLuint textureID;
GLSLProgram *ReflectImageProg;
bool grayscale = false;
extern int bounces;
GLdouble floorplane[4];
GLdouble mirror1plane[4];
GLdouble mirror2plane[4];
float reflectiveness = .8;

//Model for Mirror
HalfEdgeModel *MirrorhemModel;

extern int width;
extern int height;

void InitMirror()
{
	int* mode = new int;
	FloorTexPattern = ReadPPM("./textures/squareWhiteTiles.ppm",mode,&texwidth,&texheight);
	glGenTextures(1,&textureID);
	glBindTexture(GL_TEXTURE_2D,textureID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,texwidth,texheight,0,GL_RGB,GL_UNSIGNED_BYTE,FloorTexPattern);

	PathList path("./shaders/reflectShaders/");
	FloorProg = new GLSLProgram(true,&path);
	FloorProg->SetFragmentShader("FloorFrag.glsl");
	FloorProg->SetVertexShader("FloorVert.glsl");
	FloorProg->ReloadShaders();

	ReflectImageProg = new GLSLProgram(true,&path);
	ReflectImageProg->SetFragmentShader("ReflectFrag.glsl");
	ReflectImageProg->ReloadShaders();

	//Center of mirror is at (0,0,-5.9)
	//Normal vector is (0,0,1)
	reflectmatrix[0]=1;
	reflectmatrix[1]=0;
	reflectmatrix[2]=0;
	reflectmatrix[3]=0;
	reflectmatrix[4]=0;
	reflectmatrix[5]=1;
	reflectmatrix[6]=0;
	reflectmatrix[7]=0;
	reflectmatrix[8]=0;
	reflectmatrix[9]=0;
	reflectmatrix[10]=-1;
	reflectmatrix[11]=0;
	reflectmatrix[12]=0;
	reflectmatrix[13]=0;
	reflectmatrix[14]=-11.8;
	reflectmatrix[15]=1;

	//Center of mirror is at (0,0,5.9)
	//Normal vector is (0,0,-1)
	reflectmatrix2[0]=1;
	reflectmatrix2[1]=0;
	reflectmatrix2[2]=0;
	reflectmatrix2[3]=0;
	reflectmatrix2[4]=0;
	reflectmatrix2[5]=1;
	reflectmatrix2[6]=0;
	reflectmatrix2[7]=0;
	reflectmatrix2[8]=0;
	reflectmatrix2[9]=0;
	reflectmatrix2[10]=-1;
	reflectmatrix2[11]=0;
	reflectmatrix2[12]=0;
	reflectmatrix2[13]=0;
	reflectmatrix2[14]=11.8;
	reflectmatrix2[15]=1;

	//Center of floor is at (0,-2,0)
	//Normal vector is (0,1,0)
	reflectmatrixF[0]=1;
	reflectmatrixF[1]=0;
	reflectmatrixF[2]=0;
	reflectmatrixF[3]=0;
	reflectmatrixF[4]=0;
	reflectmatrixF[5]=-1;
	reflectmatrixF[6]=0;
	reflectmatrixF[7]=0;
	reflectmatrixF[8]=0;
	reflectmatrixF[9]=0;
	reflectmatrixF[10]=1;
	reflectmatrixF[11]=0;
	reflectmatrixF[12]=0;
	reflectmatrixF[13]=-4;
	reflectmatrixF[14]=0;
	reflectmatrixF[15]=1;

	floorplane[0]=0;
	floorplane[1]=1;
	floorplane[2]=0;
	floorplane[3]=2;
	glClipPlane(GL_CLIP_PLANE0,floorplane);

	mirror1plane[0]=0;
	mirror1plane[1]=0;
	mirror1plane[2]=1;
	mirror1plane[3]=5.9;
	glClipPlane(GL_CLIP_PLANE1,mirror1plane);

	mirror2plane[0]=0;
	mirror2plane[1]=0;
	mirror2plane[2]=1;
	mirror2plane[3]=-5.9;
	glClipPlane(GL_CLIP_PLANE2,mirror2plane);

	//Load Model for Mirror
	MirrorhemModel = new HalfEdgeModel( "./models/dragon_250k.hem", TYPE_HEM_FILE );
	if (!MirrorhemModel->IsValid()) printf( "dragon_250k.hem not loaded!\n" );
	GLuint dispID = MirrorhemModel->CreateOpenGLDisplayList(USE_TRIANGLES | WITH_NORMALS);
}


void DrawFloorReflectedSceneToBuffer()
{
	ReflectFloor->BindBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glLoadIdentity();
	scene->LookAtMatrix();
	glMultMatrixf(reflectmatrixF);
	scene->SetupEnabledLightsWithCurrentModelview();
	
	glClipPlane(GL_CLIP_PLANE0,floorplane);
	glEnable(GL_CLIP_PLANE0);

	glCullFace(GL_FRONT);
	DrawSceneStuff();
	DrawFloorMirror();
	glCullFace(GL_BACK);

	glDisable(GL_CLIP_PLANE0);

	glPopMatrix();
	ReflectFloor->UnbindBuffer();
}

void DrawFloorMirror()
{
	FloorProg->EnableShader();

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,textureID);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,ReflectFloor->GetColorTextureID(0));
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

	GLint texSamplerF;
	texSamplerF = glGetUniformLocation(FloorProg->GetProgramID(),"texF");
	glUniform1i(texSamplerF,0);

	GLint texSamplerM;
	texSamplerM = glGetUniformLocation(FloorProg->GetProgramID(),"texM");
	glUniform1i(texSamplerM,1);

	GLint dim;
	dim = glGetUniformLocation(FloorProg->GetProgramID(),"width");
	glUniform1i(dim,width);
	dim = glGetUniformLocation(FloorProg->GetProgramID(),"height");
	glUniform1i(dim,height);

	glBegin(GL_QUADS);
		glMultiTexCoord2f(GL_TEXTURE0,0,0);
		glVertex3f(-6,-2,-6);
		glMultiTexCoord2f(GL_TEXTURE0,5,0);
		glVertex3f(-6,-2,6);
		glMultiTexCoord2f(GL_TEXTURE0,5,5);
		glVertex3f(6,-2,6);
		glMultiTexCoord2f(GL_TEXTURE0,0,5);
		glVertex3f(6,-2,-6);
	glEnd();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_2D);

	FloorProg->DisableShader();
}

void Draw1stReflectedSceneToBuffer(int lookback,FrameBuffer *fbo)
{
	fbo->BindBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	glMultMatrixf(reflectmatrix);
	if(lookback != 0)	Draw2ndReflectedSceneToBuffer(lookback,Temp2);
	scene->SetupEnabledLightsWithCurrentModelview();

	glClipPlane(GL_CLIP_PLANE1,mirror1plane);
	glEnable(GL_CLIP_PLANE1);

	glDisable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
//	DrawSceneStuff();
	DrawFan();
	scene->Draw();
	DrawFloorMirror();
	DrawSilModel();
	DrawExtraWalls();
	DrawSelectionObjects(GL_RENDER);
	if(Mirror2Drawn != 0)
		DrawSecondMirror(0);
	else
		Mirror2Drawn = 1;
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glDisable(GL_CLIP_PLANE1);

	glPopMatrix();
	fbo->UnbindBuffer();
}

void DrawFirstMirror(int bounced)
{
	//Prepare all data for reflection shader
	ReflectImageProg->EnableShader();
	GLint Uniforms;

	Uniforms=glGetUniformLocation(ReflectImageProg->GetProgramID(),"width");
	glUniform1i(Uniforms,width);

	Uniforms=glGetUniformLocation(ReflectImageProg->GetProgramID(),"height");
	glUniform1i(Uniforms,height);

	Uniforms=glGetUniformLocation(ReflectImageProg->GetProgramID(),"gray");
	glUniform1i(Uniforms,grayscale);

	Uniforms=glGetUniformLocation(ReflectImageProg->GetProgramID(),"inTex");
	glUniform1i(Uniforms,0);

	Uniforms=glGetUniformLocation(ReflectImageProg->GetProgramID(),"reflectiveness");
	glUniform1f(Uniforms,reflectiveness);

	//Load Texture and Draw Mirror
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);	
	if(bounced == 0)	glBindTexture(GL_TEXTURE_2D,Reflect->GetColorTextureID(0));
	else			glBindTexture(GL_TEXTURE_2D,Temp1->GetColorTextureID(0));
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

	glPushMatrix();
	glTranslatef(0,0,-5.9);
	glScalef(3,3,0);
	MirrorhemModel->CallList(USE_TRIANGLES);
	glPopMatrix();

/*
	//For a rectangular mirror
	glBegin(GL_QUADS);
		glVertex3f(-2,-1.8,-5.9);
		glVertex3f(2,-1.8,-5.9);
		glVertex3f(2,3,-5.9);
		glVertex3f(-2,3,-5.9);
	glEnd();
*/
	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_2D);

	ReflectImageProg->DisableShader();
}

void Draw2ndReflectedSceneToBuffer(int lookback,FrameBuffer *fbo)
{
	fbo->BindBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	glMultMatrixf(reflectmatrix2);
	Draw1stReflectedSceneToBuffer(lookback-1,Temp1);
	scene->SetupEnabledLightsWithCurrentModelview();

	glClipPlane(GL_CLIP_PLANE2,mirror2plane);
	//glEnable(GL_CLIP_PLANE2);
	
	glCullFace(GL_FRONT);
	DrawSceneStuff();
	DrawFloorMirror();
	DrawFirstMirror(1);
	glCullFace(GL_BACK);

	glDisable(GL_CLIP_PLANE2);

	glPopMatrix();
	fbo->UnbindBuffer();
}

void DrawSecondMirror(int bounced)
{
	//Prepare all data for reflection shader
	ReflectImageProg->EnableShader();
	GLint Uniforms;

	Uniforms=glGetUniformLocation(ReflectImageProg->GetProgramID(),"width");
	glUniform1i(Uniforms,width);

	Uniforms=glGetUniformLocation(ReflectImageProg->GetProgramID(),"height");
	glUniform1i(Uniforms,height);

	Uniforms=glGetUniformLocation(ReflectImageProg->GetProgramID(),"gray");
	glUniform1i(Uniforms,false);

	Uniforms=glGetUniformLocation(ReflectImageProg->GetProgramID(),"inTex");
	glUniform1i(Uniforms,0);

	Uniforms=glGetUniformLocation(ReflectImageProg->GetProgramID(),"reflectiveness");
	glUniform1f(Uniforms,reflectiveness);

	//Load texture and draw mirror
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	if(bounced == 0)	glBindTexture(GL_TEXTURE_2D,Reflect2->GetColorTextureID(0));
	else			glBindTexture(GL_TEXTURE_2D,Temp2->GetColorTextureID(0));
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	glBegin(GL_QUADS);
		glVertex3f(3,-1.8,5.9);
		glVertex3f(-3,-1.8,5.9);
		glVertex3f(-3,2.5,5.9);
		glVertex3f(3,2.5,5.9);
	glEnd();
	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_2D);

	ReflectImageProg->DisableShader();
}
