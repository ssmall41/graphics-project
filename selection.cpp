#include "sceneLoader.h"

//From Gooch
extern GLSLProgram *GoochProg,*GoochSilhouetteProg;
extern Scene *scene;

//Models
GLMmodel *SelectTeapot;
HalfEdgeModel *SelectTriceratops;
GLMmodel *SelectDoor;
GLMmodel *SelectFridge;
GLMmodel *SelectSinkStand;
GLMmodel *SelectSink;
GLMmodel *SelectTable;
GLMmodel *SelectChair;
GLMmodel *SelectSideboard;
GLMmodel *SelectMicrowave;
GLMmodel *SelectCabinet;
//Display Lists
GLuint TeapotList;
GLuint TriceratopsList;
GLuint DoorList;
GLuint FridgeList;
GLuint SinkStandList;
GLuint SinkList;
GLuint TableList;
GLuint ChairList;
GLuint SideboardList;
GLuint MicrowaveList;
GLuint CabinetList;
//Using Gooch shading?
int TeapotGooch = 0;
int TriceratopsGooch = 0;
int DoorGooch = 0;
int FridgeGooch = 0;
int SinkStandGooch = 0;	//Will I ever use a variable name like this again in my entire life?
int TableGooch = 0;
int Chair1Gooch = 0;
int Chair2Gooch = 0;
int Chair3Gooch = 0;
int MicrowaveGooch = 0;
int Sideboard1a2Gooch = 0;
int Sideboard3a4Gooch = 0;
int Cabinet1Gooch = 0;


void LoadSelectionModels()
{

	//Setup Teapot
	SelectTeapot = glmReadOBJ( "./models/teapot.obj" );
	glmUnitize( SelectTeapot );
	glmFacetNormals( SelectTeapot );
	glmVertexNormals( SelectTeapot, 180 );

	TeapotList = glGenLists(1);
	glNewList(TeapotList,GL_COMPILE);
	 glmDraw( SelectTeapot, GLM_SMOOTH );
	glEndList();

	//Setup Triceratops
	SelectTriceratops = new HalfEdgeModel( "./models/triceratops.hem", TYPE_HEM_FILE );
	if (!SelectTriceratops->IsValid()) printf( "triceratops.hem not loaded!\n" );
	TriceratopsList = SelectTriceratops->CreateOpenGLDisplayList(USE_TRIANGLES | WITH_NORMALS);

	//Setup Door
	SelectDoor = glmReadOBJ( "./models/selection/door.obj" );
	glmUnitize( SelectDoor );
	glmFacetNormals( SelectDoor );
	glmVertexNormals( SelectDoor, 180 );

	DoorList = glGenLists(1);
	glNewList(DoorList,GL_COMPILE);
	 glmDraw( SelectDoor, GLM_SMOOTH );
	glEndList();

	//Setup Fridge
	SelectFridge = glmReadOBJ( "./models/selection/fridge1.obj" );
	glmUnitize( SelectFridge );
	glmFacetNormals( SelectFridge );
	glmVertexNormals( SelectFridge, 180 );

	FridgeList = glGenLists(1);
	glNewList(FridgeList,GL_COMPILE);
	 glmDraw( SelectFridge, GLM_SMOOTH );
	glEndList();

	//Setup SinkStand
	SelectSinkStand = glmReadOBJ( "./models/selection/sinkstand.obj" );
	glmUnitize( SelectSinkStand );
	glmFacetNormals( SelectSinkStand );
	glmVertexNormals( SelectSinkStand, 180 );

	SinkStandList = glGenLists(1);
	glNewList(SinkStandList,GL_COMPILE);
	 glmDraw( SelectSinkStand, GLM_SMOOTH );
	glEndList();

	//Setup Sink
	SelectSink = glmReadOBJ( "./models/selection/sink.obj" );
	glmUnitize( SelectSink );
	glmFacetNormals( SelectSink );
	glmVertexNormals( SelectSink, 180 );

	SinkList = glGenLists(1);
	glNewList(SinkList,GL_COMPILE);
	 glmDraw( SelectSink, GLM_FLAT);
	glEndList();

	//Setup Table
	SelectTable = glmReadOBJ( "./models/selection/table1.obj" );
	glmUnitize( SelectTable );
	glmFacetNormals( SelectTable );
	glmVertexNormals( SelectTable, 180 );

	TableList = glGenLists(1);
	glNewList(TableList,GL_COMPILE);
	 glmDraw( SelectTable, GLM_SMOOTH );
	glEndList();

	//Setup Chair
	SelectChair = glmReadOBJ( "./models/selection/chair1.obj" );
	glmUnitize( SelectChair );
	glmFacetNormals( SelectChair );
	glmVertexNormals( SelectChair, 180 );

	ChairList = glGenLists(1);
	glNewList(ChairList,GL_COMPILE);
	 glmDraw( SelectChair, GLM_SMOOTH );
	glEndList();


	//Setup Sideboard
	SelectSideboard = glmReadOBJ( "./models/selection/sideboard.obj" );
	glmUnitize( SelectSideboard );
	glmFacetNormals( SelectSideboard );
	glmVertexNormals( SelectSideboard, 180 );

	SideboardList = glGenLists(1);
	glNewList(SideboardList,GL_COMPILE);
	 glmDraw( SelectSideboard, GLM_FLAT );
	glEndList();

	//Setup Microwave
	SelectMicrowave = glmReadOBJ( "./models/selection/microwave.obj" );
	glmUnitize( SelectMicrowave );
	glmFacetNormals( SelectMicrowave );
	glmVertexNormals( SelectMicrowave, 180 );

	MicrowaveList = glGenLists(1);
	glNewList(MicrowaveList,GL_COMPILE);
	 glmDraw( SelectMicrowave, GLM_SMOOTH );
	glEndList();

	//Setup Cabinet
	SelectCabinet = glmReadOBJ( "./models/selection/cabinet.obj" );
	glmUnitize( SelectCabinet );
	glmFacetNormals( SelectCabinet );
	glmVertexNormals( SelectCabinet, 180 );

	CabinetList = glGenLists(1);
	glNewList(CabinetList,GL_COMPILE);
	 glmDraw( SelectCabinet, GLM_SMOOTH );
	glEndList();

}

