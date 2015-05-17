#version 120

void main()
{
	gl_Position = gl_ModelViewMatrix*gl_Vertex;
	gl_TexCoord[6].xyz = gl_NormalMatrix * gl_MultiTexCoord6.xyz;
	gl_TexCoord[7].xyz = gl_NormalMatrix * gl_MultiTexCoord7.xyz;
}
