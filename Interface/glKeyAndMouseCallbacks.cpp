/***************************************************************************/
/* glKeyAndMouseCallbacks.cpp                                              */
/* -----------------------                                                 */
/*                                                                         */
/* Defines the basic OpenGL/GLUT keyboard and mouse callbacks.  However,   */
/*    since this program allows the file to directly map UI commands to    */
/*    constants, these really shouldn't need frequent changes.  Instead,   */
/*    the calls to ApplyUICommand() defined in glInterface should take     */
/*    care of the heavy lifting and state changes.                         */
/*                                                                         */
/* Chris Wyman (02/01/2008)                                                */
/***************************************************************************/

#include "sceneLoader.h"
#include "Scene/Scene.h"
#include "Interface/SceneFileDefinedInteraction.h"
#include "DataTypes/Array1D.h"
#include <climits>

#define VAR_TYPE_FLOAT   0
#define VAR_TYPE_BOOL    1
#define VAR_TYPE_INT     2

extern int bounces;
extern bool grayscale;
extern Scene *scene;
extern float reflectiveness;
double rotation_angle=3.141592653589;

typedef struct _incDecStruct {
	int keyStroke;
	int type;
	int iLimit, iDelta;
	float fLimit, fDelta;
	void *dataPtr;
} incDecStruct;
Array1D< incDecStruct * > incrDecrKeys;


extern Scene *scene;
extern SceneDefinedUI *ui;
int trackballInUse = KEY_UNKNOWN, modifiers=0;
int lastMouseX=INT_MAX, lastMouseY=INT_MAX, lastButton=-1;



bool ApplyUICommand( unsigned int curCommand, int xVal, int yVal );
bool CheckBoundIncrDecr( unsigned int key );

// These functions search for the correct trackball from above, based
//    on the UI command 'curCommand' and update if appropriately.
//    They are defined in Interface/glTrackballInterface.cpp
void InitializeTrackball( int curCommand, int xVal, int yVal );
void UpdateTrackball( int curCommand, int xVal, int yVal );
void ReleaseTrackball( int curCommand );


void MouseMotionCallback ( int x, int y )
{
	if (lastMouseX == INT_MAX && lastMouseY == INT_MAX) return;

	if (trackballInUse != KEY_UNKNOWN)
		UpdateTrackball( trackballInUse, x, y );
	else // There wasn't a trackball, but maybe there was some other UI functions
	{
		int mouseX = ui->ConvertGLUTMouseX( lastButton, x-lastMouseX, modifiers );
		int mouseY = ui->ConvertGLUTMouseY( lastButton, y-lastMouseY, modifiers );

		ApplyUICommand( mouseX, abs(x-lastMouseX), abs(y-lastMouseY) );
		ApplyUICommand( mouseY, abs(x-lastMouseX), abs(y-lastMouseY) );
	}

	lastMouseX = x;
	lastMouseY = y;
}


void MouseButtonCallback ( int b, int st, int x, int y )
{
	if( b == GLUT_LEFT_BUTTON && st == GLUT_DOWN )
	{
		GLuint selectBuff[512];
		GLint viewport[4];
		GLint hits;

		glGetIntegerv(GL_VIEWPORT,viewport);
		glSelectBuffer(512,selectBuff);
		glRenderMode(GL_SELECT);

		glInitNames();
		glPushName(0);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		gluPickMatrix((GLdouble) x, (GLdouble) (viewport[3]-y), 5.0, 5.0, viewport);
		scene->PerspectiveMatrix();
		glMatrixMode(GL_MODELVIEW);
	
		DrawSelectionObjects(GL_SELECT);
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glFlush();

		hits = glRenderMode(GL_RENDER);
		ProcessHits(hits,selectBuff);
	}


	if ( st == GLUT_DOWN )
	{
		lastButton = b;
		lastMouseX = x;
		lastMouseY = y;
		modifiers = glutGetModifiers();
		trackballInUse = ui->ConvertGLUTTrackball( lastButton, modifiers );
		if (trackballInUse) InitializeTrackball( trackballInUse, x, y );
	}
	else
	{
		if (trackballInUse) ReleaseTrackball( trackballInUse );
		lastMouseX = lastMouseY = INT_MAX;
		lastButton = -1;
		modifiers = 0;
		trackballInUse = KEY_UNKNOWN;
	}
}


