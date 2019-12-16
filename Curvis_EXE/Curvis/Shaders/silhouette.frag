#version 400 compatibility 

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

uniform float   uKa = .0;
uniform float   uKd = .1;
uniform float   uKs = .0;				// coefficients of each type of lighting	
uniform float   uShininess = 4;		    // specular exponent

in vec4  vColor;
in vec3  vNormal;
in vec3  vMinorCurvature;
in vec3  vMajorCurvature;
in vec3  vMeanCurvature;
in float vGaussCurvature;
in vec3  vLightPos;
in vec4  vMVPosition;
in float vDepth;

out vec4 gl_FragColor;

vec2 Spherical(in vec3 v_in)
{
  v_in = normalize(v_in);
  vec2 v_out;
  v_out.x = atan(v_in.y,v_in.x);//phi
  v_out.y = acos(v_in.z);       //theta
  return v_out;
}

vec3 Cartesian(in vec2 v_in)
{
  vec3 v_out;
  v_out.x = sin(v_in.y)*cos(v_in.x);
  v_out.y = sin(v_in.y)*sin(v_in.x);
  v_out.z = cos(v_in.y);
  return v_out;
}

vec3 encode3(in vec3 v_in)
{
  return 0.5 + 0.5 * normalize(v_in);
}

void main()
{ 
	// normal vector
	float Kh = 0.5*length(vMinorCurvature);
	//vec2 sp = Spherical(vMinorCurvature);
	//float q = 50;
	//sp = floor(sp*q)/q;
	//vec3 ca = Cartesian(sp);
	//gl_FragColor = vec4(encode3(ca)*Kh, 1.0);
	
	vec3 c = encode3(vMinorCurvature);
	float q = 32;
	c = 0.5*c+0.5*(floor(c*q)/q)*Kh;
	gl_FragColor = vec4(c, 1.0);
}