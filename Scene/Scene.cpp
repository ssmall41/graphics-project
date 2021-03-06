/******************************************************************/
/* Scene.cpp                                                      */
/* -----------------------                                        */
/*                                                                */
/* The file defines a scene class that encapsulates all the       */
/*     information necessary to render an image with a ray tracer */
/* Also not that this class includes the TraceRay() method, which */
/*     actually traces a ray through the scene.                   */
/*                                                                */
/* Chris Wyman (10/26/2006)                                       */
/******************************************************************/

#include "Utils/ImageIO/imageIO.h"
#include "DataTypes/Color.h"
#include "DataTypes/glTexture.h"
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

// This is a big hack...  Sometimes the global scene pointer
//    is needed before the Scene( filename ) constructor returns...
//    So this needs to be set in the constructor.
extern Scene *scene;

SceneDefinedUI *ui=0;
Trackball *eyeBall      = 0;
Trackball *lightBall[4] = {0,0,0,0};
Trackball *objBall[4]   = {0,0,0,0};


void Scene::SetupDefaultScenePaths( void )
{
	paths->AddShaderPath( "shaders/" );
	paths->AddShaderPath( "shaders/normalSurfaceShaders/" );
	paths->AddTexturePath( "textures/" );
	paths->AddModelPath( "models/" );	
	paths->AddScenePath( "scenes/" );
}


Scene::Scene() : 
	camera(0), geometry(0),
	screenWidth(256), screenHeight(256), 
	verbose(true), sceneFileDataAccessed(false)
{
	paths = new ProgramSearchPaths();
	SetupDefaultScenePaths();
	ui = new SceneDefinedUI();
}


// Free all the allocated memory inside the scene.
Scene::~Scene()
{
	if (camera) delete camera;
	if (geometry) delete geometry;
}

// Set the camera to a new camera.
void Scene::SetCamera( Camera *cam )
{
	if (camera) delete camera;
	camera = cam;
}

// This is essentially equivalent to calling glLightfv() for
//   all of the currently active lights.
void Scene::SetupEnabledLightsWithCurrentModelview( void )
{
	for (unsigned int i=0; i<light.Size(); i++)
		if (light[i]->IsEnabled())
			light[i]->SetLightUsingCurrentTransforms();
}

// This is called by the scene constructor.  You should never
//    need to call this
void Scene::SetupLightTrackball( int i, Trackball *ball )
{
	if (i < 0 || i > 3)
	{
		char buf[255];
		sprintf( buf, "Light trackball ID #%d out of range (0..3)!", i);
		Warning(buf);
		return;
	}
	lightBall[i] = ball;
}

// This is called by the scene constructor.  You should never
//    need to call this
void Scene::SetupObjectTrackball( int i, Trackball *ball )
{
	if (i < 0 || i > 3)
	{
		char buf[255];
		sprintf( buf, "Object trackball ID #%d out of range (0..3)!", i);
		Warning(buf);
		return;
	}
	objBall[i] = ball;
}

// Check if this scene file keyword opens a block (that needs closure
//    with a matching "end" line).  Technically, this need not return
//    true on *all* block keywords, only those that can be nested inside
//    other blocks.
// This is called by the scene constructor.  You should never
//    need to call this
bool Scene::IsBlockKeyword( char *keyword )
{
	if (!strcmp(keyword, "object") || !strcmp(keyword, "material") || !strcmp(keyword, "instance"))
		return true;
	return false;
}

// When reading the scene file, there may be keywords we do not understand.
//   This function removes blocks of the scene we do not understand and prints
//   a warning message.
// This is called by the scene constructor.  You should never
//    need to call this.
void Scene::UnhandledKeyword( FILE *f, char *typeString, char *keyword )
{
	char buf[ MAXLINELENGTH ];
	if (typeString && keyword)
	{
		sprintf(buf, "Currently unhandled %s: '%s'!", typeString, keyword);
		Warning( buf );
	}

	// Search the scene file.
	char token[256], *ptr;
	while( fgets(buf, MAXLINELENGTH, f) != NULL )  
	{
		ptr = StripLeadingWhiteSpace( buf );
		if (ptr[0] == '#') continue;
		ptr = StripLeadingTokenToBuffer( ptr, token );
		MakeLower( token );
		if (!strcmp(token,"end")) break;
		else if (IsBlockKeyword( token ))
			UnhandledKeyword( f );
	}
}

