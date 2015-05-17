//ReflectFrag.glsl
#version 120

uniform sampler2D inTex;
uniform int width;
uniform int height;
uniform bool gray;
uniform float reflectiveness;


void main(void)
{
	vec2 TexCoord;
	TexCoord.x = gl_FragCoord.x/float(width);
	TexCoord.y = gl_FragCoord.y/float(height);
	gl_FragColor = texture2D(inTex,TexCoord);

	if(gray)
		gl_FragColor = vec4( .3*gl_FragColor.r + .59*gl_FragColor.g + .11*gl_FragColor.b);
	else
		gl_FragColor = reflectiveness * gl_FragColor + (1.0-reflectiveness) * vec4(.5,.5,.5,1);
}
