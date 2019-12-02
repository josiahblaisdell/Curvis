#version 400 compatibility 
in  vec2 vTexCoords;

uniform vec2 img_size;

uniform sampler2D uPattern;
uniform sampler2D uShading;
uniform sampler2D uField;
uniform sampler2D uSilhouette;

out vec4 gl_FragColor;

const float STEPSIZE  = 0.001; 
const float LEVEL     = 6.0; 
const float SIGMA  = 30; 
const float SIGMA2 = 0.25; 
const vec2 REPEAT  = vec2(3, 3);

const float sigma_e = 0.5;
const float sigma_r = 3.0;
const float tau = 0.98;

const float sigma2_e = 1.0;
const float sigma2_r = 1.2;
const float tau2 = 0.99;

const float phi = 3.415;

struct lic_t { 
    vec2 p; 
    vec2 t;
    float w;
    float dw;
	float d;
};

vec2 sampleFeild(in int index, in vec2 uv) {
    vec2 v;
	if(index==0)
	{  v = texture2D(uField, uv).xy; }
	else
	{  v = texture2D(uField, uv).zw; }
    return normalize((v*2)-1);
}

float sampleNoise(in vec2 uv) {
	vec2 repeated_uv = vec2(mod(uv.x * REPEAT.x, 1), mod(uv.y * REPEAT.y, 1));
    return texture2D(uPattern, repeated_uv).w;
}

float samplePattern(in float intensty, in vec2 uv) {
    float color = 0;
	vec2 repeated_uv = vec2(mod(uv.x * REPEAT.x, 1), mod(uv.y * REPEAT.y, 1));
    if(intensty*LEVEL<=5)
	{ 
	  color += texture2D(uPattern, repeated_uv).x;
	}
	if(intensty*LEVEL<=4)
	{ 
	  color += texture2D(uPattern, repeated_uv).y;
	}
	if(intensty*LEVEL<=3)
	{ 
	  color += texture2D(uPattern, repeated_uv).z;
	}
    return clamp(color,0.0,1.0);
}

int step(in int field_index,inout lic_t s) {
    vec2  t = sampleFeild(field_index, s.p);
	float d = texture(uShading, vTexCoords).g;
	float mask 		= texture(uShading, vTexCoords).a;
	if(abs(s.d - d)<0.05 && length(t)>0 && mask==1)
	{
		if (dot(t, s.t) < 0.0) t = -t;
		s.t = t;
		
		s.dw = (abs(t.x) > abs(t.y))? 
			abs((fract(s.p.x) - 0.5 - sign(t.x)) / t.x): 
			abs((fract(s.p.y) - 0.5 - sign(t.y)) / t.y);
		
		s.p += t / img_size;
		s.w += s.dw;
		return 1;
	}
	return 0;
}

float lic(in int field_index)
{
	float twoSigma2 = 2.0 * SIGMA * SIGMA;
    float halfWidth = 2.0 * SIGMA;

	float c = sampleNoise(vTexCoords);
    float w = 1.0;
	lic_t a, b;
    a.p = b.p = vTexCoords;
    a.t = sampleFeild(field_index, vTexCoords);
    b.t = -a.t;
	a.d = b.d = texture(uShading, vTexCoords).g;
    a.w = b.w = 0.0;
	
	int flag = 1;
	while (a.w < halfWidth && flag==1) {
        flag = step(field_index, a);
        float k = a.dw * exp(-a.w * a.w / twoSigma2);
        c += k * sampleNoise(a.p);
        w += k;
    }

	flag = 1;
	while (b.w < halfWidth && flag==1) {
        step(field_index, b);
        float k = b.dw * exp(-b.w * b.w / twoSigma2);
        c += k * sampleNoise(b.p);
        w += k;
    }
	
    c /= w;
	
	return c;
}

