/******************************************************************/
/* Scene.h                                                        */
/* -----------------------                                        */
/*                                                                */
/* The file defines a scene class that encapsulates all the       */
/*     information necessary to render a scene in this OpenGL     */
/*     framework.  This allows one to focus on the rendering      */
/*     algorithm instead of the scene/shader/geometry/texture     */
/*     management.                                                */
/*                                                                */
/* Chris Wyman (10/26/2006)                                       */
/******************************************************************/

#ifndef SCENE_H
#define SCENE_H

#include "DataTypes/Color.h"
#include "DataTypes/MathDefs.h"
#include "DataTypes/Array1D.h"
#include "Objects/Group.h"
#include "Objects/Object.h"
#include "Scene/Camera.h"
#include "Scene/glLight.h"
#include "Materials/Material.h"
#include "Utils/ProgramPathLists.h"
#include "Utils/Trackball.h"
#include "Utils/glslProgram.h"


class FrameBuffer;

class Scene {
/****************************************************************************/
/* Functions you are likely to need to access frequently when implementing  */
/*    new rendering techniques...  (Plus the constructor & destructor)      */
/****************************************************************************/
public:  
	// Constructors & destructors
	Scene();										// Default constructor (avoid)
	Scene( char *filename, bool verbose=false );    // Reads a scene from the file
	~Scene();                                       // Free scene memory

	// Access scene (image) width and height
	inline int GetWidth( void ) const			{ return screenWidth; }
	inline int GetHeight( void ) const			{ return screenHeight; }

	// Access the camera (in case you need camera parameters)
	inline Camera *GetCamera( void )            { return camera; }

	// Access the scene lights (either the quantity, or individual lights)
	inline int GetNumLights( void ) const       { return light.Size(); }
	inline GLLight *GetLight( int i )           { return light[i]; }

	// Multiply various camera matrices onto the current matrix stack
	inline void PerspectiveMatrix( void )       { gluPerspective( camera->GetFovy(), ((float)screenWidth)/screenHeight, camera->GetNear(), camera->GetFar() ); }
	inline void LookAtMatrix( void )            { camera->LookAtMatrix(); }
	inline void LookAtInverseMatrix( void )		{ camera->InverseLookAtMatrix(); }

	// Multiply various light matrices onto the current matrix stack
	void LightPerspectiveMatrix( int i, float aspect );
	void LightLookAtMatrix( int i );
	void LightLookAtInverseMatrix( int i );

	// Whenever the eye's viewpoint changes, the lights need to be reset.  This
	//   function should be called everytime you plan to use lighting and change
	//   the modelview matrix (e.g., using gluLookAt() or LookAtMatrix()). 
	//   Usually this is called *immediately* after either of those functions.
	void SetupEnabledLightsWithCurrentModelview( void );

	// Draw all the geometry in the scene
	inline void Draw( unsigned int matlFlags=MATL_FLAGS_NONE,      // Any special instructions for geometry materials?
		              unsigned int optionFlags=OBJECT_OPTION_NONE, // Any optional instructions (e.g., use low res model)
					  bool matlAlreadySpecified=false )            // Have you already specified a material? (Ignore those in scene)			    
		{ geometry->Draw( this,matlFlags,optionFlags,matlAlreadySpecified ); }

	// Draw only a portion of the scene geometry 
	inline void DrawOnly( unsigned int propertyFlags,              // Which objects should be draw? (background? reflective? those casting shadows?)              
		              unsigned int matlFlags=MATL_FLAGS_NONE,      // Any special instructions for geometry materials??
				      unsigned int optionFlags=OBJECT_OPTION_NONE, // Any optional instructions (e.g., use low res model)
					  bool matlAlreadySpecified=false )            // Have you already specified a material? (Ignore those in scene)
		{ geometry->DrawOnly( this,propertyFlags,matlFlags,optionFlags,matlAlreadySpecified); }

	// Create a shadow map and associate it with the scene for easier rendering.
	//    Please note (for 22C:251) this function is NOT FULLY IMPLEMENTED!
	void CreateShadowMap( FrameBuffer *shadMapBuf,         // Put the shadow map in this FBO
		                  float *shadMapMatrixTranspose,   // Put the shadow map matrix (transpose) here
						  int lightNum,                    // Create the shadow map for which light?
						  float shadowMapBias=0 );         // What is the shadow map bias?

