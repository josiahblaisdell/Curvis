#version 400 compatibility 
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

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

vec2 encode_v2(in vec2 v_in)
{
  return (127.5 + 127.5 * (normalize(v_in)))/255.0;
}

vec3 encode_v3(in vec3 v_in)
{
  return (127.5 + 127.5 * (normalize(v_in)))/255.0;
}

void main()
{ 
	//project the directions to 2D
	vec2 major = encode_v2(vMajorCurvature.xy);
	vec2 minor = encode_v2(vMinorCurvature.xy);
	
	gl_FragColor = vec4(major.x, major.y, minor.x, minor.y);
}