void KeyboardCallback ( unsigned char key, int x, int y )
{
	// These lines first check to see if the keystroke matches any of the keys
	//    whose behavior was defined in the scene file.  If such a definition
	//    exists, it performs the behavior and returns.
	int curKey = ui->ConvertGLUTKey( key );
	if (!CheckBoundIncrDecr( curKey ))
		if (ApplyUICommand( curKey, x, y )) return;
	
	// If the keystroke is not defined in the scene file, you can put standard
	//    OpenGL/GLUT keyboard callback stuff here....
	switch (key) 
	{
    default:
        break;
	case ',':
	case '<':
		if(reflectiveness>0) reflectiveness -= .01;
		break;
	case '.':
	case '>':
		if(reflectiveness<1) reflectiveness += .01;
		break;
	case '-':
	case '_':
		if(bounces > 1)		bounces--;
		break;
	case '+':
	case '=':
		bounces++;
		break;
	case 'g':
	case 'G':
		grayscale = 1 - grayscale;
		break;
	case 'H':
	case 'h':
		printf("The 'h' key behavior is defined in Interface/glKeyAndMouseCallbacks.cpp\n");
		printf("   If you wish to define standard key callbacks (as in your previous GL\n");
		printf("   programs, see the KeyboardCallback() in that file).\n");
		break;
	};
}


void SpecialKeyboardCallback ( int key, int x, int y )
{
	// These lines first check to see if the keystroke matches any of the keys
	//    whose behavior was defined in the scene file.  If such a definition
	//    exists, it performs the behavior and returns.
	unsigned int curKey = ui->ConvertGLUTSpecialKey( key );
	if (!CheckBoundIncrDecr( curKey ))
		if (ApplyUICommand( curKey, x, y )) return;

	// If the keystroke is not defined in the scene file, you can put standard
	//    OpenGL/GLUT keyboard callback stuff here....

	Camera* cam = scene->GetCamera();
	Point Eye = cam->GetEye();
	Point At = cam->GetAt();

	switch (key) 
	{
    default:
        break;
	case GLUT_KEY_UP:
		Eye = Eye + (At-Eye)*(.2/sqrt((At-Eye).Dot(At-Eye)));
		At = At + (At-Eye)*(.2/sqrt((At-Eye).Dot(At-Eye)));

		//Check to make sure we stay in the room
		if(Eye.Z()>-5.0 && Eye.Z()<5.0 && Eye.X()>-5.0 && Eye.X()<5.0)
		{
			cam->SetEye(Eye);
			cam->SetAt(At);
		}
		break;
	case GLUT_KEY_DOWN:
		Eye = Eye - (At-Eye)*(.2/sqrt((At-Eye).Dot(At-Eye)));
		At = At - (At-Eye)*(.2/sqrt((At-Eye).Dot(At-Eye)));

		//Check to make sure we stay in the room
		if(Eye.Z()>-5.0 && Eye.Z()<5.0 && Eye.X()>-5.0 && Eye.X()<5.0)
		{
			cam->SetEye(Eye);
			cam->SetAt(At);
		}
		break;
	case GLUT_KEY_LEFT:
		rotation_angle += .05;
		At = Point(sin(rotation_angle),0,cos(rotation_angle)) + Eye;
		cam->SetAt(At);
		break;
	case GLUT_KEY_RIGHT:
		rotation_angle -= .05;
		At = Point(sin(rotation_angle),0,cos(rotation_angle)) + Eye;
		cam->SetAt(At);
		break;
	};
}