	// If you need to load textures, shaders, models, etc., you may wish to use
	//    the search path class to search in specific locations. 
	// FILE *f = scene->paths->OpenTexture( "myTex.ppm", "r" );  is the same as
	// FILE *f = fopen( "myTex.ppm", "r" );  only it searches a number of directories
	//    to find the file "myTex.ppm".  You can add to the list of paths by calling
	//    scene->paths->AddTexturePath( "anotherDirectory/" ); or by specifying them 
	//    in the scene file.
	ProgramSearchPaths *paths;


/****************************************************************************/
/* Functions you *might* use when implementing new rendering techniques...  */
/****************************************************************************/
public:
	// If your scene file defines variables, you can access them in the
	//    program using these functions.  For instance, if your scene has the line:
	// float indexOfRefraction 1.3
	//    you can access that variable in your program using:
	// float *idxRefract = scene->GetScenFloatVar( "indexofrefraction" );
	// if (*idxRefract > 1.5) printf("Index of refraction is greater than 1.5!");   
	//    Please note the lower-case in the input!
	float *GetSceneFloatVar( char *varName );
	int *GetSceneIntVar    ( char *varName );
	bool *GetSceneBoolVar  ( char *varName );

	// If you forget where you stored your shadow map when calling 
	//    scene->CreateShadowMap(), you can get it by calling this method.
	inline GLuint GetShadowMapID( void ) const			{ return shadowMapTexID; }

	// If you forgot what your shadow map matrix is after you've created it using
	//    scene->CreateShadowMap(), you can access the transpose matrix here.
	inline float *GetShadowMapTransposeMatrix( void )	{ return shadMapTransMatrix; }

	// Reload all the shaders in the scene class.  This is already bound to the
	//   "reload-shaders" UI command, but you may need to call it at other times.
	bool ReloadShaders( void );


/****************************************************************************/
/* Functions you SHOULD NOT CALL unless you really know what you're doing,  */
/*    and have examined the code to understand them.  Most of these are     */
/*    used when loading the scene from a file.  They are public, because    */
/*    various objects and materials need access to them as well.            */
/****************************************************************************/
public:
	// Set various internal data
	void SetCamera( Camera *cam );
	inline void SetWidth( int newWidth )		        { screenWidth = newWidth; }
	inline void SetHeight( int newHeight )		        { screenHeight = newHeight; }
	inline void SetGeometry( Group *obj )               { geometry = obj; }
	inline void SetShadowMapID( GLuint id )				{ shadowMapTexID = id; }
	inline void SetShadowMapTransposeMatrix( float *m ) { memcpy( shadMapTransMatrix, m, 16*sizeof(float) ); }

	// Get the internal scene geometry.  
	inline Group *GetGeometry( void ) const    { return geometry; }

	// This is here as a stub (my research version of this code base uses this)
	//   because removing calls to this was a bit of a pain.
	inline float GetLightIntensityModifier( void ) const { return 1.0f; }
	
	// Add various object types to the internal scene data structures
	inline void AddLight( GLLight *lght )          { light.Add( lght ); }
	inline void AddTexture( GLTexture *tex )      { fileTextures.Add( tex ); }
	inline void AddMaterial( Material *mat )      { fileMaterials.Add( mat ); }
	inline void AddShader( GLSLProgram *shader )  { fileShaders.Add( shader ); }

	// Setup trackballs for objects and lights.  These are called by object
	//    and/or light constructors when they find out the user wants to 
	//    associate a trackball with the object/light.  The scene takes care
	//    of setting things up correctly.
	// 'i' is a user spectified trackball ID.  Usually this works in conjunction
	//    with a scene-defined user interface that attaches some manipulation to
	//    this particular trackball ID.  A warning is printed if this is out of 
	//    the valid range.
	// The callee is responsible for creating the trackball and remembering a 
	//    pointer for any further use.
	void SetupLightTrackball( int i, Trackball *ball );
	void SetupObjectTrackball( int i, Trackball *ball );

	// Preprocess the scene.  This is mainly necessary because the scene is
	//   often loaded before a GL context has been created, but lots of setup
	//   must be done after we have a GL context.
	void Preprocess( void );
	void SetupDefaultScenePaths( void );

	// This function is really only valid during scene preprocessing. 
	//    Calling at other times may return NULL values, invalid values, or
	//    otherwise corrupted data (i.e., undefined behavior).  It is public
	//    because various Object types access this single GLU quadric.
	inline GLUquadricObj *GetQuadric( void )   { return quadObj; }

