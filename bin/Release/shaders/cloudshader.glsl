#define Scale vec3(.8, .8, .8)
#define K 19.19
const float PI = 3.141592;

const ivec2 POSITION = ivec2(1, 0);

const ivec2 VMOUSE = ivec2(1, 1);
vec3 LightPosition = vec3(0.0, -50.0, 0.0);
float metallic = 1.0;
float roughness = 1.0;
float udBox( vec3 p, vec3 b )
{
  return length(max(abs(p)-b,0.0));
}

float sdSphere( vec3 p, float s)
{
	return length(p)-s;
}

float udRoundBox( vec3 p, vec3 b, float r )
{
  return length(max(abs(p + vec3(.0, .5, .0))-b,.0))-r;
}

////////////////////////


vec3 hash(vec3 p3)
{
	p3 = fract(p3 * Scale);
    p3 += dot(p3, p3.yxz+19.19);
    return fract((p3.xxy + p3.yxx)*p3.zyx);

}

vec3 noise( in vec3 x )
{
    return texture(iChannel0, x*0.1).rgb;
}

const mat3 m = mat3( 0.00,  0.80,  0.60,
                    -0.80,  0.36, -0.48,
                    -0.60, -0.48,  0.64 );
vec3 fbm(in vec3 q)
{
            vec3 f  = 0.5000*noise( q ); q = m*q*2.01;
            f += 0.2500*noise( q ); q = m*q*2.02;
            f += 0.1250*noise( q ); q = m*q*2.03;
            f += 0.0625*noise( q ); q = m*q*2.04; 
            f += 0.03125*noise( q ); q = m*q*2.05; 
    return vec3(f);
}

////////////////

