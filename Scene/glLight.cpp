/************************************************************************/
/* glLight.h                                                            */
/* ------------------                                                   */
/*                                                                      */
/* This file defines a class storing information about an OpenGL light, */
/*     as well as access routines.                                      */
/*                                                                      */
/* Chris Wyman (12/7/2007)                                              */
/************************************************************************/

#include "sceneLoader.h"


GLLight::GLLight( GLenum lightNum ) :
	lightNum(lightNum), enabled(false), ball(NULL),
		pos( 0, 1, 0 ), currentWorldPos( 0, 1, 0 ),
		amb( 0, 0, 0 ), dif( 0, 0, 0 ), spec( 0, 0, 0 ),
		spotDir( 0, 0, 0 ), spotExp(0), spotCutoff(180.0f),
		mat(0), _near(1), _far(50), fovy(90.0f), name(0)
{
	// Default OpenGL light attenuation
	atten[0] = 1.0f; atten[1] = atten[2] = 0.0f;

	// GL_LIGHT0 has special defaults...
	if (lightNum == GL_LIGHT0)
	{
		dif = Color::White();
		spec = Color::White();
	}
}

GLLight::GLLight( FILE *f, Scene *s ) :
	lightNum(0), enabled(false), ball(NULL),
		pos( 0, 1, 0 ), currentWorldPos( 0, 1, 0 ),
		amb( 0, 0, 0 ), dif( 0, 0, 0 ), spec( 0, 0, 0 ),
		spotDir( 0, 0, 0 ), spotExp(0), spotCutoff(180.0f),
		mat(0), _near(1), _far(50), fovy(90.0f), name(0)
{ 
	lightNum = s->GetNumLights() + GL_LIGHT0;

	// Search the scene file.
	char buf[ MAXLINELENGTH ], token[256], *ptr;
	while( fgets(buf, MAXLINELENGTH, f) != NULL )  
	{
		// Is this line a comment?
		ptr = StripLeadingWhiteSpace( buf );
		if (ptr[0] == '#') continue;

		// Nope.  So find out what the command is...
		ptr = StripLeadingTokenToBuffer( ptr, token );
		MakeLower( token );
	
		// Take different measures, depending on the command.
		if (!strcmp(token,"end")) break;
		if ( !strcmp(token,"rgb") ||      // This means there's only one light "color"
			 !strcmp(token,"color") ||    //   so assign amb, dif, and spec colors to 
			 !strcmp(token,"spectral"))   //   this value.
		{
			 dif = LoadColor::Load( token, f, ptr );
			 amb = dif; spec = dif;
		}
		else if (!strcmp(token,"amb") || !strcmp(token,"ambient") )  
			amb = LoadColor::Load( token, f, ptr );
		else if (!strcmp(token,"dif") || !strcmp(token,"diffuse") )  
			dif = LoadColor::Load( token, f, ptr );
		else if (!strcmp(token,"spec") || !strcmp(token,"specular") )  
			spec = LoadColor::Load( token, f, ptr );
		else if (!strcmp(token,"pos") || !strcmp(token,"position") )  
			pos = Point( ptr );
		else if (!strcmp(token, "matrix") || !strcmp(token, "xform"))
		{
			if (!mat) 
				mat = new Matrix4x4( f, ptr );
			else
				(*mat) *= Matrix4x4( f, ptr );
		}
		else if (!strcmp(token, "trackball"))
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			int id = atoi(token);
			ball = new Trackball( s->GetWidth(), s->GetHeight() );
			s->SetupLightTrackball( id, ball );
		}
		else if (!strcmp(token,"name"))
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			this->SetName( token );
		}
		else
			printf("Error: Unknown command '%s' when loading Light\n", token);
	}


	
	currentWorldPos = pos;
}



GLLight::~GLLight()
{
	if( mat) delete mat;
}

void GLLight::SetAttenuation( float constant, float linear, float quadratic )
{
	atten[0] = constant; 
	atten[1] = linear; 
	atten[2] = quadratic;
	glLightf( lightNum, GL_CONSTANT_ATTENUATION, atten[0] );
	glLightf( lightNum, GL_LINEAR_ATTENUATION, atten[1] );
	glLightf( lightNum, GL_QUADRATIC_ATTENUATION, atten[2] );
}

void GLLight::SetSpotCutoff( float degrees )
{
	spotCutoff = degrees;
	glLightf( lightNum, GL_SPOT_CUTOFF, spotCutoff );
}

void GLLight::SetSpotExponent( float exponent ) 
{ 
	spotExp = exponent; 
	glLightf( lightNum, GL_SPOT_EXPONENT, spotExp );
}

void GLLight::SetLightUsingCurrentTransforms( void )
{
	if (mat || ball)
	{
		glPushMatrix();
		if (mat)  glMultMatrixf( mat->GetDataPtr() );
		if (ball) 
			ball->MultiplyTrackballMatrix();
	}
	glLightfv( lightNum, GL_POSITION, pos.GetDataPtr() );
	glLightfv( lightNum, GL_SPOT_DIRECTION, pos.GetDataPtr() );
	if (mat || ball)
		glPopMatrix();
}

const Point GLLight::GetCurrentPos( void )        
{
	if (mat && ball)
		return (*mat) * ball->ApplyTrackballMatrix( pos );
	if (ball)
		return ball->ApplyTrackballMatrix( pos );
	if (mat)
		return (*mat) * pos;
	return pos;
}



