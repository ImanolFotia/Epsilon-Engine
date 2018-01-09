
const float PI = 3.14159265359;

float minLight = 0.03;
float b = 1.0 / 40.0;
//float radius = sqrt(1.0 / (b * minLight));

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
  
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
  
    return nom / denom;
}

float DistributionGGX2(in vec3 N, in vec3 H, in float roughness, in vec3 L,  in float rad)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float aP2    = a2 * a2;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    return (a2 * aP2) / pow(NdotH2 * (a2 - 1.0) + 1.0, 2.0);
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

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
} 

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - clamp(cosTheta, 0.0, 1.0), 5.0);
}

float calculateAttenuation(float power, float distance)
{
    return power / (4.0f * PI * (distance*distance));
}

float orenNayarDiffuse(vec3 ld,vec3 vd,vec3 sn,float r,float a) {
  
  float LdotV = dot(ld, vd);
  float NdotL = dot(ld, sn);
  float NdotV = dot(sn, vd);

  float s = LdotV - NdotL * NdotV;
  float t = mix(1., max(NdotL, NdotV), step(.0, s));

  float sigma2 = r * r;
  float A = 1. - .5 * (sigma2/((sigma2 + .33) + .000001));
  float B = .45 * sigma2 / (sigma2 + .09) + .00001;
    
  float ga = dot(vd-sn*NdotV,sn-sn*NdotL);

  return max(0., NdotL) * (A + B * max(0., ga) * sqrt((1.0-NdotV*NdotV)*(1.0-NdotL*NdotL)) / max(NdotL, NdotV));
}

float LambertDiffuse(in vec3 N, in vec3 L)
{
    return max(dot(N, L), 0.0);
}

vec3 calculatePointPBR(vec3 pos, vec3 color)
{
  // calculate per-light radiance
        vec3 lightcolor = normalize(color);
        if(pos.z == 56.0)
            lightcolor = vec3(0.0, 0.0, 0.7);
        vec3 V = normalize(viewPos - FragPos);
        vec3 L = normalize(pos - FragPos);
        vec3 H = normalize(V + L);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= clamp(1.0 - ExtraComponents.x, 0.0, 1.0);
        //float distance    = length(pos - FragPos);
        float distance = length(pos - FragPos);
        float watts = 300;
        float attenuation = calculateAttenuation(watts, distance);//1.0 / (1.0 + 0.1/*factor*/ * pow(distance, 2));
        //attenuation *= smoothstep(watts, watts - 5.0, distance);
        vec3 radiance     = vec3(1.0) * attenuation;        
        
        // cook-torrance brdf
        float NDF = DistributionGGX(Normal, H, clamp(Specular, 0.05, 1.0));        
        float G   = GeometrySmith(Normal, V, L, Specular);      
        
        vec3 nominator    = NDF * G * F;
        float denominator = (4 * max(dot(V, Normal), 0.0) * max(dot(L, Normal), 0.0)) + 0.001; 
        vec3 brdf = nominator / denominator;
            
        // add to outgoing radiance Lo
        float NdotL = orenNayarDiffuse(L, V, Normal, clamp(Specular, 0.05, 1.0), 1.0);             
        vec3 Lo = (kD * Diffuse / PI + brdf) * radiance * NdotL; 
        return Lo * lightcolor;
}

vec3 CalculateDirectionalPBR()
{
    // calculate per-light radiance
        vec3 lightcolor = normalize(vec3(68, 88, 135));
        vec3 V = normalize(viewPos - FragPos);
        vec3 L = normalize(lightDir);
        vec3 H = normalize(V + L);  

        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - ExtraComponents.x;
        kD = clamp(kD, vec3(0.2), vec3(1.0));
        
        // cook-torrance brdf
        float NDF = DistributionGGX(Normal, H, clamp(Specular, 0.03, 1.0));        
        float G   = GeometrySmith(Normal, V, L, Specular);      
        
        vec3 nominator    = NDF * G * F;
        float denominator = (4 * max(dot(V, Normal), 0.0) * max(dot(L, Normal), 0.0)) + 0.001; 
        vec3 brdf = nominator / denominator;
            
        // add to outgoing radiance Lo
        float NdotL = orenNayarDiffuse(L, V, Normal, clamp(Specular, 0.03, 1.0), 1.0);//              
        vec3 Lo = (kD * Diffuse / PI + brdf) * NdotL; 
        return mix(vec3(0.0), Lo * clamp(1.0 - shadow, 0.0, 1.0) /** lightcolor*/, clamp(lightDir.y + 0.1, 0.0, 1.0))*4.0;
}

