/******************************************************************/
/* Object.h                                                       */
/* -----------------------                                        */
/*                                                                */
/* The file defines a base object class that is inherited by more */
/*     complex object types.                                      */
/*                                                                */
/* Chris Wyman (10/26/2006)                                       */
/******************************************************************/

#ifndef OBJECT_H
#define OBJECT_H

#include <stdio.h>
#include <stdlib.h>
#include "Utils/TextParsing.h"


// Requirements to draw ( passed to DrawOnly() )...  If the
//    object does not meet the criteria passed to DrawOnly(),
//    it is not drawn.  They may be OR'd together, though some
//    make no sense together (e.g., foreground/background)
#define OBJECT_FLAGS_NONE					0x00000000   
#define OBJECT_FLAGS_ISBACKGROUND				0x00000001   
#define OBJECT_FLAGS_ISFOREGROUND				0x00000002
#define OBJECT_FLAGS_CASTSSHADOWS				0x00000003
#define OBJECT_FLAGS_ISREFLECTIVE				0x00000008
#define OBJECT_FLAGS_ISREFRACTIVE				0x00000010
#define OBJECT_FLAGS_ALLOWDRAWEDGESONLY     			0x00000020

// These are optional requests to the drawing routines, and 
//    may be ignored if a particular object type does not 
//    support them.  If the options are ignored, the object
//    may display as usual without any modifications.
#define OBJECT_OPTION_NONE					0x00000000
#define OBJECT_OPTION_USE_LOWRES			0x00000001


class Material;
class Vector;
class Material;
class Scene;
class Trackball;

class Object {
public:
	Object( Material *matl=0 ) : matl(matl), ball(0), flags(0), objectOptionFlags(0) {}
	Object( FILE *f, Scene *s ) 
	     { printf("Constructor Object::Object( FILE *f ) called.  This is not implemented!"); }
	virtual ~Object() {}

	// The basic operation every object must do:  Draw itself.   
	virtual void Draw( Scene *s, 
		               unsigned int matlFlags, 
		               unsigned int optionFlags=OBJECT_FLAGS_NONE, 
		               bool matlAlreadySpecified=false ) = 0;

	// Draw this object (or it's sub-objects only if they have some property
	virtual void DrawOnly( Scene *s, 
		                   unsigned int propertyFlags, 
						   unsigned int matlFlags, 
		                   unsigned int optionFlags=OBJECT_FLAGS_NONE, 
						   bool matlAlreadySpecified=false ) = 0;

	// Some data may need preprocessing, perhaps to collect it into a group efficiently,
	//    or some other reason.  If so, you need to specify the object needs processing
	//    and then write a method to do so.
	virtual bool NeedsPreprocessing( void ) { return false; }
	virtual void Preprocess( Scene * ) {}

	// Functions to see if geometry wants to update itself every frame.
	virtual bool NeedPerFrameUpdates( void ) { return false; }
	virtual void Update( float currentTime ) {}

	// Functions to get and set the material type
	inline Material *GetMaterial( void )         { return matl; }
	inline void SetMaterial( Material *newMatl ) { matl = newMatl; }

	// Function to return and set the trackball
	inline Trackball *GetTrackball( void )         { return ball; }
	inline void SetTrackball( Trackball *newBall ) { ball = newBall; }

protected:
	Material *matl;
	Trackball *ball;
	unsigned int flags, objectOptionFlags;

	// When reading from a file, there's commond properties of all object,
	//   (e.g., those stored in the base object) that are annoying to repeat
	//   checks for in all object constructors...
	bool TestCommonObjectProperties( char *keyword, char *restOfLine, Scene *s, FILE *f );
};


#endif

