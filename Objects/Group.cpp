/******************************************************************/
/* Group.cpp                                                      */
/* -----------------------                                        */
/*                                                                */
/* The file defines an object container (i.e., an object that     */
/*     contains numerous others).  The intersection technique     */
/*     simply loops through all objects that have been added to   */
/*     the group, calls its Intersect() routine, and returns the  */
/*     closest object.  Obviously, this is NOT the most efficient */
/*     container class possible.                                  */
/*                                                                */
/* Chris Wyman (10/26/2006)                                       */
/******************************************************************/

#include "Group.h"
#include <assert.h>
#include "Materials/Material.h"
#include "Utils/ImageIO/imageIO.h"
#include "Scene/Scene.h"

Group::Group( Material *matl ) : Object(matl), 
	needsPreprocessing(false), needsFrameUpdates(false),
	groupXForm( Matrix4x4::Identity() )
{
}

Group::~Group()
{
}

void Group::Add(Object* obj)
{
  objs.Add(obj);
  if (obj->NeedsPreprocessing())
	  needsPreprocessing=true;
  if (obj->NeedPerFrameUpdates())
  {
	  needsFrameUpdates=true;
	  updatableObjs.Add(obj);
  }
}

Object *Group::Get( int i ) 
{
  return objs[i];
}

const Object* Group::Get(int i) const
{
    return objs[i];
}

void Group::Preprocess( Scene *s ) 
{ 
	needsPreprocessing=false; 
	for (unsigned int i=0;i<objs.Size();i++) 
		if (objs[i]->NeedsPreprocessing())
			objs[i]->Preprocess( s );
}

void Group::Update( float currentTime )
{
	for (unsigned int i=0;i<updatableObjs.Size();i++)
		updatableObjs[i]->Update( currentTime );
}

void Group::Draw( Scene *s, unsigned int matlFlags, unsigned int optionFlags, bool matlAlreadySpecified )
{
	bool matlSpecified = matlAlreadySpecified;
	if (!matlAlreadySpecified && matl)
	{
		matl->Enable( s, matlFlags );
		matlSpecified = true;
	}

	glPushMatrix();
	if (ball) ball->MultiplyTrackballMatrix();
	glMultMatrixf( groupXForm.GetDataPtr() );

	for (unsigned int i=0;i<objs.Size();i++) 
		objs[i]->Draw( s, matlFlags, optionFlags, matlSpecified ); 

	glPopMatrix();

	if (!matlAlreadySpecified && matl)
		matl->Disable();
}

// Draw this object (or it's sub-objects only if they have some property
void Group::DrawOnly( Scene *s, unsigned int propertyFlags, unsigned int matlFlags, unsigned int optionFlags, bool matlAlreadySpecified )
{
	bool meetsReqs       = ((propertyFlags & flags) == propertyFlags);
	bool someReqsNotMet  = ((propertyFlags & ~flags) > 0);
	unsigned int missingFlags = propertyFlags ^ (propertyFlags & flags);
	if (meetsReqs && !someReqsNotMet) 
		this->Draw( s, matlFlags, optionFlags, matlAlreadySpecified );
	else
	{
		bool matlSpecified = matlAlreadySpecified;
		if (!matlAlreadySpecified && matl)
		{
			matl->Enable( s, matlFlags );
			matlSpecified = true;
		}

		glPushMatrix();
		if (ball) ball->MultiplyTrackballMatrix();
		glMultMatrixf( groupXForm.GetDataPtr() );

		for (unsigned int i=0;i<objs.Size();i++) 
			objs[i]->DrawOnly( s, missingFlags, matlFlags, optionFlags, matlSpecified ); 

		glPopMatrix();

		if (!matlAlreadySpecified && matl)
			matl->Disable();
	}
}





Group::Group( FILE *f, Scene *s )
{
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
		else if (!strcmp(token,"object")) // Perhaps there are nested objects in the group?
		{
			char *ptr1 = StripLeadingTokenToBuffer( ptr, token );
			Object *obj = s->ExistingObjectFromFile( token );
			if (!obj)
				obj = s->LoadObject( ptr, f );
			if (obj) Add( obj );
		}
		else if (!strcmp(token, "matrix")) // Perhaps everything in the group should be transformed?
		{
			Matrix4x4 mat( f, ptr );
			groupXForm *= mat;
		}
		else if (!strcmp(token, "trackball"))
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			int id = atoi(token);
			ball = new Trackball( s->GetWidth(), s->GetHeight() );
			s->SetupObjectTrackball( id, ball );
		}
		else if (!strcmp(token, "update")) // The group's transform should be updated every frame!
		{
			needsFrameUpdates=true;
			//ptr = StripLeadingTokenToBuffer( ptr, token );
			//if (strcmp(token, "matrix"))
			//	printf("Unknown 'update' command '%s' in Instance::Instance()!\n", token);
			//else
			//{
			//	updateMatrix = Matrix4x4(f, ptr);
			//	needsPerFrameUpdates = true;
			//}
		}
		else
			Error("Unknown command '%s' when loading Group!", token);
	}

}

