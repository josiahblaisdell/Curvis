#version 400 compatibility 

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

uniform float   uKa = .0;
uniform float   uKd = .2;
uniform float   uKs = .6;				// coefficients of each type of lighting	
uniform float   uShininess = 4;		    // specular exponent

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

float ADSLighting(vec3 vN, vec3 vL, vec3 vE, vec3 vR, vec3 vRE)
{
	float ambient  = uKa;
	float diffuse  = max(0.0, -dot(vL,vN))*uKd;
	float specular = 0.0;
	if(dot(vL,vE)  > 0.0)
	{
		specular = pow(max(0.0, -dot(vE,vR) ),1/(1+uShininess))*uKs;
	}
	return clamp(ambient+diffuse+specular,0.0,1.0);
}

void main()
{ 
	// normal vector
	vec3 fN = vNormal;
	// vector from point to light
	vec3 vL = normalize(vMVPosition.xyz - vec3(vLightPos));	
	// vector from point to eye								
	vec3 vE = normalize(vMVPosition.xyz - vec3( 0., 0., 0. ));	
	// vector from point to light reflected about normal
	vec3 vR = normalize(reflect(vL,fN));
	// vector from point to eye reflected about the normal;
	vec3 vRE = normalize(reflect(-vE,fN));
	// 
	float shading = ADSLighting(fN, vL, vE, vR, vRE);
	float depth   = clamp(vDepth,-1,1)*0.5+0.5;
	float outline = abs(dot(vNormal, normalize(vMVPosition.xyz)));
	if(   outline < 0.2) {outline = 0.0;}
	else{ outline = 1.0; }
	gl_FragColor  = vec4(shading, depth, outline, 1);
}