void DrawSelectionObjects(GLenum mode)
{

	glDisable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);

	//Draw Teapot***********************************************************
	glColor3f(1,.5,.6);
	glPushMatrix();
	glTranslatef(-5.5,-.41,0);
	glRotatef(90,0,1,0);
	glScalef(.3,.3,.3);
	glLoadName(1);

	if(TeapotGooch == 1 && mode != GL_SELECT)
	{
		GoochProg->EnableShader();
		GLuint Uniforms;
		Uniforms=glGetUniformLocation(GoochProg->GetProgramID(),"LightPos");
		Point P = scene->GetLight(0)->GetCurrentPos();
		glUniform3f(Uniforms,P.X(),P.Y(),P.Z());
	}
	glCallList(TeapotList);
	if(TeapotGooch == 1 && mode != GL_SELECT)	GoochProg->DisableShader();

	glPopMatrix();

	//Draw Triceratops***********************************************************
	glColor3f(1,1,0);
	glPushMatrix();
	glTranslatef( -2, -1.55, -4 );
	//glRotatef(180,0,1,0);
	glLoadName(2);

	if(TriceratopsGooch == 1 && mode != GL_SELECT)
	{
		GoochProg->EnableShader();
		GLuint Uniforms;
		Uniforms=glGetUniformLocation(GoochProg->GetProgramID(),"LightPos");
		Point P = scene->GetLight(0)->GetCurrentPos();
		glUniform3f(Uniforms,P.X(),P.Y(),P.Z());
	}
	glCallList(TriceratopsList);
	if(TriceratopsGooch == 1 && mode != GL_SELECT)	GoochProg->DisableShader();

	glPopMatrix();

	//Draw Door***********************************************************
	glColor3f(.545,.271,.0745);
	glPushMatrix();
	glTranslatef( -6, -.5, 3.5 );
	glRotatef(100,0,1,0);
	glScalef(1.5,1.5,1.5);
	glLoadName(3);

	if(DoorGooch == 1 && mode != GL_SELECT)
	{
		GoochProg->EnableShader();
		GLuint Uniforms;
		Uniforms=glGetUniformLocation(GoochProg->GetProgramID(),"LightPos");
		Point P = scene->GetLight(0)->GetCurrentPos();
		glUniform3f(Uniforms,P.X(),P.Y(),P.Z());
	}
	glCallList(DoorList);
	if(DoorGooch == 1 && mode != GL_SELECT)	GoochProg->DisableShader();

	glPopMatrix();

	//Draw Fridge***********************************************************
	glColor3f(1,.937,.859);
	glPushMatrix();
	glTranslatef( 5.5, -1, 3.5 );
	glRotatef(180,0,1,0);
	glScalef(1.5,1.5,1.5);
	glLoadName(4);

	if(FridgeGooch == 1 && mode != GL_SELECT)
	{
		GoochProg->EnableShader();
		GLuint Uniforms;
		Uniforms=glGetUniformLocation(GoochProg->GetProgramID(),"LightPos");
		Point P = scene->GetLight(0)->GetCurrentPos();
		glUniform3f(Uniforms,P.X(),P.Y(),P.Z());
	}
	glCallList(FridgeList);
	if(FridgeGooch == 1 && mode != GL_SELECT)	GoochProg->DisableShader();

	glPopMatrix();

	//Draw SinkStand***********************************************************
	glColor3f(1,.937,.859);
	glPushMatrix();
	glTranslatef( -5.5, -1.2, -3.5 );
	//glRotatef(180,0,1,0);
	//glScalef(1.5,1.5,1.5);
	glLoadName(5);

	if(SinkStandGooch == 1 && mode != GL_SELECT)
	{
		GoochProg->EnableShader();
		GLuint Uniforms;
		Uniforms=glGetUniformLocation(GoochProg->GetProgramID(),"LightPos");
		Point P = scene->GetLight(0)->GetCurrentPos();
		glUniform3f(Uniforms,P.X(),P.Y(),P.Z());
	}
	glCallList(SinkStandList);
	if(SinkStandGooch == 1 && mode != GL_SELECT)	GoochProg->DisableShader();

	glPopMatrix();

	//Draw Sink***********************************************************
	glColor3f(.5,.5,.5);
	glPushMatrix();
	glTranslatef( -5.5, -.4, -3.5 );
	glRotatef(90,0,1,0);
	glScalef(1,.8,.8);
	//glLoadName(5);

	if(SinkStandGooch == 1 && mode != GL_SELECT)
	{
		GoochProg->EnableShader();
		GLuint Uniforms;
		Uniforms=glGetUniformLocation(GoochProg->GetProgramID(),"LightPos");
		Point P = scene->GetLight(0)->GetCurrentPos();
		glUniform3f(Uniforms,P.X(),P.Y(),P.Z());
	}
	glCallList(SinkList);
	if(SinkStandGooch == 1 && mode != GL_SELECT)	GoochProg->DisableShader();

	glPopMatrix();

	//Draw Table***********************************************************
	glColor3f(.627,.322,.176);
	glPushMatrix();
	glTranslatef( 2, -1.2, -2 );
	//glRotatef(90,0,1,0);
	glScalef(1,.8,1);
	glLoadName(6);

	if(TableGooch == 1 && mode != GL_SELECT)
	{
		GoochProg->EnableShader();
		GLuint Uniforms;
		Uniforms=glGetUniformLocation(GoochProg->GetProgramID(),"LightPos");
		Point P = scene->GetLight(0)->GetCurrentPos();
		glUniform3f(Uniforms,P.X(),P.Y(),P.Z());
	}
	glCallList(TableList);
	if(TableGooch == 1 && mode != GL_SELECT)	GoochProg->DisableShader();

	glPopMatrix();

	//Draw Chairs***********************************************************
	glColor3f(.698,.133,.133);
	glPushMatrix();
	glTranslatef( 3.8, -1, -2.8 );
	glRotatef(-60,0,1,0);
	//glScalef(1,.8,1);
	glLoadName(7);

	if(Chair1Gooch == 1 && mode != GL_SELECT)
	{
		GoochProg->EnableShader();
		GLuint Uniforms;
		Uniforms=glGetUniformLocation(GoochProg->GetProgramID(),"LightPos");
		Point P = scene->GetLight(0)->GetCurrentPos();
		glUniform3f(Uniforms,P.X(),P.Y(),P.Z());
	}
	glCallList(ChairList);
	if(Chair1Gooch == 1 && mode != GL_SELECT)	GoochProg->DisableShader();

	glPopMatrix();


	glColor3f(.698,.133,.133);
	glPushMatrix();
	glTranslatef( 1.4, -1, -3.2 );
	glRotatef(30,0,1,0);
	//glScalef(1,.8,1);
	glLoadName(11);

	if(Chair2Gooch == 1 && mode != GL_SELECT)
	{
		GoochProg->EnableShader();
		GLuint Uniforms;
		Uniforms=glGetUniformLocation(GoochProg->GetProgramID(),"LightPos");
		Point P = scene->GetLight(0)->GetCurrentPos();
		glUniform3f(Uniforms,P.X(),P.Y(),P.Z());
	}
	glCallList(ChairList);
	if(Chair2Gooch == 1 && mode != GL_SELECT)	GoochProg->DisableShader();

	glPopMatrix();


	glColor3f(.698,.133,.133);
	glPushMatrix();
	glTranslatef( 2.2, -1, -.6 );
	glRotatef(180,0,1,0);
	//glScalef(1,.8,1);
	glLoadName(12);

	if(Chair3Gooch == 1 && mode != GL_SELECT)
	{
		GoochProg->EnableShader();
		GLuint Uniforms;
		Uniforms=glGetUniformLocation(GoochProg->GetProgramID(),"LightPos");
		Point P = scene->GetLight(0)->GetCurrentPos();
		glUniform3f(Uniforms,P.X(),P.Y(),P.Z());
	}
	glCallList(ChairList);
	if(Chair3Gooch == 1 && mode != GL_SELECT)	GoochProg->DisableShader();

	glPopMatrix();


	//Draw Sideboard1***********************************************************
	glColor3f(.804,.804,.757);
	glPushMatrix();
	glTranslatef( -5.5, -1.4, -1.5 );
	glRotatef(90,0,1,0);
	//glScalef(1,.8,1);
	glLoadName(8);

	if(Sideboard1a2Gooch == 1 && mode != GL_SELECT)
	{
		GoochProg->EnableShader();
		GLuint Uniforms;
		Uniforms=glGetUniformLocation(GoochProg->GetProgramID(),"LightPos");
		Point P = scene->GetLight(0)->GetCurrentPos();
		glUniform3f(Uniforms,P.X(),P.Y(),P.Z());
	}
	glCallList(SideboardList);
	if(Sideboard1a2Gooch == 1 && mode != GL_SELECT)	GoochProg->DisableShader();

	glPopMatrix();

	//Draw Sideboard2***********************************************************
	glColor3f(.804,.804,.757);
	glPushMatrix();
	glTranslatef( -5.5, -1.4, .4 );
	glRotatef(90,0,1,0);
	//glScalef(1,.8,1);
	//glLoadName(8);

	if(Sideboard1a2Gooch == 1 && mode != GL_SELECT)
	{
		GoochProg->EnableShader();
		GLuint Uniforms;
		Uniforms=glGetUniformLocation(GoochProg->GetProgramID(),"LightPos");
		Point P = scene->GetLight(0)->GetCurrentPos();
		glUniform3f(Uniforms,P.X(),P.Y(),P.Z());
	}
	glCallList(SideboardList);
	if(Sideboard1a2Gooch == 1 && mode != GL_SELECT)	GoochProg->DisableShader();

	glPopMatrix();

	//Draw Sideboard3***********************************************************
	glColor3f(.804,.804,.757);
	glPushMatrix();
	glTranslatef( 5.5, -1.4, 1.8 );
	glRotatef(-90,0,1,0);
	//glScalef(1,.8,1);
	glLoadName(9);

	if(Sideboard3a4Gooch == 1 && mode != GL_SELECT)
	{
		GoochProg->EnableShader();
		GLuint Uniforms;
		Uniforms=glGetUniformLocation(GoochProg->GetProgramID(),"LightPos");
		Point P = scene->GetLight(0)->GetCurrentPos();
		glUniform3f(Uniforms,P.X(),P.Y(),P.Z());
	}
	glCallList(SideboardList);
	if(Sideboard3a4Gooch == 1 && mode != GL_SELECT)	GoochProg->DisableShader();

	glPopMatrix();

	//Draw Sideboard4***********************************************************
	glColor3f(.804,.804,.757);
	glPushMatrix();
	glTranslatef( 5.5, -1.4, 0 );
	glRotatef(-90,0,1,0);
	//glScalef(1,.8,1);
	//glLoadName(9);

	if(Sideboard3a4Gooch == 1 && mode != GL_SELECT)
	{
		GoochProg->EnableShader();
		GLuint Uniforms;
		Uniforms=glGetUniformLocation(GoochProg->GetProgramID(),"LightPos");
		Point P = scene->GetLight(0)->GetCurrentPos();
		glUniform3f(Uniforms,P.X(),P.Y(),P.Z());
	}
	glCallList(SideboardList);
	if(Sideboard3a4Gooch == 1 && mode != GL_SELECT)	GoochProg->DisableShader();

	glPopMatrix();

	//Draw Microwave***********************************************************
	glColor3f(.5,.5,.5);
	glPushMatrix();
	glTranslatef( 5.5, -.2, 2 );
	glRotatef(-90,0,1,0);
	glScalef(.7,.7,.7);
	glLoadName(10);

	if(MicrowaveGooch == 1 && mode != GL_SELECT)
	{
		GoochProg->EnableShader();
		GLuint Uniforms;
		Uniforms=glGetUniformLocation(GoochProg->GetProgramID(),"LightPos");
		Point P = scene->GetLight(0)->GetCurrentPos();
		glUniform3f(Uniforms,P.X(),P.Y(),P.Z());
	}
	glCallList(MicrowaveList);
	if(MicrowaveGooch == 1 && mode != GL_SELECT)	GoochProg->DisableShader();

	glPopMatrix();

	//Draw Cabinets***********************************************************
	glColor3f(.804,.804,.757);
	glPushMatrix();
	glTranslatef( -5.5, 1.5, -1.5 );
	//glRotatef(-90,0,1,0);
	//glScalef(2,2,2);
	glLoadName(13);

	if(Cabinet1Gooch == 1 && mode != GL_SELECT)
	{
		GoochProg->EnableShader();
		GLuint Uniforms;
		Uniforms=glGetUniformLocation(GoochProg->GetProgramID(),"LightPos");
		Point P = scene->GetLight(0)->GetCurrentPos();
		glUniform3f(Uniforms,P.X(),P.Y(),P.Z());
	}
	glCallList(CabinetList);
	if(Cabinet1Gooch == 1 && mode != GL_SELECT)	GoochProg->DisableShader();

	glPopMatrix();

	glColor3f(.804,.804,.757);
	glPushMatrix();
	glTranslatef( -5.5, 1.5, .5 );
	//glRotatef(-90,0,1,0);
	//glScalef(2,2,2);
	//glLoadName(13);

	if(Cabinet1Gooch == 1 && mode != GL_SELECT)
	{
		GoochProg->EnableShader();
		GLuint Uniforms;
		Uniforms=glGetUniformLocation(GoochProg->GetProgramID(),"LightPos");
		Point P = scene->GetLight(0)->GetCurrentPos();
		glUniform3f(Uniforms,P.X(),P.Y(),P.Z());
	}
	glCallList(CabinetList);
	if(Cabinet1Gooch == 1 && mode != GL_SELECT)	GoochProg->DisableShader();

	glPopMatrix();


	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);
}