	// Process the scene between frames
	inline void PerFrameUpdate( float currentTime ) { if (geometry->NeedPerFrameUpdates()) geometry->Update( currentTime ); }

	// When loading the scene, you may want to first check the scene data
	//    structures to make sure you don't duplicate models, textures, shaders,
	//    etc.  The Existing*FromFile() check for these duplicates and return
	//    pointers to them if they exist.  
	Material *ExistingMaterialFromFile( char *name );
	GLTexture *ExistingTextureFromFile( char *filename );
	GLTexture *GetNamedTexture( char *name );
	Object *ExistingObjectFromFile( char *name );
	GLLight *ExistingLightFromFile( char *name );

	// The scene has a default material.  If a scene file does not specify
	//    a material for a particular object, the default is used.  This
	//    returns a pointer to this default material.
	Material *GetDefaultMaterial( void ) { return fileMaterials[0]; }

	// Assuming you don't already have a specified material, object, camera, etc
	//    at some point you actually want to load them.  These functions do that.
	Material *LoadMaterial( char *typeString, FILE *file );
	Object *LoadObject( char *typeString, FILE *file );
	Camera *LoadCamera( char *typeString, FILE *file );
	
	// When parsing the file, we sometimes encounter keywords we do not 
	//    understand.  Since some of theys keywords may actually be blocks 
	//    of data, we cannot simply throw these lines away.  This method
	//    parses these unknown keywords/blocks in a semi-intelligent manner
	//    to eliminate as many syntax errors as possible when reading scenes
	void UnhandledKeyword( FILE *f, char *typeString=0, char *keyword=0 );

	// To truly deal with unhandled keywords, we must deal with unhandled
	//    keywords with nested keyword blocks (e.g., a material defined 
	//    inside an object).  In these cases, searching for the first "end"
	//    doesn't work.  This checks if the keyword opens a block, and if
	//    so UnhandledKeyword() can be called recursively.
	bool IsBlockKeyword( char *keyword );


/****************************************************************************/
/* Internal data structures you may want to look at and understand well.    */
/****************************************************************************/
private:
	Camera *camera;           // Scene camera.  There is only one (for now).
	Group *geometry;          // Scene geometry.  There is only one, as it should be a container object.
	Array1D<GLLight *> light; // Scene lights.  A list of all lights in the scene.

	// Size of resulting image
	int screenWidth, screenHeight;

	// If we call CreateShadowMap, we'll store some temporary data here to allow usage
	//   of the shadow map by anyone with access to the scene data.  This could be 
	//   modified to encapsulate the data better, by making the scene object control
	//	 the shadow map.
	GLuint shadowMapTexID;
	float shadMapTransMatrix[16];

/****************************************************************************/
/* Internal data structures you may have use for.                           */
/****************************************************************************/
private:
	// The following variables are declared in the scene file.  These variables have an associated
	//    data type and a character string name.  A search through the identifier list gives the
	//    array index of the identifier, which is the same index as the corresponding data in the
	//    data array.
	Array1D<char *>			floatVarNames;
	Array1D<float>			floatData;
	Array1D<char *>			intVarNames;
	Array1D<int>			intData;
	Array1D<char *>			boolVarNames;
	Array1D<bool>			boolData;


/****************************************************************************/
/* Internal data structures you should not need to modify or understand,    */
/*     unless you plan to significantly modify the framework.               */
/****************************************************************************/
private:
	// These are used for loading the scene into memory from a file
	Array1D<Material *   > fileMaterials;  // a list of materials, so they can be named and reused
	Array1D<GLTexture *  > fileTextures;   // a list of textures, so they can be reused
	Array1D<GLSLProgram *> fileShaders;    // a list of shaders, so they can be reloaded when needed

	// If the float, int, or bool values declared in the scene file are accessed
	//   before all the scene variables are defined, bad things can happen.  This
	//   variable tracks if they have been accessed so errors can be printed it the
	//   user breaks this rule.
	bool sceneFileDataAccessed;

	// Used for loading objects.  Some may have names associated, in which case we may 
	//     need the names later in the file to reference them (e.g., for instances)
	Array1D<Object *> namedObjects;
	Array1D<char *>   objectNames;

	// Describes verbosity of output.  Not all output to the console is
	//    surpressed with this flag.
	bool verbose;

	// Used if our scene has any spheres or cylinders
	GLUquadricObj *quadObj;
	
};



#endif



