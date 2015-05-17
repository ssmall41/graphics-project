/***************************************************************************/
/* glInterface.cpp                                                         */
/* -----------------------                                                 */
/*                                                                         */
/* Basic GLUT callbacks for the sceneLoader OpenGL program.                */
/*                                                                         */
/* Chris Wyman (02/01/2008)                                                */
/***************************************************************************/

#include "sceneLoader.h"
#include "RenderingTechniques/renderingData.h"
#include "Scene/Scene.h"
#include "Interface/SceneFileDefinedInteraction.h"

extern Trackball *eyeBall;
extern Trackball *lightBall[4];
extern Trackball *objBall[4];

extern Scene *scene;
extern SceneDefinedUI *ui;
extern RenderingData *data;
double currentTime = 0;



// Updates to the scene based on an animation?
void IdleCallback ( void )
{
	if (data->param->lightFOV)
		scene->GetLight( 0 )->SetFOV( *data->param->lightFOV );
	scene->PerFrameUpdate( (float)currentTime );
	glutPostRedisplay();
}


// This is where most changes occurring from UI events occur.  This uses
//   UI definitions from a scene file, as specified by the SceneDefinedUI
//   class, which allows a file to map keys to functionality directly.
// The mouse, keyboard, special_keys callbacks below all call this function.
bool ApplyUICommand( unsigned int curCommand, int xVal, int yVal )
{
	if ( curCommand == KEY_UNKNOWN ) return false;
	else if ( curCommand == ui->Key( UI_RELOAD_SHADERS ) )
	{
		printf("(+) Reloading Shaders....");
		bool ok = scene->ReloadShaders();
		printf( ok ? "Successful!\n" : "Errors encountered!" );
		glutPostRedisplay();
		return true;
	}
	else if ( curCommand == ui->Key( UI_SCREEN_CAPTURE ) )
	{
		data->ui->captureScreen = true;
		glutPostRedisplay();
		return true;
	}
	else if ( curCommand == ui->Key( UI_QUIT ) )
	{
		exit(0);
		return true;
	}
	
	return false;
}


void ReshapeCallback ( int w, int h )
{
	int newWidth = w, newHeight = h;

	// Double check that we're willing to allow the screen resolution to change...
	
	//       (Yes, I guess...)

	// Setup the new projection matrix and viewport
	glViewport( 0, 0, newWidth, newHeight );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	scene->PerspectiveMatrix();
	glMatrixMode( GL_MODELVIEW );
	glutPostRedisplay();

	// Resize the trackballs, if necessary.
	if (eyeBall) eyeBall->ResizeTrackballWindow( newWidth, newHeight );
	for (int i=0; i<4; i++)
	{
		if (lightBall[i]) lightBall[i]->ResizeTrackballWindow( newWidth, newHeight );
		if (objBall[i])   objBall[i]->ResizeTrackballWindow( newWidth, newHeight );
	}
}

