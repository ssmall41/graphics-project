/******************************************************************/
/* Cylinder.cpp                                                     */
/* -----------------------                                        */
/*                                                                */
/* The file defines a simple drawable OpenGL sphere type.         */
/*                                                                */
/* Chris Wyman (01/01/2008)                                       */
/******************************************************************/

#include "sceneLoader.h"

Cylinder::Cylinder( Material *matl ) : 
	Object(matl), displayList(0), center( Point::Origin() ), 
	slices(20), stacks(20), radius(1.0f)
{	
}

void Cylinder::Draw( Scene *s, unsigned int matlFlags, unsigned int optionFlags, bool matlAlreadySpecified )
{
	if (!matlAlreadySpecified && matl)
		matl->Enable( s, matlFlags );

	glCallList( displayList );

	if (!matlAlreadySpecified && matl)
		matl->Disable();
}

// Draw this object (or it's sub-objects only if they have some property)
void Cylinder::DrawOnly( Scene *s, unsigned int propertyFlags, unsigned int matlFlags, unsigned int optionFlags, bool matlAlreadySpecified )
{
	if ((propertyFlags & flags) == propertyFlags) 
		this->Draw( s, matlFlags, optionFlags, matlAlreadySpecified );
}


void Cylinder::Preprocess( Scene *s )
{
	Vector rotateCylinderBy = Vector::ZAxis().Cross( axis );
	float rotateLength = rotateCylinderBy.Normalize();
	float dotPrd = Vector::ZAxis().Dot( axis );
	float angle = 180.0f * atan2( rotateLength, dotPrd ) / M_PI;

	displayList = glGenLists(1);
	glNewList( displayList, GL_COMPILE );
	glPushMatrix();
	glTranslatef( center.X(), center.Y(), center.Z() );
	glRotatef( angle, rotateCylinderBy.X(), rotateCylinderBy.Y(), rotateCylinderBy.Z() );
	glTranslatef( 0, 0, -0.5*height );
	gluCylinder( s->GetQuadric(), radius, radius, height, slices, stacks );
	glPopMatrix();
	glEndList();
}


Cylinder::Cylinder( FILE *f, Scene *s ) :
	Object( s->GetDefaultMaterial() ),
	center( Point::Origin() ), slices(50), 
	stacks(25), displayList(0), radius(1.0f),
	axis( Vector::ZAxis() ), height(1.0f)
{
	bool normalsDefined=false;

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
		if (TestCommonObjectProperties( token, ptr, s, f ))
			continue;
		else if (!strcmp(token,"center"))
			center = Point( ptr );
		else if (!strcmp(token,"radius"))
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			radius = (float)atof( token );
		}
		else if (!strcmp(token,"height"))
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			height = (float)atof( token );
		}
		else if (!strcmp(token,"axis"))
		{
			axis = Vector( ptr );
			axis.Normalize();
		}
		else if (!strcmp(token, "stacks"))
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			stacks = (unsigned char)MIN(255,atoi( token ));
		}
		else if (!strcmp(token, "slices"))
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			slices = (unsigned char)MIN(255,atoi( token ));
		}
		else
			Error("Unknown command '%s' when loading Cylinder!", token);
	}


}


