
// The texture for this object, indexed by gl_TexCoord[0]
uniform sampler2D       wallTex;

// This is either 1 (for true) or 0 (for false)...  Should you use the shadow map?
uniform float			useShadowMap;

// If using the shadow map, here's where it is specified.
uniform sampler2DShadow shadowMap;

// Remember the shadow map matrix is passed down using automatic texture generation.
//   The automatic texture generation matrices are accessable using either
//   gl_EyePlane*[<texUnit>] or gl_ObjectPlane*[<texUnit>], where * is either
//   S, T, R, or Q, and <texUnit> should be 7 (unless you changed the code to bind
//   the shadow map elsewhere).  In this case, since you're doing computations in
//   eye space, gl_EyePlane*[7] should be correctly populated.


vec4 IsPointIlluminated( vec4 smapCoord )
{
	// if you plan on doing shadow mapping, you'll have to change this function...
	return vec4(1.0);
}


void main( void )
{
	// Get the direction between the current fragment position in eye space (passed down in
	//   gl_TexCoord[5]) and the eye-space light position.
	vec3 toLight = normalize( gl_LightSource[0].position.xyz - gl_TexCoord[5].xyz );
	
	// Get the eye-space surface normal at this point (passed down in gl_TexCoord[6])
	vec3 norm = normalize( gl_TexCoord[6].xyz );
	
	// Compute a diffuse color based on N dot L.
	float NdotL = max( 0.0, dot( norm, toLight ) );
	
	// Here you'll want to compute the shadow map coordinate, if you plan to do shadow 
	//   mapping...  This one is clearly not right!\
	// You can access the shadow map matrix using gl_EyePlaneS[7], gl_EyePlaneT[7], 
	//   gl_EyePlaneR[7], and gl_EyePlaneQ[7] unless you have changed the texture unit
	//   the shadow map is bound to (GL_TEXTURE7 currently).
	vec4 smapCoord = vec4(1.0);
	
	// Determine if this location is lit (look in shadow map, if using that).
	//    The trailing 0.05 is an ambient term.
	vec4 lit = ( useShadowMap>0 ? IsPointIlluminated( smapCoord )*NdotL : vec4(NdotL) ) + 0.05;
	
	// Get the texture color here at this fragment.
	vec4 color = texture2D( wallTex, gl_TexCoord[0].xy );
	
	// Output the result
	gl_FragColor = lit * color;
	gl_FragColor.a = 1.0;
	
}