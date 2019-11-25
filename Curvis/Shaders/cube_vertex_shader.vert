#version 400 compatibility 
layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec4 inColor;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;
//uniform vec3 uEyePos;
out vec4 vColor;
out vec3 vWorldPos;
out vec3 vWorldNormal;
out vec3 vLightPos;
void main(){
	vWorldPos = (uModelMatrix*inPosition).xyz;
	mat4 normalMat = transpose(inverse(uViewMatrix*uModelMatrix));
	vWorldNormal = normalize( mat3(normalMat)*inNormal );
	vColor = inColor;
	vLightPos = vec3(1,1,1);
	gl_Position = uProjection*uViewMatrix*uModelMatrix*inPosition;
	
}