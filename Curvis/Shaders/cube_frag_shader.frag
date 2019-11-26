#version 400 compatibility 

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

in vec4  vColor;
in vec3  vPos;
in vec3  vMPos;
in vec3  vMVPos;
in vec3  vMVPPos;
in vec3  vNormal;
in vec3  vMinorCurvature;
in vec3  vMajorCurvature;
in vec3  vMeanCurvature;
in float vGaussCurvature;
in vec3  vLightPos;
in float vDepth;
in float vNormalLightAngle;

out vec4 fragColor;

void main()
{ 
	vec3 cameraFacing = vec3(0.0,0.0,-1.0);
	vec3  uEyePos = vec3(0.0,0.0,0.0);
	float kD = .9;
	float kS = .9;
	float kA = 1.0;
	float shiny = .08;
	float diffuse = vNormalLightAngle;
	float specular = 0.0;
	float ambient = 1.0;
	vec3  fNormal  = normalize(vNormal);
	vec3  fReflect = normalize(-reflect(vLightPos,fNormal));
	vec3  v = normalize(-vMVPos);
	float angleReflectPos = max(0.0,dot(fReflect,fNormal));
	if( vNormalLightAngle != 0.0) specular = pow(angleReflectPos,shiny);
	float shading = .2*kA*ambient + .2*kD*diffuse + .2*kS*specular;
	shading = kD*diffuse;
	//if(abs(pow(2.0,1-1.0/dot(fNormal,normalize(vMVPos)))) < .7) fragColor = vec4(0.0,0.0,0.0,1.0);
	//else fragColor = vec4(1.0);
	//fragColor = vec4(fNormal,1);
	float nx = fNormal.x;
	float ny = fNormal.y;
	float nz = fNormal.z;
	
	nx = (floor(nx*(256/16))*16)/256;
	ny = (floor(ny*(256/16))*16)/256;
	nz = (floor(nz*(256/16))*16)/256;
	//fragColor = vec4(log(nx),log(ny),pow(16.0,nz),vMVPos.z);
	fragColor = vec4(vMinorCurvature,1.0);
}