float hatch(in float intensty, in int field_index)
{
	float twoSigma2 = 2.0 * SIGMA * SIGMA;
    float halfWidth = 2.0 * SIGMA;

    float c = samplePattern(intensty, vTexCoords);
	lic_t a, b;
    a.p = b.p = vTexCoords;
    a.t = sampleFeild(field_index, vTexCoords);
    b.t = -a.t;
	a.d = b.d = texture(uShading, vTexCoords).g;
    a.w = b.w = 0.0;
	
	int flag = 1;
	while (a.w < halfWidth && flag==1) {
        flag = step(field_index, a);
        float k = a.dw * exp(-a.w * a.w / twoSigma2);
        c += k * samplePattern(intensty, a.p);
    }

	flag = 1;
	while (b.w < halfWidth && flag==1) {
        flag = step(field_index, b);
        float k = b.dw * exp(-b.w * b.w / twoSigma2);
        c += k * samplePattern(intensty, b.p);
    }
	
	c = exp(-c*c/SIGMA2);
	
	return c;
}

float SilhouetteDoG()
{
	float twoSigmaESquared = 2.0 * sigma_e * sigma_e;
    float twoSigmaRSquared = 2.0 * sigma_r * sigma_r;
    int halfWidth = int(ceil( 2.0 * sigma_r ));

    vec2 sum  = vec2(0.0);
    vec2 norm = vec2(0.0);
    for ( int i = -halfWidth; i <= halfWidth; ++i ) {
        for ( int j = -halfWidth; j <= halfWidth; ++j ) {
            float d = length(vec2(i,j));
            vec2 kernel = vec2( exp( -d * d / twoSigmaESquared ), 
                                exp( -d * d / twoSigmaRSquared ));
            vec3  c = texture(uSilhouette, vTexCoords + vec2(i,j) / img_size).rgb;
			float a = texture(uShading, vTexCoords + vec2(i,j) / img_size).a;
			float gray = (c.x+c.y+c.z)/3;
            norm += 2.0  * kernel;
            sum  += gray * kernel;
        }
    }
	sum /= norm;
	float H = 100.0 * (sum.x - tau * sum.y);
	float  edge = ( H > 0.0 )? 1.0 : smoothstep(-2.0, 2.0, H*15 );
	return edge;
}

float OutlineDoG()
{
	float twoSigmaESquared = 2.0 * sigma2_e * sigma2_e;
    float twoSigmaRSquared = 2.0 * sigma2_r * sigma2_r;
    int halfWidth = int(ceil( 2.0 * sigma2_r ));

    vec2 sum  = vec2(0.0);
    vec2 norm = vec2(0.0);
    for ( int i = -halfWidth; i <= halfWidth; ++i ) {
        for ( int j = -halfWidth; j <= halfWidth; ++j ) {
            float d = length(vec2(i,j));
            vec2 kernel = vec2( exp( -d * d / twoSigmaESquared ), 
                                exp( -d * d / twoSigmaRSquared ));
            float  c = texture(uShading, vTexCoords + vec2(i,j) / img_size).a;
            norm += 2.0  * kernel;
            sum  += c    * kernel;
        }
    }
	sum /= norm;
	float H = 100.0 * (sum.x - tau2 * sum.y);
	float  edge = ( H > 0.0 )? 1.0 : 2.0 * smoothstep(-2.0, 2.0, phi * H );
	return edge;
}

void main()
{ 
	
	float mask 		= texture(uShading, vTexCoords).a;
	float shading 	= texture(uShading, vTexCoords).r;
	float outline 	= texture(uShading, vTexCoords).b;
	float edge      = mix(1,min(SilhouetteDoG(),outline),mask);
	float contour 	= OutlineDoG();
	float color     = min(edge, contour);
	if( mask == 1 )//hatching
	{
		float H1 = hatch(shading, 0);
		float H2 = hatch(shading+0.5, 1);
		float H = 1-clamp((1-H1)+(1-H2)/2,0,1);
		color = min(0.01+H*0.99, color);
		
		//float L1 = lic(0);
		//float L2 = lic(1);
		//color = (L1+L2)*0.5*clamp(0.9+shading,0,1);
	}
	//gl_FragColor = vec4(mask,mask,mask, 1);
	gl_FragColor = vec4(color,color,color, 1);
	//gl_FragColor = texture(uShading, vTexCoords).xxxx;
	//gl_FragColor = vec4(texture(uSilhouette, vTexCoords).xyz,1);
	//gl_FragColor = vec4(outline ,outline ,outline ,1.0);
	//gl_FragColor = vec4(img_size.x, 0, 0, 1);
}