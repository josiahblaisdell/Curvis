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

uniform sampler2D uPattern;
uniform sampler2D uShading;
uniform sampler2D uField;

out vec4  vColor;
out vec3  vNormal;
out vec3  vMinorCurvature;
out vec3  vMajorCurvature;
out vec3  vMeanCurvature;
out float vGaussCurvature;

out float vDepth;
out vec3  vLightPos;
out vec4  vMVPosition;

void main(){
	vLightPos = vec3(10,10,10);
	vMVPosition = uViewMatrix*uModelMatrix*inPosition;
	gl_Position = uProjection*vMVPosition;
	gl_Position.y -= 0.2;
	vNormal = uNormalMatrix*inNormal;
	
	vMinorCurvature = uNormalMatrix*inMinorCurvature;
	vMajorCurvature = uNormalMatrix*inMajorCurvature;
	vMeanCurvature  = uNormalMatrix*inMeanCurvature;
	
	vGaussCurvature = inGaussCurvature;
	
	//Relative Depth of the object
	vec4 esObjOrigin = uViewMatrix * uModelMatrix * vec4(0,0,0,1);
	float distToCamera = -vMVPosition.z;
	float distToOrigin = -esObjOrigin.z;
	float originToVertexZ = distToOrigin - distToCamera;
	vDepth  = originToVertexZ;
	
	vColor = inColor;
}