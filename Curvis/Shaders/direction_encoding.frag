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


vec2 encode2(in vec3 v_in)
{
  return 0.5 + 0.5 * normalize(v_in.xy);
}

vec2 encode3(in vec3 v_in)
{
  return 0.5 + 0.5 * normalize(v_in).xy;
}

void main()
{ 
	//project the directions to 2D
	vec2 major = encode2(vMajorCurvature);
	vec2 minor = encode2(vMinorCurvature);
	gl_FragColor = vec4(major.xy, minor.xy);
}
