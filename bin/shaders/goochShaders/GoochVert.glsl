//GoochVert.glsl

uniform vec3 LightPos;

void main()
{
	vec3 Normal = gl_NormalMatrix * gl_Normal;
	float dot = dot(LightPos,Normal);

	vec3 cool = vec3(0,0,.4) + .2 * gl_Color.xyz;
	vec3 warm = vec3(.4,.4,0) + .6 * gl_Color.xyz;

	gl_FrontColor.rgb = (1.0 - dot)/2.0 * warm + (1.0 + dot)/2.0 * cool;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
