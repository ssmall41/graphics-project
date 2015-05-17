//FloorFrag.glsl

uniform sampler2D texF;
uniform sampler2D texM;
uniform int width;
uniform int height;


void main()
{
	//Calculate N.V
	float NV=abs(dot( vec3(0,1,0) , normalize(gl_FragCoord.xyz) ));

	//Calculate Reflectance
	float reflect = 1.0 - pow(1.0 - NV , 5.0);

	vec2 TexCoord;
	TexCoord.x = gl_FragCoord.x/float(width);
	TexCoord.y = gl_FragCoord.y/float(height);

	vec4 floor_val = (1-.3*reflect)*texture2D(texF,gl_TexCoord[0].st);
	vec4 mirror_val = (.3*reflect)*texture2D(texM,TexCoord);

	gl_FragColor =  floor_val + mirror_val;
}
