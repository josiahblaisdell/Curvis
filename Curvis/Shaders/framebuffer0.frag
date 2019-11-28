#version 400 compatibility 
in  vec2 vTexCoords;

uniform sampler2D uShading;
uniform sampler2D uField;

out vec4 gl_FragColor;

const float STEPSIZE  = 0.001; 
const float LEVEL     = 6.0; 
const float SIGMA  = 80; 
const float SIGMA2 = 0.25; 
const vec2 REPEAT = vec2(1, 1);

struct lic_t { 
    vec2 p; 
    vec2 t;
    float w;
    float dw;
};

vec2 sampleFeild(in int index, in vec2 uv) {
    vec2 v;
	if(index==0)
	{
	   v = texture2D(uField, uv).xy;
	}
	else
	{
	   v = texture2D(uField, uv).zw;
	}
	v = ((v*256.0)-127.5)/127.5;
    return v;
}

void step(in int field_index,inout lic_t s) {
    vec2 t = sampleFeild(field_index, s.p);
	
    if (dot(t, s.t) < 0.0) t = -t;
    s.t = t;
    
    s.dw = (abs(t.x) > abs(t.y))? 
        abs((fract(s.p.x) - 0.5 - sign(t.x)) / t.x): 
        abs((fract(s.p.y) - 0.5 - sign(t.y)) / t.y);
    
    s.p += t * STEPSIZE;
    s.w += s.dw;
}

void main()
{ 
	float shading 	= texture(uShading, vTexCoords).r;
	float outline 	= texture(uShading, vTexCoords).b;
	float mask 		= texture(uShading, vTexCoords).a;
	vec3 color    = vec3(1.0,1.0,1.0);
	if( mask == 1 )//hatching
	{
		
	}
	//color = mix(color, texture(uShading, vTexCoords).rrr, outline);
	color = mix(color, vec3(0), outline);
	gl_FragColor = vec4(color.xyz, mask);
}