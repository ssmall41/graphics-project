/******************************************************************/
/* SceneRenderFuncs.cpp                                           */
/* -----------------------                                        */
/*                                                                */
/* The file defines part of a scene class that encapsulates the   */
/*     scene information.  In particular, this file includes      */
/*     common rendering commands that might be used either by     */
/*     applications or other scene methods.                       */
/*                                                                */
/* Chris Wyman (02/07/2008)                                       */
/******************************************************************/

#include "Utils/ImageIO/imageIO.h"
#include "DataTypes/Color.h"
#include "DataTypes/glTexture.h"
#include "DataTypes/Matrix4x4.h"
#include "Scene/Scene.h"
#include "Scene/Camera.h"
#include "Scene/glLight.h"
#include "Objects/Group.h"
#include "Objects/Triangle.h"
#include "Objects/Sphere.h"
#include "Objects/Quad.h"
#include "Objects/Cylinder.h"
#include "Objects/Mesh.h"
#include "Materials/Material.h"
#include "Materials/GLMaterial.h"
#include "Materials/GLLambertianMaterial.h"
#include "Materials/GLLambertianTexMaterial.h"
#include "Materials/GLConstantMaterial.h"
#include "Materials/GLSLShaderMaterial.h"
#include "Utils/ProgramPathLists.h"
#include "Interface/SceneFileDefinedInteraction.h"
#include "Utils/Trackball.h"
#include "Utils/framebufferObject.h"


// This multiples onto the current matrix stack the perspective matrix
//   used by light #i.  Since lights do not have "aspect ratios", you need
//   to specify one for the image you are rendering to.
void Scene::LightPerspectiveMatrix( int i, float aspect )
{
	gluPerspective( light[i]->GetLightFovy(), aspect, 
					light[i]->GetLightNearPlane(), light[i]->GetLightFarPlane() );
}

// This multiplies onto the current matrix stack a look-at matrix (using
//   gluLookAt()) for light #i.
void Scene::LightLookAtMatrix( int i )
{
	Point lightPos( light[i]->GetCurrentPos() );
	Point at( camera->GetAt() );
	Vector view = at-lightPos;
	view.Normalize();
	Vector perp = fabs( view.Dot( Vector::YAxis() ) ) < 0.95 ? view.Cross( Vector::YAxis() ) : view.Cross( Vector::XAxis() );
	perp.Normalize();
	Vector up = perp.Cross( view );
	gluLookAt( lightPos.X(), lightPos.Y(), lightPos.Z(), 
		       at.X(), at.Y(), at.Z(), 
			   up.X(), up.Y(), up.Z() );
}

// This multiplies onto the current matrix stack an inverse look-at matrix
//    (equivalent to Invert( gluLookAt() ) -- if such an operation were possible)
void Scene::LightLookAtInverseMatrix( int i )
{
	Point lightPos( light[i]->GetCurrentPos() );
	Vector view = camera->GetAt()-lightPos;
	view.Normalize();
	Vector perp = fabs( view.Dot( Vector::YAxis() ) ) < 0.95 ? view.Cross( Vector::YAxis() ) : view.Cross( Vector::XAxis() );
	perp.Normalize();
	Vector up = perp.Cross( view );
	Matrix4x4 mat = Matrix4x4::LookAt( lightPos, camera->GetAt(), up ).Invert();
	glMultMatrixf( mat.GetDataPtr() );
}

// This creates a shadow map.  The rendering pass is already done for you, but you need to
//    fill in the matrix creation code (though all the subparts are available in the framework)
void Scene::CreateShadowMap( FrameBuffer *shadMapBuf, float *shadMapMatrixTranspose, int lightNum, float shadowMapBias )
{
	// Go ahead and render the shadow map
	glPushAttrib( GL_COLOR_BUFFER_BIT );
	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	shadMapBuf->BindBuffer();
	shadMapBuf->ClearBuffers();

	// Set up the projection matrix for the shadow map
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	LightPerspectiveMatrix( lightNum, shadMapBuf->GetAspectRatio() );

	// Set up the modelview ( i.e., light gluLookAt() )
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
		LightLookAtMatrix( lightNum );
		this->Draw( MATL_FLAGS_NONE, OBJECT_OPTION_NONE, true );
	glPopMatrix();

	// Go ahead and pop off the new projection and modelview matrices
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	shadMapBuf->UnbindBuffer();
	glPopAttrib();

	// Now compute the shadow map matrix for use by the current eye/camera view
	glPushMatrix();
	glLoadIdentity();

	// Put here the commands to create the shadow map matrix.  Note:  The 
	//    Scene and Camera classes give you access to functions that multiply
	//    bits of the shadow map matrix onto the current matrix stack.  You
	//    simply need to discover what they are and apply them in the right 
	//    order here.  The scale and bias matrix needs to be done yourself, 
	//    however (and you should try to use the shadowMapBias) parameter.
	
	glGetFloatv(GL_TRANSPOSE_MODELVIEW_MATRIX, shadMapMatrixTranspose);  // Copy the transpose matrix
	glGetFloatv(GL_TRANSPOSE_MODELVIEW_MATRIX, shadMapTransMatrix);      // Also copy to the scene's internal storage
	glPopMatrix(); 

	// Make sure the Scene class knows where the shadow map is stored.
	shadowMapTexID = shadMapBuf->GetDepthTextureID();
}


