#version 400 compatibility 
layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec4 inColor;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix

varying vec4  vColor;
varying vec3  vPos;
varying vec3  vMPos;
varying vec3  vMVPos;
varying vec3  vMVPPos;
varying vec3  vNormal;
varying vec3  vLightPos;
varying float vDepth;
varying float vNormalLightAngle;

void main(){
	vLightPos = vec3(1,1,1);
	vec3 posInc = inNormal;
	vPos    = (inPosition+vec4(posInc,0.0)).xyz;
	vMPos   = (uModelMatrix*(inPosition+vec4(posInc,0.0))).xyz;
	vMVPos  = (uViewMatrix*uModelMatrix*(inPosition+vec4(posInc,0.0))).xyz;
	vMVPPos = (uProjection*uViewMatrix*uModelMatrix*(inPosition+vec4(posInc,0.0))).xyz;
	vNormal = uNormalMatrix*inNormal;
	vDepth  = (length(vPos)/90.0);
	vDepth  = .5 + .5*vDepth;
	vNormalLightAngle = max(0.0,dot(vNormal,normalize(vLightPos)));
	vColor = inColor;

	gl_Position = uProjection*uViewMatrix*uModelMatrix*inPosition;
	
}