// A constructor to read a scene from a file
Scene::Scene( char *filename, bool verbose ) : 
	camera(0), geometry(0), screenWidth(256), screenHeight(256), verbose(verbose),
	sceneFileDataAccessed(false)
{
	// HACK!
	scene = this;
	// END HACK!

	paths = new ProgramSearchPaths();
	SetupDefaultScenePaths();
	ui = 0;

	printf("    (-) Loading scene from '%s'...\n", filename);
	char buf[ MAXLINELENGTH ], token[256], *ptr;
	FILE *sceneFile = paths->OpenScene( filename, "r" );
	if (!sceneFile) 
		FatalError( "Scene::Scene() unable to open scene file '%s'!", filename );

	// Setup a default material type, in case objects don't define their own...
	Material *defaultMatl = new GLMaterial( "__sceneDefaultMaterial" );
	fileMaterials.Add( defaultMatl );

	// A group for all scene objects
	geometry = new Group();

	// Iterate through the lines of the file....
	while( fgets(buf, MAXLINELENGTH, sceneFile) != NULL )  
	{
		// Is this line a comment?
		ptr = StripLeadingWhiteSpace( buf );
		if (ptr == 0 || ptr[0] == '\n' || ptr[0] == '#') continue;

		// Nope.  So find out what the command is...
		ptr = StripLeadingTokenToBuffer( ptr, token );
		MakeLower( token );
	
		// Take different measures, depending on the command.  You may need to add more!
		if (!strcmp(token, "")) continue;
		
		// "epsilon" manages numerical errors in a ray tracer.  Ignore.
		else if (!strcmp(token, "epsilon"))
			Warning( "Keyword 'epsilon' ignored for OpenGL programs!" );
		
		// This maps key/mouse controls to certain functions in the program
		else if (!strcmp(token, "keymap") || !strcmp(token, "uimap") || !strcmp(token, "controls"))
			ui = new SceneDefinedUI( sceneFile );

		// The keywords "float", "int" and "bool" define variables with scope in the
		//    file that can also be used in the program.  These are useful because the
		//    file can bind/map these variables to keystrokes that change their value.
		else if (!strcmp(token, "float"))
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );
			floatVarNames.Add( strdup( token ) );
			ptr = StripLeadingTokenToBuffer( ptr, token );
			floatData.Add( (float)atof( token ) );
			if (sceneFileDataAccessed)
				Error("Cannot use *any* variables in scene file until *all* are defined!  Expect undefined behavior!");
		}

		// The keywords "float", "int" and "bool" define variables with scope in the
		//    file that can also be used in the program.  These are useful because the
		//    file can bind/map these variables to keystrokes that change their value.
		else if (!strcmp(token, "int"))
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );
			intVarNames.Add( strdup( token ) );
			ptr = StripLeadingTokenToBuffer( ptr, token );
			intData.Add( atoi( token ) );
			if (sceneFileDataAccessed)
				Error("Cannot use *any* variables in scene file until *all* are defined!  Expect undefined behavior!");
		}

		// The keywords "float", "int" and "bool" define variables with scope in the
		//    file that can also be used in the program.  These are useful because the
		//    file can bind/map these variables to keystrokes that change their value.
		else if (!strcmp(token, "bool"))
		{
			bool value=false;
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );
			boolVarNames.Add( strdup( token ) );
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );
			if (!strcmp(token,"t") || !strcmp(token,"true") || token[0]=='y' || token[0]=='1') 
				value = true;
			boolData.Add( value );
			if (sceneFileDataAccessed)
				Error("Cannot use *any* variables in scene file until *all* are defined!  Expect undefined behavior!");
		}

		// Defines a material.  Let's load it.
		else if (!strcmp(token,"material"))
		{
			Material *matl = LoadMaterial( ptr, sceneFile );
			if (matl) fileMaterials.Add( matl );
		}

		// Defines an object.  Let's load it.
		else if (!strcmp(token,"object"))
		{
			Object *obj = LoadObject( ptr, sceneFile );
			if (obj) geometry->Add( obj );
		}

		// Defines a texture.  Let's load it.
		else if (!strcmp(token,"texture"))
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );
			if ( GetNamedTexture( token ) )
				Error("Multiply defined texture, named %s!", token);
			else
			{
				char file[256];
				ptr = StripLeadingTokenToBuffer( ptr, file );
				char *fullPath = paths->GetTexturePath( file );
				GLTexture *tex = new GLTexture( fullPath, 
					                            TEXTURE_MIN_NEAR_MIP_NEAR, 
												true ); 
				tex->SetName( token );
				free( fullPath );
				fileTextures.Add( tex );
			}
		}

		// Defines a directory to search for models/textures/shaders.  
		else if (!strcmp(token,"directory") || !strcmp(token,"dir") || !strcmp(token,"path"))
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			for (int i=0;ptr[i];i++)
				if (ptr[i] == '\r' || ptr[i] == '\n' || ptr[i] == '\t')	ptr[i] = 0;

			MakeLower( token );
			if( !strcmp(token, "model") || !strcmp(token, "models")) 
				paths->AddModelPath( ptr );
			if (!strcmp(token, "texture") || !strcmp(token, "textures")) 
				paths->AddTexturePath( ptr );
			if (!strcmp(token, "shader") || !strcmp(token, "shaders")) 
				paths->AddShaderPath( ptr );
		}

		// Defines a light.  Let's load it.
		else if (!strcmp(token,"light"))
			AddLight( new GLLight( sceneFile, this ) );

		// Defines a camera.  Let's load it.
		else if (!strcmp(token,"camera"))  
			SetCamera( LoadCamera( ptr, sceneFile) );

		// We have no clue what this user was typing...
		else
			Error( "Unknown scene command '%s' in Scene::Scene()!", token );
	}

	// If there are no key controls defined in the file, define a (very minimal) basic set
	if (!ui)
		ui = new SceneDefinedUI();

	// A scene *must* have a camera.
	if (!camera)
		FatalError("Scene file has no camera specification!");

	// If there are any trackballs, make sure they're visible to the interface routines.
	eyeBall = camera->GetTrackball();

	// Make sure and trackball defined in the scene all have the correct size.
	if (eyeBall) eyeBall->ResizeTrackballWindow( screenWidth, screenHeight );
	for (int i=0; i<4; i++)
	{
		if (lightBall[i]) lightBall[i]->ResizeTrackballWindow( screenWidth, screenHeight );
		if (objBall[i])   objBall[i]->ResizeTrackballWindow( screenWidth, screenHeight );
	}

	// Clean up and close the file
	fclose( sceneFile );
	if (verbose) printf("    (-) Finished loading scene.\n");
}


