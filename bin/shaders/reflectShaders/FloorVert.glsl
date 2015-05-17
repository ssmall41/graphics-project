//FloorVert.glsl

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	gl_TexCoord[7].xyz = gl_NormalMatrix * gl_Normal.xyz;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
