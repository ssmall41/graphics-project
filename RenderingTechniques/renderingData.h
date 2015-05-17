/***************************************************************************/
/* renderingData.h                                                         */
/* -----------------------                                                 */
/*                                                                         */
/* Stores a "class" that encapsulates global data used by the various      */
/*     rendering techniques described in this directory.  The internals of */
/*     this class are initialized by calling InitializeRenderingData()     */
/*     defined in initializeRenderingData.cpp.                             */
/*                                                                         */
/* Chris Wyman (02/23/2008)                                                */
/***************************************************************************/

#ifndef __RENDERING_DATA_H__
#define __REDNERING_DATA_H__

class FrameBuffer;
class GLSLProgram;
class GLTexture;
class FrameBufferData;
class VertexBufferData;
class TextureData;
class ShaderData;
class ParameterData;
class OtherGLIDData;
class UIData;

// Class encapsulating global rendering data
class RenderingData
{
public:
	FrameBufferData  *fbo;
	ShaderData       *shader;
	ParameterData    *param;
	TextureData      *tex;
	VertexBufferData *vbo;
	OtherGLIDData    *glID;
	UIData           *ui;
};



class FrameBufferData
{
public:
	// Framebuffers for simple rendering with shadow maps
	FrameBuffer *shadowMap, *mainWin;
};


class ShaderData
{
public:
	// Shaders you need for your rendering algorithms should go here, e.g.,
	// GLSLProgram *refract;
};


class ParameterData
{
public:
	// Parameters for simple rendering with shadow maps
	float shadowMatrixTrans[16];
	bool *usingShadows;
	float *shadowMapBias;

	// The light's field of view (useful for shadow map)
	float *lightFOV;
};

class TextureData
{
public:
	// You may want to define textures of your own here for various rendering
	//   algorithms.  Properties of the scene should probably be defined in the
	//   scene file.  Some examples:
	// GLTexture *noiseTexture, *gameOfLifeStart;
	;
};

class VertexBufferData
{
public:
	// If you have VBOs for things other than geometry, put them here.  You 
	//   probably won't need anything here.
	;
};

class OtherGLIDData
{
public:
	// There's a bunch of other IDs OpenGL needs sometimes.  You probably won't need
	//   this, but if it's not a texture ID, a framebuffer ID, or a vertexbuffer ID
	//   it should probably go here
	;
};

class UIData
{
public:
	// You may have a bunch of UI commands and variables that may be modified.  They
	//   might go here.
	bool captureScreen;
};



#endif