float saturate(in float x)
{
    return clamp(x, 0.0, 1.0);
}

float SpotLightIntensity(in float y, in float p, in float t)
{
    return saturate((t - y) / (p - y));
}

vec3 SpotLightPBR(in vec3 pos, in vec3 dir, in float radius, in vec3 color)
{


        vec3 lightcolor = normalize(color);
        //if(pos.z == 56.0)
        //    lightcolor = vec3(0.0, 0.0, 0.7);
        vec3 V = normalize(viewPos - FragPos);
        vec3 L = normalize(pos - FragPos);
        float distance = length(pos - FragPos);
        float maxDistance = 5.0;

        float theta = dot(L, -dir);
        float innerRadius = cos(radius);
        float outerRadius = clamp(distance/maxDistance, 0.0, 0.15);
        outerRadius = innerRadius - outerRadius;
        if(theta < outerRadius)
            return vec3(0.0);

        vec3 H = normalize(V + L);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= clamp(1.0 - ExtraComponents.x, 0.0, 1.0);
        //float distance    = length(pos - FragPos);
        float watts = 60;
        float attenuation = calculateAttenuation(watts, distance);//1.0 / (1.0 + 0.1/*factor*/ * pow(distance, 2));
        attenuation *= smoothstep(watts, watts - 5.0, distance);
        vec3 radiance     = vec3(1.0) * attenuation;        
        
        // cook-torrance brdf
        float NDF = DistributionGGX(Normal, H, clamp(Specular, 0.05, 1.0));        
        float G   = GeometrySmith(Normal, V, L, Specular);      
        
        vec3 nominator    = NDF * G * F;
        float denominator = (4 * max(dot(V, Normal), 0.0) * max(dot(L, Normal), 0.0)) + 0.001; 
        vec3 brdf = nominator / denominator;
            
        // add to outgoing radiance Lo
        float NdotL = orenNayarDiffuse(L, V, Normal, clamp(Specular, 0.05, 1.0), 1.0);             
        vec3 Lo = (kD * Diffuse / PI + brdf) * radiance * NdotL; 
        vec3 outColor = Lo;


        return outColor * lightcolor * SpotLightIntensity(outerRadius, innerRadius, theta)*3.0;
    
}

vec3 SphereAreaLight(in vec3 position, in float radius, in vec3 color, in float power)
{
    vec3 I;
    vec3 L, centerToRay, r, closestPoint;
    L = (position - FragPos);
    vec3 V = normalize(viewPos - FragPos);
    r = normalize(reflect(V, Normal));
    centerToRay = dot(L, r) * r - L;
    closestPoint = L + centerToRay*clamp(radius/length(centerToRay), 0.0, 1.0);
    vec3 l = normalize(closestPoint);

        L = l;//normalize(closestPoint - FragPos);
        vec3 H = normalize(V + L);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= clamp(1.0 - ExtraComponents.x, 0.0, 1.0);
        //float distance    = length(pos - FragPos);
        float distance = length(position - FragPos);
        float watts = power;
        float attenuation = calculateAttenuation(watts, distance);//1.0 / (1.0 + 0.1/*factor*/ * pow(distance, 2));
        //attenuation *= smoothstep(watts, watts - 5.0, distance);
        vec3 radiance     = vec3(1.0) * attenuation;        
        
        // cook-torrance brdf
        float NDF = DistributionGGX(Normal, H, clamp(Specular, 0.05, 1.0));       
        float G   = GeometrySmith(Normal, V, L, Specular);      
        
        vec3 nominator    = NDF * G * F;
        float denominator = (4 * max(dot(V, Normal), 0.0) * max(dot(L, Normal), 0.0)) + 0.001; 
        vec3 brdf = nominator / denominator;
            
        // add to outgoing radiance Lo
        float NdotL = orenNayarDiffuse(L, V, Normal, clamp(Specular, 0.05, 1.0), 1.0);             
        vec3 Lo = (kD * Diffuse / PI + brdf) * radiance * NdotL; 
        return Lo * color;
}