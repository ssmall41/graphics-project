#include "sceneLoader.h"

int Bricktexwidth,Bricktexheight;
unsigned char* BrickTexPattern;
GLuint BricktextureID;

int Stonetexwidth,Stonetexheight;
unsigned char* StoneTexPattern;
GLuint StonetextureID;

extern double rotate;
extern Scene *scene;

void DrawExtraWalls()
{
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D,BricktextureID);
	glBegin(GL_QUADS);
		glMultiTexCoord2f(GL_TEXTURE0,0,0);
		glVertex3f(-6,-2,-6);
		glMultiTexCoord2f(GL_TEXTURE0,2,0);
		glVertex3f(6,-2,-6);
		glMultiTexCoord2f(GL_TEXTURE0,2,3);
		glVertex3f(6,6,-6);
		glMultiTexCoord2f(GL_TEXTURE0,0,3);
		glVertex3f(-6,6,-6);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,StonetextureID);
	glBegin(GL_QUADS);
		glMultiTexCoord2f(GL_TEXTURE0,0,0);
		glVertex3f(6,-2,6);
		glMultiTexCoord2f(GL_TEXTURE0,5,0);
		glVertex3f(-6,-2,6);
		glMultiTexCoord2f(GL_TEXTURE0,5,3);
		glVertex3f(-6,6,6);
		glMultiTexCoord2f(GL_TEXTURE0,0,3);
		glVertex3f(6,6,6);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_2D);
}


void InitExtraWalls()
{
	int* mode = new int;
	BrickTexPattern = ReadPPM("./textures/bigBrickWall.ppm",mode,&Bricktexwidth,&Bricktexheight);
	glGenTextures(1,&BricktextureID);
	glBindTexture(GL_TEXTURE_2D,BricktextureID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,Bricktexwidth,Bricktexheight,0,GL_RGB,GL_UNSIGNED_BYTE,BrickTexPattern);

	StoneTexPattern = ReadPPM("./textures/stoneFloor.ppm",mode,&Stonetexwidth,&Stonetexheight);
	glGenTextures(1,&StonetextureID);
	glBindTexture(GL_TEXTURE_2D,StonetextureID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,Stonetexwidth,Stonetexheight,0,GL_RGB,GL_UNSIGNED_BYTE,StoneTexPattern);

	glBindTexture(GL_TEXTURE_2D,0);
}


void DrawFan()
{
	glPushMatrix();

	glTranslatef(0,2.3,0);
	glRotatef(-90,1,0,0);
	glScalef(.6,.6,.6);
	glRotatef(rotate,0,0,1);

	glEnable(GL_COLOR_MATERIAL);
	//Draw Tube
	glColor3f(1,.843,0);
	GLUquadric* quadric = gluNewQuadric();
	gluCylinder(quadric,.15,.15,1,30,30);
     
	//Draw Cap
	glPushMatrix();
	glRotatef(180,1,0,0);
	gluCylinder(quadric,.26,0,.25,20,20);
	glPopMatrix();

	//Draw Blades
	glPushMatrix();
	glColor3f(.545,.271,.0745);
	glBegin(GL_QUADS);
	 glVertex3f(0,.25,0);
	 glVertex3f(1,1.50,0);
	 glVertex3f(2,2,0);
	 glVertex3f(.25,0,0);
	glEnd();

	glRotatef(90,0,0,1);
	glBegin(GL_QUADS);
	 glVertex3f(0,.25,0);
	 glVertex3f(1,1.50,0);
	 glVertex3f(2,2,0);
	 glVertex3f(.25,0,0);
	glEnd();

	glRotatef(90,0,0,1);
	glBegin(GL_QUADS);
	 glVertex3f(0,.25,0);
	 glVertex3f(1,1.50,0);
	 glVertex3f(2,2,0);
	 glVertex3f(.25,0,0);
	glEnd();

	glRotatef(90,0,0,1);
	glBegin(GL_QUADS);
	 glVertex3f(0,.25,0);
	 glVertex3f(1,1.50,0);
	 glVertex3f(2,2,0);
	 glVertex3f(.25,0,0);
	glEnd();
	glPopMatrix();

	glDisable(GL_COLOR_MATERIAL);
	glPopMatrix();
}

//Draws everything but the mirrors
void DrawSceneStuff()
{
	DrawFan();
	scene->Draw();
	DrawSilModel();
	DrawExtraWalls();
	DrawSelectionObjects(GL_RENDER);
}