float hash13(vec3 p3)
{
	p3  = fract(p3 * .1031);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

float voronoi(in vec3 Pos, in float mult) {

    vec3 uv = Pos * mult;
    float dst = 10.;
    for(int i = -1; i <= 1; i++)
        for(int j = -1; j <= 1; j++)
        	for(int k = -1; k <= 1; k++){
            vec3 offset = vec3(i, j, k);
            float rand = hash13(floor(uv) + offset);
    		dst = min(dst, length(fract(uv) - offset - rand));
        }
    
    return smoothstep(0.2, 0.8, 1.0-dst);
}
//468.47
float getDensity(in vec3 p) {
    float f = pow( fbm(p*vec3(0.1)+ vec3(iTime*0.1, 0.0, iTime*0.1)).x , 3.0 );
    f = clamp(f, 0.01, 0.2);
    f = smoothstep(0.1, 0.99, f)*1.5;
    float v = voronoi(p + vec3(iTime, iTime, iTime) * vec3(0.5, 0.5, 0.5),0.1);
    return clamp(v*f, 0.0, 1.0);
}

float mapVolume(in vec3 p) {
    vec3 q = vec3(0.0, -25.0, -20.0);
    float sphere = sdSphere(p+q, (10.0));
    
    float box5 = udBox(p+q, vec3(50.0, 2.0, 50.0));
    
    float box2 = udRoundBox(p+q, vec3(10.0, 2.0, 10.0), 0.2);
    
    return box5;
}


float lightMarch(in vec3 p, in float stepSize) {
    //Lets march towards the light
    vec3 lv = -LightPosition - p;
    vec3 ld = -normalize(lv)*5.0;
    ld = normalize(vec3(0.0, 1.0, 0))*2.0;
    float laccum = 0.0;
    float h = -1.0;
    float t = 0.0;
    vec3 pos = p+ld*stepSize;
    for(int i = 0; i < 50; i++) {
        t += stepSize;
        pos = p+ld*t;
        h = mapVolume(pos);
        if(h > 0.0) {
            break;
        }
        laccum += getDensity(pos);
    }
    return laccum;
}

float intersectVolume(in vec3 ro, in vec3 rd, in float stepSize, out float tr, out vec3 e) {
    const int MAX_STEPS_OUTSIDE = 100;
    const int MAX_STEPS_INSIDE = 200;
    //Get to the outer part of the volume
    const float maxd = 150.0;
	float h          = 1.0;
    float t          = 0.0;
    for( int i=0; i<MAX_STEPS_OUTSIDE; i++ )
    {
        if( h<0.001 || t>maxd ) break;
	    h = mapVolume( ro+rd*t);
        t += h;
    }

    if( t>maxd ) { 
        t=-1.0;
        return t;
    }
    
    float newT = t;
    float oldT = t;
    t = 0.0;
    //float D = 0.0;
    
    int steps = 0;
    //Now we have the distance to our volume, let's go inside
    if(h<0.0)
        ro = ro;
    else
        ro = ro+rd*newT;
        
    h = -1.0;
        
    vec3 LightEnergy = vec3(0.0);
    float LightTransmittance = 0.0;
    float Transmittance = 1.0;
    float Distance = 0.0;
    float Density = 0.0;
    float Absorption = 4.5;
    float Phase = 1.5;
     
    while(true) {
        if(steps >= MAX_STEPS_INSIDE) return oldT;
        t += stepSize + (texture(iChannel2, ro.xy).x*0.01);
        vec3 pos = ro+rd*t;
	    h = mapVolume(pos);
        
        if(h > 0.00) break;
        
        Density += getDensity(pos);
        
        if(Density > 0.0) {
            stepSize = stepSize;
            LightTransmittance = lightMarch(pos, stepSize);
            LightEnergy -= Density * stepSize * Transmittance * LightTransmittance * Phase;
            Transmittance *= exp(-Density * stepSize * Absorption);
            
            if(Transmittance < 0.01) {
                break;
            }
        }
        steps++;
        
        /**/
        float shadowdist = 0.0;

        vec3 lpos = normalize(vec3(0.0, 1.0, 0))*10.0;
        lpos += clamp(vec3(0.0,0.05, 0.0), 0.0, 1.0);
        float lsample = getDensity(lpos);
        shadowdist += lsample;
        lpos += vec3(0.0,0.1, 0.0);
        lsample = getDensity(lpos);
        shadowdist += lsample;
        lpos += vec3(0.0,0.2, 0.0);
        lsample = getDensity(lpos);
        shadowdist += lsample;

        vec3 SkyColor = vec3( 0.45, 0.68, 0.88 )*0.78;
        
        shadowdist /= 3.0;

        //shadowterm = exp(-shadowdist * AmbientDensity);
        //absorbedlight = exp(-shadowdist * AmbientDensity) * curdensity;
        LightEnergy += exp(-shadowdist *1.0) * Density * SkyColor * Transmittance;


        /**/
    }
    
    float l = -lightMarch(ro+rd*t, stepSize);
    vec3 pos = ro+rd*t;
    Density = getDensity(pos);
    
    //LightEnergy *= mix(LightEnergy, vec3( 0.45, 0.68, 0.88 )*3.0, 1.0-LightEnergy);//Density * vec3( 0.45, 0.68, 0.88 ) * 0.1;
    
    
    
    e = LightEnergy;
    tr = Transmittance;
    //D
    return oldT;
    
}


#define load(P) texelFetch(iChannel1, ivec2(P), 0)

void Camera(in vec2 fragCoord, out vec3 ro, out vec3 rd) 
{
    ro = load(POSITION).xyz;
    vec2 m = load(VMOUSE).xy/iResolution.x;
    
    float a = 1.0/max(iResolution.x, iResolution.y);
    rd = normalize(vec3((fragCoord - iResolution.xy*0.5)*a, 0.5));
    
    rd = CameraRotation(m) * rd;
}


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    
	vec2 uv = fragCoord.xy / iResolution.xy;
    
    vec2 q = uv * 2.0 - 1.0;
	q.x *= iResolution.x/iResolution.y;

	vec3 ro = vec3(0.0, 0.0, -20.0 );
    vec3 rd = normalize( vec3(q,1.0) );
    
    Camera(fragCoord, ro, rd);
	vec3 col = vec3(0.0);
       
    //LightPosition.y = sin(iTime*0.3) * -30.0;
    
    vec3 backcol = mix(vec3( 0.92, 0.94, 0.95 ), vec3( 0.45, 0.68, 0.88 ), vec3(uv.y));
    
    float tr = 0.0;
    vec3 e = vec3(0.0);
    float t2 = intersectVolume(ro,rd, 0.2, tr, e);
    col = backcol;
    //if( t2>0.0   )
    {
        e = clamp(e, 0.0, 1.0);
        vec3 cloudCol = (vec3(e) * (vec3(0.9, 0.6, 0.45)*3.0));//vec3( 0.98, 0.95, 0.87 )) ;
        col = backcol * (tr + abs(min(0.0,t2))) + cloudCol;//vec3(exp(-l));//clamp(vec3(l/10.0), 0.0, 1.0);
        //col = vec3(v);
    }
    
    fragColor = vec4(col, 1.0 );
}