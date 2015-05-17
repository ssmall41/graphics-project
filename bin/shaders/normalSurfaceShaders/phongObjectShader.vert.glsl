

void main( void )
{
	// Find out the vertex position and normal in eye-space
	vec4 eyePos = gl_ModelViewMatrix * gl_Vertex;
	vec3 eyeNorm = gl_NormalMatrix * gl_Normal;
	
	// Pass the eye-space position and normal to the frag shader
	gl_TexCoord[5] = eyePos;
	gl_TexCoord[6].xyz = eyeNorm;
	
	// Output the clip-space position of the vertex
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}