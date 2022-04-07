#version 450 core

const float PI = 3.14159265359;

out vec4 FragColor;
out vec4 BrightFragColor;

layout (binding = 0) uniform sampler2D texture_diffuse;
layout (binding = 1) uniform sampler2D texture_specular;
layout (binding = 2) uniform sampler2D texture_normal;
layout (binding = 3) uniform sampler2D texture_height;
layout (binding = 4) uniform samplerCube skybox;
layout (binding = 5) uniform sampler2D blurredTexture;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    noperspective mat3 TBN;
    mat3 TBN2;
    vec3 wFragPos;
    vec3 vertexPos;
} fs_in;

in mat4 invViewMatrix;

uniform int using_color_diffuse = 0;
uniform int using_color_specular = 0;
uniform int using_color_normal = 0;
uniform int using_color_height = 0;

uniform vec4 color_diffuse = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 color_specular;
uniform vec4 color_normal;
uniform vec4 color_height;

uniform float roughness;

uniform vec3 viewPos;
//Inputs from previous shader stage, mostly used for motion blur
in vec4 PrevPosition;
in vec4 Position;
in flat int varCurrEnt;

uniform int resX;
uniform int resY;

uniform float alphaBlend = 1.0;


#define Scale vec3(0.3, 0.1, 0.1)
#define K 19.19

uniform float time;


vec3 hash(vec3 p3)
{
	p3 = fract(p3 * Scale);
    p3 += dot(p3, p3.yxz+19.19);
    return fract((p3.xxy + p3.yxx)*p3.zyx);

}

vec3 noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
	
    return mix(mix(mix( hash(p+vec3(0,0,0)), 
                        hash(p+vec3(1,0,0)),f.x),
                   mix( hash(p+vec3(0,1,0)), 
                        hash(p+vec3(1,1,0)),f.x),f.y),
               mix(mix( hash(p+vec3(0,0,1)), 
                        hash(p+vec3(1,0,1)),f.x),
                   mix( hash(p+vec3(0,1,1)), 
                        hash(p+vec3(1,1,1)),f.x),f.y),f.z);
}

const mat3 m = mat3( 0.00,  0.80,  0.60,
                    -0.80,  0.36, -0.48,
                    -0.60, -0.48,  0.64 );
vec3 fbm(in vec3 q)
{
            vec3    f  = 0.5000*noise( q ); q = m*q*2.01;
                    f += 0.2500*noise( q ); q = m*q*2.02;
                    f += 0.1250*noise( q ); q = m*q*2.03;
                    f += 0.0625*noise( q ); q = m*q*2.04;  
                    f += 0.03125*noise( q ); q = m*q*2.05; 
                    //f += 0.015625*noise( q ); q = m*q*2.06; 
    return vec3(f);
}


  vec4 DiffuseTex = vec4(vec3(0.0), 1.0); 
  vec2 MetallicHeight = vec2(0.0);
  vec3 normal_tex = vec3(0.0);
  vec3 r_tex = vec3(0.0);

struct t_light {

        vec4 position; // 4/*
        vec4 direction; // 8
        vec4 color; // 12
        float radius; float padding0[3]; // 13
        float watts;  float padding1[3]; // 14
        int type;     float padding2[3];//15*/
    };

layout(std430, binding = 0) buffer ssbo
 {
    t_light Lights[];

 } outBuffer;


float fresnel(float r0, float theta, float roughness) {
    return r0 + (max(1.0 - roughness, r0) - r0)  * pow(1.0 - cos(theta), 0.2);
}


float DistributionGGX2(in vec3 N, in vec3 H, in float roughness, in vec3 L, in float rad)
{
    
    float a      = roughness*roughness;
    float a2     = a*a;
    float aP     = clamp( rad / ( length(L) * 2.0 ) + a, 0.0, 1.0 );
    float aP2    = aP * aP;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom = a2 * aP2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
  
    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
  
    return ggx1 * ggx2;
}

