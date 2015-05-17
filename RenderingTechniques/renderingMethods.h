/***************************************************************************/
/* renderingMethods.h                                                      */
/* -----------------------                                                 */
/*                                                                         */
/* Has prototypes and other data needed to use the various rendering       */
/*     techniques (i.e., DisplayCallback()'s) located in the .cpp files    */
/*     in this directory.                                                  */
/*                                                                         */
/* Chris Wyman (02/01/2008)                                                */
/***************************************************************************/


#ifndef __REDNERINGMETHODS_H__
#define __REDNERINGMETHODS_H__

// The data structure in "renderingData.h" stores various data needed
//    by the rendering techniques in this directory.  This function 
//    initializes all this data.
void InitializeRenderingData( void );					     // See: initializeRenderingData.cpp

// Utility routines used by multiple rendering techniques
float sqr( float f );                                             // See: utilRoutines.cpp
float log2( float x );										      // See: utilRoutines.cpp


#endif

