/***************************************************************************/
/* sceneLoader.h                                                           */
/* -----------------------                                                 */
/*                                                                         */
/* The header file for using the basic scene loader in OpenGL programs.    */
/*     This scene loader will parse the exact same scene files used in my  */
/*     basic ray tracing framework, allowing direct comparisons using the  */
/*     exact same scene functions.                                         */
/*                                                                         */
/* Chris Wyman (02/01/2008)                                                */
/***************************************************************************/

#ifndef __SCENELOADER_H__
#define __SCENELOADER_H__

#define HAVE_SHADER_MODEL_4    1


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "DataTypes/Array1D.h"
#include "DataTypes/Color.h"
#include "DataTypes/MathDefs.h"
#include "DataTypes/Matrix4x4.h"
#include "DataTypes/Point.h"
#include "DataTypes/Vector.h"
#include "DataTypes/glTexture.h"

#include "Materials/Material.h"
#include "Materials/GLMaterial.h"

#include "Objects/Group.h"
#include "Objects/Object.h"
#include "Objects/Triangle.h"
#include "Objects/Sphere.h"
#include "Objects/Cylinder.h"
#include "Objects/Quad.h"
#include "Objects/Mesh.h"

#include "Utils/drawTextToGLWindow.h"
#include "Utils/frameRate.h"
#include "Utils/ProgramPathLists.h"
#include "Utils/searchPathList.h"
#include "Utils/TextParsing.h"
#include "Utils/Trackball.h"
#include "Utils/ImageIO/imageIO.h"
#include "Utils/framebufferObject.h"
#include "Utils/frameGrab.h"
#include "Utils/ModelIO/glm.h"

#include "Scene/Camera.h"
#include "Scene/glLight.h"
#include "Scene/Scene.h"

// GLUT callback functions
void ReshapeCallback ( int w, int h );
void IdleCallback ( void );
void MouseMotionCallback ( int x, int y );
void MouseButtonCallback ( int b, int st, int x, int y );
void KeyboardCallback ( unsigned char key, int x, int y );
void SpecialKeyboardCallback ( int key, int x, int y );

//Reflections
void InitMirror();
void Draw1stReflectedSceneToBuffer(int lookback,FrameBuffer *fbo);
void Draw2ndReflectedSceneToBuffer(int lookback,FrameBuffer *fbo);
void DrawFloorReflectedSceneToBuffer();
void DrawFloorMirror();
void DrawFirstMirror(int bounced);
void DrawSecondMirror(int bounced);

//Silhouette
void LoadSilModel();
void DrawSilModel();

//Gooch Model
void LoadGoochShader();

//Selection
void DrawSelectionObjects(GLenum mode);
void ProcessHits(GLint hits,GLuint selectBuff[]);
void LoadSelectionModels();

//Extras
void DrawExtraWalls();
void InitExtraWalls();
void DrawSceneStuff();
void DrawFan();

#ifndef BUFFER_OFFSET 
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#endif

#endif



