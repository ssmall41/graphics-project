//BumpVert.glsl

uniform sampler2D intex;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	gl_TexCoord[1] = gl_MultiTexCoord1;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