// Ok.  We're loading an object.
Object *Scene::LoadObject( char *typeString, FILE *file )
{
	Object *objPtr=0;
	char token[256], *ptr;

	// What type of object?
	ptr = StripLeadingTokenToBuffer( typeString, token );
	MakeLower( token );	

	// Load the correct type by calling its constructor.
	if (!strcmp(token,"sphere"))
		objPtr = new Sphere( file, this );
	else if (!strcmp(token,"parallelogram"))
		objPtr = new Quad( file, this );
	else if (!strcmp(token,"texparallelogram"))
		objPtr = new Quad( file, this );
	else if (!strcmp(token,"testquad"))
		objPtr = new Quad( file, this );
	else if (!strcmp(token,"testdisplacedquad"))
		objPtr = new Quad( file, this );
	else if (!strcmp(token,"noisyquad"))
		objPtr = new Quad( file, this );
	else if (!strcmp(token,"triangle") || !strcmp(token,"tri"))
		objPtr = new Triangle( file, this );
	else if (!strcmp(token,"textri"))
		objPtr = new Triangle( file, this );
	else if (!strcmp(token,"cyl") || !strcmp(token,"cylinder"))
		objPtr = new Cylinder( file, this );
	else if (!strcmp(token,"mesh"))
	{	
		objPtr = new Mesh( ptr, file, this );
		ptr = StripLeadingTokenToBuffer( ptr, token ); // remove object-type from string (in case mesh is named)
	}
	else if (!strcmp(token,"instance") || !strcmp(token,"group"))
		objPtr = new Group( file, this );
	else if (!strcmp(token,"plane"))
		Error("The InfinitePlane class has been removed.  Use a Parallelogram instead!");
	else
		FatalError("Unknown object type '%s' in LoadObject()!", token);

	// If there is another string on the line, it is the name of the current object.
	ptr = StripLeadingTokenToBuffer( ptr, token );
	if ( token[0] && !IsWhiteSpace( token[0] ) && objPtr )
	{
		if (!objPtr->GetMaterial() || objPtr->GetMaterial() == fileMaterials[0] )
		{
			if (verbose) printf("    (-) Finished reading object '%s'...\n", token );
		}
		else if (verbose) 
			printf("    (-) Finished reading object '%s' with material '%s'...\n", 
					token, objPtr->GetMaterial()->GetName() );
		namedObjects.Add( objPtr );
		objectNames.Add( strdup( token ) );
	}
	return objPtr;
}