float calculateAttenuation(float power, float distance)
{
    return power / (4.0f * PI * (distance*distance));
}
vec3 Normal;
vec3 SphereAreaLight(in vec3 position, in float radius, in vec3 color, in float power, in vec3 N, in bool uses_clearcoat = false)
{
    vec3 I;
    vec3 L, centerToRay, r, closestPoint;
    L = (position - fs_in.wFragPos);
    vec3 V = normalize(viewPos - fs_in.wFragPos);
    r = normalize(reflect(V, N));
    centerToRay = dot(L, r) * r - L;
    closestPoint = L + centerToRay*clamp(radius/length(centerToRay), 0.0, 1.0);
    vec3 l = normalize(closestPoint);

        float spec_copy = r_tex.x;
        L = l;//normalize(closestPoint - FragPos);
        vec3 H = normalize(V + L);
        
        vec3 F0 = vec3(0.04);
        F0 = mix(F0, DiffuseTex.rgb, MetallicHeight.x);
        vec3 F = vec3(fresnel(max(dot(H, V), 0.0), F0.x, spec_copy));
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= clamp(1.0 - MetallicHeight.x, 0.0, 1.0);
        //float distance    = length(pos - FragPos);
        float distance = length(closestPoint);
        float watts = power;
        float attenuation = calculateAttenuation(watts, distance);//1.0 / (1.0 + 0.1/*factor*/ * pow(distance, 2));
        attenuation *= smoothstep(watts, watts - 5.0, distance);
        vec3 radiance     = vec3(1.0) * attenuation;        
        
        // cook-torrance brdf
        float NDF = DistributionGGX2(N, H, clamp(spec_copy, 0.05, 1.0), L, radius);       
        float G   = GeometrySmith(N, V, L, clamp(spec_copy, 0.05, 1.0));      
        
        vec3 nominator    = NDF * G * F;
        float denominator = (4 * max(dot(V, N), 0.0) * max(dot(L, N), 0.0)) + 0.001; 
        vec3 brdf = (nominator / denominator);
            
        // add to outgoing radiance Lo
        float NdotL = dot(N, L) * 0.5+0.5;//orenNayarDiffuse(L, V, Normal, clamp(spec_copy, 0.05, 1.0), 1.0);             
        vec3 Lo = ((kD * DiffuseTex.rgb / PI + brdf)) * radiance * NdotL; 
        
        return Lo * vec3(color);
        
}


float udRoundBox( vec3 p, vec3 b, float r )
{
  return length(max(abs(p)-b,.0))-r;
}

float sdRoundedCylinder( vec3 p, float ra, float rb, float h )
{
  vec2 d = vec2( length(p.xz)-2.0*ra+rb, abs(p.y) - h );
  return min(max(d.x,d.y),0.0) + length(max(d,0.0)) - rb;
}

uniform mat4 PrevModel;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 bottlePosition;

vec2 map(vec3 q) {
    float mainLiquid = sdRoundedCylinder(vec4(vec4(q, 1.0) * model).xyz + bottlePosition, 0.435, 0.1, 3.0);
    float surfTension = udRoundBox(q + bottlePosition- vec3(0.0, 6.5, 0.0), vec3(5.0), 1.0);//sdRoundedCylinder(q + bottlePosition - vec3(0.0, 5.5, 0.0), 0.51, 0.2, 6.0);
    return vec2(max(mainLiquid, -surfTension), (mainLiquid + surfTension) );
}

vec2 intersect(in vec3 ro, in vec3 rd, out int steps)
{
    float maxD = 200.0;
    const int iterations = 150;
    vec2 h = vec2(1.0);
    float t = 0.0;
	int j;
    for(int i = 0; i < iterations; i++)
    {
        j = i;
        if(h.x < 0.002 || t > maxD)
            break;
        h = map(ro+rd*t);
        t += h.x;
    }
    if( t>maxD ) t=-1.0;
    steps = j;
    return vec2(t, h.y);
    
}

vec2 drawscene(vec3 ro, vec3 rd) 
{
    int steps;
    vec2 t = intersect(ro, rd, steps);
    
    return t;
}

vec3 getNormal( in vec3 pos )
{
    vec3 eps = vec3(0.002,0.0,0.0);
    
	return normalize( vec3(
           map(pos+eps.xyy).x - map(pos-eps.xyy).x,
           map(pos+eps.yxy).x - map(pos-eps.yxy).x,
           map(pos+eps.yyx).x - map(pos-eps.yyx).x));
}

