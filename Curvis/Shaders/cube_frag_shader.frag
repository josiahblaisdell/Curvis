#version 400 compatibility 
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

out vec4 fragColor;

void main()
{ 
	vec3 uEyePos = vec3(0.0,0.0,0.0);
	float kD = .9;
	float kS = .25;
	float kA = 1.0;
	float shiny = .08;
	float diffuse = vNormalLightAngle;
	float specular = 0.0;
	float ambient = 1.0;
	vec3 fNormal  = normalize(vNormal);
	vec3 fReflect = normalize(-reflect(vLightPosition,n));
	vec3 v = normalize(-vMVPos);
	float angleReflectPos = max(0.0,dot(r,v));
	if( vNormalLightAngle != 0.0) specular = pow(angleReflectPos,shiny);
	float shading = kA*ambient + kD*diffuse + kS*specular;
	fragColor = vec4(shading);
	highp vec3 L = normalize(vLightPos - vWorldPos);
	highp vec3 V = normalize(uEyePos - vWorldPos);
	highp vec3 H = normalize(L+V);
    highp float diffuse = kD*max(0, dot(L,vWorldNormal));
	float specular = 0.0;
	if(dot(L,vWorldNormal) > 0.0){
		specular = kS*pow(max(0.0,dot(H,vWorldNormal)),3);
	}
	float edge = 0.0;
	if(dot(V,vWorldNormal) > 0.3){
		edge = 1.0;
	}
	float lightColor = edge*(diffuse + specular);
	fragColor = vec4(lightColor, lightColor, lightColor,1);
	fragColor = (.5*(vec4(1,1,1,1))*vec4(1.0))
	//fragColor = vec4(vWorldNormal,1);
	//fragColor = vec4(abs(fragColor.x), abs(fragColor.y), abs(fragColor.z),1.);
}