// Given the string (read from the file) after the 'camera' keyword, determine what
//    *type* of camera, load the camera, and return it.  
Camera *Scene::LoadCamera( char *typeString, FILE *file )
{
	Camera *cameraPtr;
	char token[256], *ptr;
	ptr = StripLeadingTokenToBuffer( typeString, token );
	MakeLower( token );	
	if (!strcmp(token,"pinhole"))
		cameraPtr = new Camera( file, this );
	else
	{
		Warning("Camera type '%' unhandled by OpenGL programs, trying pinhole camera!", token);
		cameraPtr = new Camera( file, this );
	}
	return cameraPtr;
}


// Given the string (read from the file) after the 'material' keyword, determine what
//    *type* of material it is, load that material, give it (optionally) a name, and
//    return it
Material *Scene::LoadMaterial( char *typeString, FILE *file )
{
	Material *matlPtr=0;
	char token[256], *ptr;

	// What type of material are we reading?
	ptr = StripLeadingTokenToBuffer( typeString, token );
	MakeLower( token );	
	if (!strcmp(token,"lambertian"))
		matlPtr = new GLLambertianMaterial( file, this );
	else if (!strcmp(token,"glmatl"))
		matlPtr = new GLMaterial( file, this );
	else if (!strcmp(token,"texmat") || !strcmp(token,"texturematerial") || !strcmp(token,"lambertiantexture"))
		matlPtr = new GLLambertianTexMaterial( file, this );
	else if (!strcmp(token,"shader"))
		matlPtr = new GLSLShaderMaterial( file, this );
	else if (!strcmp(token,"constant"))
		matlPtr = new GLConstantMaterial( file, this );
	else if (!strcmp(token,"simpleemitter") || !strcmp(token,"ashikhminshirley") || 
		     !strcmp(token,"ashikhmin") || !strcmp(token,"brdf") || 
			 !strcmp(token,"fresnelreflective") || !strcmp(token,"reflective") ||
			 !strcmp(token,"refractive") )
		UnhandledKeyword( file, "material type", token );
	else
		FatalError("Unknown material type '%s'!", token);

	// The next string on the line is a material name.  Get that and set the internal name
	ptr = StripLeadingTokenToBuffer( ptr, token );
	MakeLower( token );	
	if (matlPtr) matlPtr->SetName( token );
	if (matlPtr && verbose) printf("    (-) Successfully read material '%s'\n", matlPtr->GetName() );

	return matlPtr;
}


// Check if a material is already in the list.  If so, return it.  Else return NULL;
Material *Scene::ExistingMaterialFromFile( char *name )
{
	MakeLower( name );	
	for (unsigned int i=0; i<fileMaterials.Size(); i++)
		if (fileMaterials[i]->GetName() && !strcmp(name, fileMaterials[i]->GetName()))
			return fileMaterials[i];
	return 0;
}