void ProcessHits(GLint hits,GLuint selectBuff[])
{
	GLuint *ptr = selectBuff;

	for(int i=0;i<hits;i++)
	{
		ptr++;
		ptr++;
		ptr++;

		//Check what is selected
		switch(*ptr)
		{
			case 1:
				TeapotGooch = 1 - TeapotGooch;
				break;
			case 2:
				TriceratopsGooch = 1 - TriceratopsGooch;
				break;
			case 3:
				DoorGooch = 1 - DoorGooch;
				break;
			case 4:
				FridgeGooch = 1 - FridgeGooch;
				break;
			case 5:
				SinkStandGooch = 1 - SinkStandGooch;
				break;
			case 6:
				TableGooch = 1 - TableGooch;
				break;
			case 7:
				Chair1Gooch = 1 - Chair1Gooch;
				break;
			case 8:
				Sideboard1a2Gooch = 1 - Sideboard1a2Gooch;
				break;
			case 9:
				//Sideboard3a4Gooch = 1 - Sideboard3a4Gooch;
				break;
			case 10:
				MicrowaveGooch = 1 - MicrowaveGooch;
				break;
			case 11:
				Chair2Gooch = 1 - Chair2Gooch;
				break;
			case 12:
				Chair3Gooch = 1 - Chair3Gooch;
				break;
			case 13:
				Cabinet1Gooch = 1 - Cabinet1Gooch;
				break;
			default:
				break;
		}

		ptr++;
	}
}
