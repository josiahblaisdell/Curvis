#version 400 compatibility 
in vec4 vColor;
in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec3 vLightPos;
uniform vec3 uEyePos;
out vec4 fragColor;

void main()
{ 
	float kD = .9;
	float kS = .25;
	float kA = 1.0;
	float shiny = .08;
	highp vec3 L = normalize(vLightPos - vWorldPos);
	highp vec3 V = normalize(uEyePos - vWorldPos);
	highp vec3 H = normalize(L+V);
    highp float diffuse = kD*max(0, dot(L,vWorldNormal));
	float specular = 0.0;
	if(dot(L,vWorldNormal) > 0.0){
		specular = kS*pow(max(0.0,dot(H,vWorldNormal)),shiny);
	}
	float edge = 0.0;
	if(dot(V,vWorldNormal) > 0.3){
		edge = 1.0;
	}
	float lightColor = edge*(diffuse + specular);
	//fragColor = vec4(lightColor, lightColor, lightColor,1);
	fragColor = vec4(vWorldNormal,1);
}