// Check if a texture with specific filename is already in the list.  If so, return it.  Else return NULL;
GLTexture *Scene::ExistingTextureFromFile( char *filename )
{
	for (unsigned int i=0; i<fileTextures.Size(); i++)
		if (!strcmp(filename, fileTextures[i]->GetFilename()))
			return fileTextures[i];
	return 0;
}

// Check if a texture with specific string name is already in the list.  If so, return it.  Else return NULL;
GLTexture *Scene::GetNamedTexture( char *name )
{
	for (unsigned int i=0; i<fileTextures.Size(); i++)
		if (!strcmp(name, fileTextures[i]->GetName()))
			return fileTextures[i];
	return 0;
}

// Check if a light is already in the list.  If so, return it.  Else return NULL;
GLLight *Scene::ExistingLightFromFile( char *name )
{
	MakeLower( name );	
	for (unsigned int i=0; i<light.Size(); i++)
		if (light[i]->GetName() && !strcmp(name, light[i]->GetName()))
			return light[i];
	return 0;
}

// Check if a named object is already in the list.  If so, return it.  Else return NULL;
Object *Scene::ExistingObjectFromFile( char *name )
{
	for (unsigned int i=0; i<objectNames.Size(); i++)
		if (!strcmp(name, objectNames[i]))
			return namedObjects[i];
	return 0;	
}

// Search through the list of scene-defined floats and return a pointer to the
//   float with the given name.  Please note that <varName> should be all lower case!
float *Scene::GetSceneFloatVar( char *varName )
{
	sceneFileDataAccessed = true;
	for (unsigned int i=0; i<floatVarNames.Size(); i++)
		if (!strcmp(varName,floatVarNames[i])) 
			return &floatData[i];
	return 0;
}

// Search through the list of scene-defined ints and return a pointer to the
//   int with the given name.  Please note that <varName> should be all lower case!
int *Scene::GetSceneIntVar    ( char *varName )
{
	sceneFileDataAccessed = true;
	for (unsigned int i=0; i<intVarNames.Size(); i++)
		if (!strcmp(varName,intVarNames[i])) 
			return &intData[i];
	return 0;
}

// Search through the list of scene-defined bools and return a pointer to the
//   bool with the given name.  Please note that <varName> should be all lower case!
bool *Scene::GetSceneBoolVar  ( char *varName )
{
	sceneFileDataAccessed = true;
	for (unsigned int i=0; i<boolVarNames.Size(); i++)
		if (!strcmp(varName,boolVarNames[i])) 
			return &boolData[i];
	return 0;
}

// Reload all the shaders specified in the file.
bool Scene::ReloadShaders( void )
{
	bool ok = true;
	for (unsigned int i=0; i<fileShaders.Size(); i++)
		ok &= fileShaders[i]->ReloadShaders();
	return ok;
}


// Some stuff cannot be done when the scene is initially loaded
//   (e.g., texture setup).  This stuff needs to be done after the GL
//   context is created.  The preprocess allows all scene objects a
//   chance to do a little more computation before execution begins.
void Scene::Preprocess( void )
{
	// Setup a tesselator if needed by scene objects
	quadObj = gluNewQuadric();
	gluQuadricDrawStyle  ( quadObj, GLU_FILL );
	gluQuadricOrientation( quadObj, GLU_OUTSIDE );
	gluQuadricNormals    ( quadObj, GLU_SMOOTH );
	gluQuadricTexture    ( quadObj, GLU_TRUE );

	printf("(+) Preprocessing scene...\n");
	if (verbose) printf("    (-) Preprocessing scene geometry...\n");
	geometry->Preprocess( this );
	if (verbose) printf("    (-) Setting up scene textures...\n");
	for (unsigned int i=0; i<fileTextures.Size(); i++)
		fileTextures[i]->Preprocess();
	if (verbose) printf("    (-) Checking if materials need preprocessing...\n");
	for (unsigned int i=0; i<fileMaterials.Size(); i++)
	{
		if (fileMaterials[i]->NeedsPreprocessing()) 
			fileMaterials[i]->Preprocess( this );
	}
	if (verbose) printf("(+) Done with Scene::Preprocess()!\n");

	gluDeleteQuadric( quadObj );
}





