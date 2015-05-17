/***************************************************************************/
/* initializeRenderingData.cpp                                             */
/* -----------------------                                                 */
/*                                                                         */
/* Initialization routines for data needed for all the rendering methods   */
/*    in this directory.                                                   */
/*                                                                         */
/* Chris Wyman (02/23/2008)                                                */
/***************************************************************************/


#include "sceneLoader.h"
#include "renderingData.h"
#include "renderingMethods.h"

extern Scene *scene;
RenderingData *data = 0;

void InitializeRenderingData( void )
{
	// Check to make sure we only initialize once.
	if (data) return;

	// Allocate global data structures
	data = new RenderingData();
	data->fbo    = new FrameBufferData();
	data->param  = new ParameterData();
	data->shader = new ShaderData();
	data->tex    = new TextureData();
	data->vbo    = new VertexBufferData();
	data->glID   = new OtherGLIDData();
	data->ui     = new UIData();

	// Allocate the framebuffers
	data->fbo->mainWin = new FrameBuffer( GL_TEXTURE_2D, scene->GetWidth(), scene->GetHeight(), -1, 
										  GL_RGBA, 1, 1, 0, "Main Render Window" );
	data->fbo->mainWin->CheckFramebufferStatus( 1 );

	data->fbo->shadowMap = new FrameBuffer( GL_TEXTURE_2D, scene->GetWidth(), scene->GetHeight(), -1, 
											GL_RGBA, 1, 1, 0, "Shadow Map" );
	data->fbo->shadowMap->CheckFramebufferStatus( 1 );

	// Allocate various parameters
	data->param->usingShadows = scene->GetSceneBoolVar( "shadowsenabled" );
	if (!data->param->usingShadows) 
		data->param->usingShadows = new bool( false );

	data->param->shadowMapBias = scene->GetSceneFloatVar( "shadowmapbias" );
	if (!data->param->shadowMapBias)
		data->param->shadowMapBias = new float( -0.005 );

	data->param->lightFOV = scene->GetSceneFloatVar( "lightfov" );


	// Initialize starting UI information
	data->ui->captureScreen = false;
}