bool CheckBoundIncrDecr( unsigned int key )
{
	for (unsigned int i=0; i < incrDecrKeys.Size(); i++)
	{
		// Check if there's a match.
		if (key != incrDecrKeys[i]->keyStroke) continue;

		// Yep! There's a match
		if (incrDecrKeys[i]->type == VAR_TYPE_BOOL)
			*((bool *)incrDecrKeys[i]->dataPtr) = !*((bool *)incrDecrKeys[i]->dataPtr);
		else if (incrDecrKeys[i]->type == VAR_TYPE_INT)
		{
			*((int *)incrDecrKeys[i]->dataPtr) += incrDecrKeys[i]->iDelta;
			if (incrDecrKeys[i]->iDelta > 0 && *((int *)incrDecrKeys[i]->dataPtr) >= incrDecrKeys[i]->iLimit)
				*((int *)incrDecrKeys[i]->dataPtr) = incrDecrKeys[i]->iLimit;
			if (incrDecrKeys[i]->iDelta < 0 && *((int *)incrDecrKeys[i]->dataPtr) <= incrDecrKeys[i]->iLimit)
				*((int *)incrDecrKeys[i]->dataPtr) = incrDecrKeys[i]->iLimit;
		}
		else if (incrDecrKeys[i]->type == VAR_TYPE_FLOAT)
		{
			*((float *)incrDecrKeys[i]->dataPtr) += incrDecrKeys[i]->fDelta;
			if (incrDecrKeys[i]->fDelta > 0 && *((float *)incrDecrKeys[i]->dataPtr) >= incrDecrKeys[i]->fLimit)
				*((float *)incrDecrKeys[i]->dataPtr) = incrDecrKeys[i]->fLimit;
			if (incrDecrKeys[i]->fDelta < 0 && *((float *)incrDecrKeys[i]->dataPtr) <= incrDecrKeys[i]->fLimit)
				*((float *)incrDecrKeys[i]->dataPtr) = incrDecrKeys[i]->fLimit;
		}
		return true;
	}

	return false;
}


void BindIncrementDecrementKey( char *varName, int key, int by, int limit )
{
	incDecStruct *ids = (incDecStruct *) malloc( sizeof(incDecStruct) );
	ids->keyStroke = key;
	ids->dataPtr = scene->GetSceneIntVar( varName );
	if (ids->dataPtr)
	{
		ids->iDelta = by;
		ids->iLimit = limit;
		ids->type = VAR_TYPE_INT;
		incrDecrKeys.Add( ids );
		return;
	}
	ids->dataPtr = scene->GetSceneFloatVar( varName );
	if (ids->dataPtr)
	{
		ids->fDelta = by;
		ids->fLimit = limit;
		ids->type = VAR_TYPE_FLOAT;
		incrDecrKeys.Add( ids );
		return;
	}
	ids->dataPtr = scene->GetSceneBoolVar( varName );
	if (ids->dataPtr)
	{
		ids->type = VAR_TYPE_BOOL;
		incrDecrKeys.Add( ids );
		return;
	}
}

void BindIncrementDecrementKey( char *varName, int key, float by, float limit )
{
	incDecStruct *ids = (incDecStruct *) malloc( sizeof(incDecStruct) );
	ids->keyStroke = key;
	ids->dataPtr = scene->GetSceneFloatVar( varName );
	if (ids->dataPtr)
	{
		ids->fDelta = by;
		ids->fLimit = limit;
		ids->type = VAR_TYPE_FLOAT;
		incrDecrKeys.Add( ids );
		return;
	}
	ids->dataPtr = scene->GetSceneIntVar( varName );
	if (ids->dataPtr)
	{
	        ids->iDelta = (int)by;
	        ids->iLimit = (int)limit;
		ids->type = VAR_TYPE_INT;
		incrDecrKeys.Add( ids );
		return;
	}
	ids->dataPtr = scene->GetSceneBoolVar( varName );
	if (ids->dataPtr)
	{
		ids->type = VAR_TYPE_BOOL;
		incrDecrKeys.Add( ids );
		return;
	}
}





