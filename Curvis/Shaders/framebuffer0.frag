#version 400 compatibility 
in  vec2 vTexCoords;

uniform sampler2D uShading;
uniform sampler2D uField;

out vec4 gl_FragColor;

void main()
{ 
	float alpha = texture(uShading, vTexCoords).a;
    gl_FragColor = vec4(alpha,alpha,alpha,1.0);
}