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

void main()
{ 
	gl_FragColor = vColor;
}
