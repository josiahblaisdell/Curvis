#version 400 compatibility 
layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec4 inColor;
layout (location = 3) in vec3 inMinorCurvature;
layout (location = 4) in vec3 inMajorCurvature;
layout (location = 5) in vec3 inMeanCurvature;
layout (location = 6) in float inGaussCurvature;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

out vec4  vColor;
out vec3  vPos;
out vec3  vMPos;
out vec3  vMVPos;
out vec3  vMVPPos;
out vec3  vNormal;
out vec3  vLightPos;
out vec3  vMinorCurvature;
out vec3  vMajorCurvature;
out vec3  vMeanCurvature;
out float vGaussCurvature;
out float vDepth;
out float vNormalLightAngle;

void main(){
	vLightPos = vec3(1,1,1);
	vec3 posInc = inNormal;
	vPos    = (inPosition+vec4(posInc,0.0)).xyz;
	vMPos   = (uModelMatrix*(inPosition+vec4(posInc,0.0))).xyz;
	vMVPos  = (uViewMatrix*uModelMatrix*(inPosition+vec4(posInc,0.0))).xyz;
	vMVPPos = (uProjection*uViewMatrix*uModelMatrix*(inPosition+vec4(posInc,0.0))).xyz;
	vNormal = uNormalMatrix*inNormal;
	vMinorCurvature = uNormalMatrix*inMinorCurvature;
	vMajorCurvature = uNormalMatrix*inMajorCurvature;
	vMeanCurvature  = uNormalMatrix*inMeanCurvature;
	
	vDepth  = (length(vPos)/90.0);
	vDepth  = .5 + .5*vDepth;
	vNormalLightAngle = max(0.0,dot(vNormal,normalize(vLightPos)));
	vColor = inColor;

	gl_Position = uProjection*uViewMatrix*uModelMatrix*inPosition;
	
}