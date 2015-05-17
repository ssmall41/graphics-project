/***************************************************************************/
/* utilRoutines.cpp                                                        */
/* -----------------------                                                 */
/*                                                                         */
/* These are utility routines that may (or may not) be reused in multiple  */
/*    techniques in the various files in this directory.  However, these   */
/*    functions are not particularly relevant to the renderings, but       */
/*    instead encapsulate annoying OpenGL code that would otherwise make   */
/*    the rendering code difficult to read.                                */
/*                                                                         */
/* Chris Wyman (02/23/2008)                                                */
/***************************************************************************/

#include "sceneLoader.h"
#include "renderingData.h"

extern Scene *scene;
extern RenderingData *data;


float sqr( float f )
{
	return f*f;
}

float log2( float x )
{
	return log(x)/log(2.0);
}


