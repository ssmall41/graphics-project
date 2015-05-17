#version 120
#extension GL_EXT_geometry_shader4:enable

uniform vec4 SilCol;

void main()
{
	vec3 DirToTri = normalize(gl_PositionIn[0].xyz);
	vec3 TriNorm1 = normalize(gl_TexCoordIn[0][6].xyz);
	vec3 TriNorm2 = normalize(gl_TexCoordIn[0][7].xyz);

	if( dot(DirToTri,TriNorm1)*dot(DirToTri,TriNorm2) < 0 )
	{
		//gl_FrontColor = vec4(1,1,1,1);
		//gl_BackColor = vec4(1,1,1,1);
		gl_FrontColor = SilCol;
		gl_BackColor = SilCol;
		gl_Position = gl_ProjectionMatrix * gl_PositionIn[0];
		EmitVertex();
		gl_Position = gl_ProjectionMatrix * gl_PositionIn[1];
		EmitVertex();
		EndPrimitive();
	}

}
