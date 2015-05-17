/******************************************************************/
/* Camera.cpp                                                     */
/* -----------------------                                        */
/*                                                                */
/* The file defines a camera class that encapsulates infromation  */
/*    necessary for rendering with an OpenGL camera.              */
/*                                                                */
/* Chris Wyman (01/30/2008)                                       */
/******************************************************************/

#include "Camera.h"
#include "Scene.h"
#include "Utils/TextParsing.h"
#include "Utils/ImageIO/imageIO.h"


Camera::Camera( const Point &eye, const Point &at, const Vector &up, 
			   float fovy, float near, float far ) :
	eye(eye), at(at), up(up), fovy(fovy), _near(near), _far(far)
{
}


void Camera::LookAtMatrix( void )
{
	if (ball)
	{
		Vector rotVec = ball->ApplyTrackballMatrix( at-eye );
		gluLookAt( at.X()-rotVec.X(), at.Y()-rotVec.Y(), at.Z()-rotVec.Z(), 
				   at.X(), at.Y(), at.Z(), 
				   up.X(), up.Y(), up.Z() );
	}
	else
		gluLookAt( eye.X(), eye.Y(), eye.Z(), 
				   at.X(), at.Y(), at.Z(), 
				   up.X(), up.Y(), up.Z() );
}


void Camera::InverseLookAtMatrix( void )
{
	Matrix4x4 m;
	if (ball)
	{
		Vector rotVec = ball->ApplyTrackballMatrix( at-eye );
		m = Matrix4x4::LookAt( at-rotVec, at, up ).Invert();
	}
	else
		m = Matrix4x4::LookAt( eye, at, up ).Invert();
	
	glMultMatrixf( m.GetDataPtr() );
}


Camera::Camera( FILE *f, Scene *s ) :
  fovy(90.f), eye( 0, 0, 1 ), at( 0, 0, 0 ),
  up( 0, 1, 0 ), _near(0.1), _far(20), ball(0)
{
	// Setup default values, in case the scene file is defective...
	s->SetWidth ( 512 );
	s->SetHeight( 512 );

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
		if (!strcmp(token,"eye")) 
			eye = Point( ptr ); 
		else if (!strcmp(token,"at")) 
			at = Point( ptr ); 
		else if (!strcmp(token,"up")) 
			up = Vector( ptr ); 
		else if (!strcmp(token,"w") || !strcmp(token,"width") )  
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			s->SetWidth( (int)atof( token ) );
		}
		else if (!strcmp(token,"h") || !strcmp(token,"height") )  
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			s->SetHeight( (int)atof( token ) );
		}
		else if (!strcmp(token,"res") || !strcmp(token, "resolution"))
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			s->SetWidth( (int)atof( token ) );
			ptr = StripLeadingTokenToBuffer( ptr, token );
			s->SetHeight( (int)atof( token ) );
		}
		else if (!strcmp(token,"trackball"))
			ball = new Trackball( s->GetWidth(), s->GetHeight() );
		else if (!strcmp(token,"fovy"))  
			ptr = StripLeadingNumber( ptr, &fovy );
		else if (!strcmp(token,"near"))  
			ptr = StripLeadingNumber( ptr, &_near );
		else if (!strcmp(token,"far"))  
			ptr = StripLeadingNumber( ptr, &_far );
		else
			Error("Unknown command '%s' when loading Camera!", token);
	}

}