uniform vec3 bottleSpeed;
void main() {
    
  
  if(using_color_diffuse == 0) DiffuseTex = texture(texture_diffuse, fs_in.TexCoords);
  else DiffuseTex = color_diffuse;
  
  if(using_color_specular == 0) r_tex = texture(texture_specular, fs_in.TexCoords).xyz;
  else r_tex = color_specular.xyz;
  
  if(using_color_normal == 0) normal_tex = texture(texture_normal, fs_in.TexCoords).xyz;
  else normal_tex = color_normal.xyz;
  
  if(using_color_height == 0) MetallicHeight = texture(texture_height, fs_in.TexCoords).xy;
  else MetallicHeight = color_height.xy;


    vec3 speed = clamp(bottleSpeed, 0.0, 2.0);
    float bottleVelocity = clamp(length(bottleSpeed), 0.0, 1.0);
    float xDisp = (bottlePosition - fs_in.wFragPos).x + speed.x + time*7.0 - mix(0.01, 0.1, 1.0 - bottleVelocity);
    float yDisp = (bottlePosition - fs_in.wFragPos).y + speed.y + time*7.0 - mix(0.01, 0.1, 1.0 - bottleVelocity);
    float zDisp = (bottlePosition - fs_in.wFragPos).z + speed.z + time*7.0 - mix(0.01, 0.1, 1.0 - bottleVelocity);

    const float WATER_HEIGHT = (sin(xDisp.x + yDisp + zDisp) * clamp(length(speed)*0.05, 0.0, 0.20)) - 0.0;//Position - PrevPosition;

    const int MAX_MIPS = 3;

    vec2 iResolution = vec2(resX, resY);
    vec2 ndc = gl_FragCoord.xy/iResolution;
    vec2 uv = ndc*iResolution + 0.5;
    vec2 iuv = floor( uv );
    vec2 fuv = fract( uv );
    uv = iuv + fuv*fuv*(3.0-2.0*fuv); // fuv*fuv*fuv*(fuv*(fuv*6.0-15.0)+10.0);
    uv = (uv - 0.5)/iResolution;

    vec2 coords = vec2(gl_FragCoord.x / iResolution.x, gl_FragCoord.y/iResolution.y);

    float r = r_tex.r;

    Normal = normalize( normalize( normal_tex * 2.0 - 1.0) * fs_in.TBN);

    int negator = ( int(gl_FrontFacing) * 2 - 1 );
    Normal = negator * Normal;
    
    vec3 light = vec3(0.0);
    for(int i = 0 ; i < outBuffer.Lights.length() ; i++)
    {
        if(outBuffer.Lights[i].type == 2)
            light += SphereAreaLight(outBuffer.Lights[i].position.rgb, outBuffer.Lights[i].radius, outBuffer.Lights[i].color.rgb, outBuffer.Lights[i].watts, Normal);
    }

    vec2 dudv = vec2(0.0);

    Normal = -Normal;
    if(using_color_normal == 0) {
        vec2 tex_offset = 1.0 / textureSize(texture_normal, 0);
        vec3 up = normalize((texture(texture_normal, fs_in.TexCoords + vec2(0.0, tex_offset.y)).xyz * 2.0 - 1.0));
        vec3 down = normalize((texture(texture_normal, fs_in.TexCoords - vec2(0.0, tex_offset.y)).xyz * 2.0 - 1.0));
        vec3 right = normalize((texture(texture_normal, fs_in.TexCoords + vec2(tex_offset.x, 0.0)).xyz * 2.0 - 1.0));
        vec3 left = normalize((texture(texture_normal, fs_in.TexCoords - vec2(tex_offset.x, 0.0)).xyz * 2.0 - 1.0));

        dudv = vec2((up.y - down.y) / 2.0, (right.x - left.x) / 2.0);
    }
    
    vec3 V = normalize(fs_in.wFragPos - viewPos);
    vec3 VV = normalize(fs_in.FragPos - viewPos);
    
	vec2 q = coords * 2. - 1.;
	q.x *= iResolution.x/iResolution.y;

	vec3 ro = vec3(viewPos);
    vec3 rd = normalize(fs_in.wFragPos - viewPos);
    //vec2 t = drawscene(ro, rd);

    vec4 lTransparency = vec4(0.0);
    
    vec3 refr = refract(Normal * mat3(invViewMatrix), VV, 1.0 / 1.52).rgb * 0.5;

    vec4 transparency = vec4(0.0);
    float surfTension = 0.0;
    vec4 liquid_color = vec4(bottleSpeed, 0.0);
    vec3 lNormal = -Normal;
    float blend = alphaBlend;
    liquid_color = vec4(vec3(0.0), 1.0);
    vec3 topWaterColor = vec3(0.0);
    float foam = 0.;
        transparency = vec4(textureLod(blurredTexture, coords + dudv + (refr.xy * 0.1), r * MAX_MIPS).rgb, 1.0);

    if(fs_in.wFragPos.y - bottlePosition.y < WATER_HEIGHT) {
        liquid_color = pow(vec4(255.0 / 255.0, 160.0 / 255.0, 18.0 / 255.0, 1.0), vec4(1.0/2.2));
        liquid_color = pow(vec4(255.0 / 255.0, 160.0 / 255.0, 18.0 / 255.0, 1.0), vec4(1.0/2.2));
        vec4 foam_color = vec4(1.0);

        
        surfTension = smoothstep(0.0, 0.025, WATER_HEIGHT - (fs_in.wFragPos.y - bottlePosition.y)  );
        surfTension = smoothstep(0.15, 0.5, surfTension ) + 0.5;

        
        float foam = smoothstep(0.0, 0.025+bottleVelocity, WATER_HEIGHT - (fs_in.wFragPos.y - bottlePosition.y)  );
        foam = clamp(smoothstep(0.15, 0.5, foam ) + 0.5, 0.0,1.0);


        if(!gl_FrontFacing){ 
            lNormal = vec3(0.0, 1.0, 0.0); 
            Normal = lNormal;
        }

        MetallicHeight.x = 1.0;
        r_tex.x = .0;
        r_tex.z = 1.0;

        liquid_color *= (foam_color*(1.0-abs(foam))) + liquid_color;
        
        vec3 refr = refract(Normal, V, 1.0 / (1.33 + surfTension)).rgb;
        transparency = vec4(textureLod(blurredTexture, coords  + dudv + (refr.xy * clamp(1.0-surfTension, 0.1, 1.)), 0.0).rgb, 1.0) * liquid_color * 0.95 * clamp(1.0 /*- DiffuseTex.a*/, 0.0, 1.0);

        vec3 Lo = vec3(0.0);
        for(int i = 0 ; i < outBuffer.Lights.length() ; i++)
        {
            if(outBuffer.Lights[i].type == 2) {
                Lo += SphereAreaLight(outBuffer.Lights[i].position.rgb, outBuffer.Lights[i].radius, outBuffer.Lights[i].color.rgb, outBuffer.Lights[i].watts, lNormal) * liquid_color.rgb* clamp(1.0 /*- DiffuseTex.a*/, 0.0, 1.0);
                light += Lo;
            }
        }
        
        
        if(DiffuseTex.a > 0)
            blend = 1.0;
        else 
            blend = 0.5;

        vec3 sky = texture(skybox, normalize(reflect(normalize(lNormal), V)), r * MAX_MIPS).rgb;
        light += sky;
        
        topWaterColor = (Lo + sky + ( clamp(1.0 - 0.0, 0.0, 1.0) * transparency.rgb)) * DiffuseTex.rgb * liquid_color.rgb;
        
    } else {
        
        if(DiffuseTex.a > 0)
            blend = DiffuseTex.a;
        else 
            blend = 0.5;
    }


    if(!gl_FrontFacing && fs_in.wFragPos.y - bottlePosition.y < WATER_HEIGHT) {
        FragColor.rgb = topWaterColor;//(light + ( clamp(1.0 - 0.0, 0.0, 1.0) * transparency.rgb)) * DiffuseTex.rgb;
        FragColor.a = 0.5;
    } else {

            vec3 sky = texture(skybox, normalize(reflect(Normal, V)), r * MAX_MIPS).rgb;
            float F = fresnel(max(dot(Normal, V), 0.0), 1.0, r);
            FragColor.rgb = (light + sky + ( clamp(1.5 - 0.0, 0.0, 1.0) * (clamp(1.0-foam, 0.0, 1.0)*transparency.rgb))) * DiffuseTex.rgb;
            FragColor.a = blend;
            BrightFragColor = vec4(vec3(0.0), 1.0);
    }


        
}