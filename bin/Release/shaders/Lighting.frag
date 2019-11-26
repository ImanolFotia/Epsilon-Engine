#version 440 core

in vec2 TexCoords;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 blurImage;

layout (binding = 0) uniform sampler2D gPositionDepth;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D gAlbedoSpec;
layout (binding = 3) uniform sampler2D ssaoColorBufferBlur;
layout (binding = 4) uniform sampler2DShadow shadowMap;

uniform float exposure;
uniform mat4 lightSpaceMatrix;
uniform vec3 viewPos;
uniform vec3 lightDir;
uniform bool hdr;
uniform vec3 LightPositions[7];


const float gamma = 2.2;
const int NR_LIGHTS = 32;
const float shadowDistance = 100.0f;
const float fadeDistance = 10.0f;
const vec3 SkyColor = vec3(0.5,0.5,0.5);
const float density = 0.01;
const float gradient = 8.0;


vec3 FragPos;
vec3 Normal;
vec3 Diffuse;
float Specular;
float Depth;
vec3 ambient;
vec3 SpecDiff;
float AmbientOcclusion;
float shadow = 0.0;

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

// Returns a random number based on a vec3 and an int.
float random(vec3 seed, int i){
    vec4 seed4 = vec4(seed,i);
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

vec3 HDR(vec3 image)
{
    vec3 result = vec3(1.0) - exp(-image.xyz * exposure);
    result.xyz = pow(result.xyz, vec3(1.0 / gamma));
    return (result.xyz - 0.5f) * 1.1f + 0.5f;
}

float poissonShadowMapping(vec4 fragPosLightSpace){

    float visibility = 1.0;
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    fragPosLightSpace = fragPosLightSpace * 0.5 + 0.5;
    
    vec3 normal = normalize(Normal);
    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.005);

    for (int i=0;i<4;i++){
        int index = int(16.0*random(floor(FragPos.xyz*1000.0), i))%16;
        visibility -= 0.2*(texture(shadowMap, vec3(fragPosLightSpace.xy+ poissonDisk[index]/2000.0, fragPosLightSpace.z - bias)/fragPosLightSpace.w).r);
    }

    return visibility;
}

float attenuation = 0.0f;
float minLight = 0.08;
float b = 1.0 / 40.0;
float radius = sqrt(1.0 / (b * minLight));


vec3 CalculateDirectional();
vec3 calculatePoint(vec3 LightPos);

void main()
{

    FragColor.a = 1.0;
    blurImage.a = 1.0;

    // Retrieve data from gbuffer
    FragPos = texture(gPositionDepth, TexCoords).rgb;
    Normal = normalize(texture(gNormal, TexCoords).rgb);
    Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    Specular = texture(gAlbedoSpec, TexCoords).a;
    SpecDiff = Diffuse * Specular;
    AmbientOcclusion = texture(ssaoColorBufferBlur, TexCoords).r;
    vec4 FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0f);
    
    ambient = clamp(Diffuse *  0.05, 0.0, 1.0);

    shadow = poissonShadowMapping(FragPosLightSpace);

    vec3 light = CalculateDirectional();

    b = 1.0 / (radius*radius * minLight);

    for(int i = 0 ; i < 7 ; i++)
      light += calculatePoint(LightPositions[i]);

    light += ambient;

    float distance = length(viewPos - FragPos);

    float visibility = exp(-pow((distance*density), gradient));
    vec3 composedScene;

    if(hdr)
        composedScene = light.rgb * (AmbientOcclusion);
    else
        composedScene = light.rgb;

    FragColor.rgb = mix(SkyColor, composedScene, visibility);
    vec3 testHDR = FragColor.rgb;
    float brightness = dot(testHDR.rgb * ((exposure - 3.0) <= 0 ? 1.0 : (exposure - 3.0)), vec3(0.2126, 0.7152, 0.0722));

    if(brightness > 1.0)
        blurImage.rgb = FragColor.rgb;
    else
        blurImage = vec4(0.0f, 0.0f, 0.0f, 1.0f);

}

vec3 CalculateDirectional()
{
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightcolor = normalize(vec3(205, 109, 39));
    vec3 lightdirection = lightDir;

    if(clamp(lightDir.y + 0.1, 0.0, 1.0) < 0.0)
    {
      vec3 lightdirection = -lightDir;
      lightcolor = normalize(vec3(0.5, 0.5, 0.5));
    }

    float diff =  max(dot(lightDir, Normal), 0.0);
    vec3 diffuse = diff * Diffuse;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    vec3 specular = pow(max(dot(Normal, halfwayDir), 0.0), clamp(Specular * 256, 16.0, 256.0)) * vec3(Specular) * 1.5;

    ambient = Diffuse * vec3(mix(0.005, 0.05, clamp(lightDir.y + 0.1, 0.0, 1.0)));

    return mix(ambient, clamp(1.0 - shadow, 0.0, 1.0) * vec3(diffuse + specular) * lightcolor, clamp(lightDir.y + 0.1, 0.0, 1.0)) * 2.0;

}


vec3 calculatePoint(vec3 LightPos)
{

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDirection = normalize(LightPos - FragPos);
    float diff =  max(dot(lightDirection, Normal), 0.0);
    vec3 diffuse = vec3(diff) * Diffuse;

    float distance = length(LightPos - FragPos);
    attenuation = clamp(1.0 - distance/radius, 0.0, 1.0);
    attenuation *= attenuation;
    vec3 halfwayDir = normalize(lightDirection + viewDir);
    float refractiveFactor = dot(viewDir, Normal);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), max(clamp((1 - refractiveFactor) * 256, 16.0, 256.0), clamp((1 - Specular) * 256, 16.0, 256.0))) ;
    vec3 specular = vec3(1.0) * spec * SpecDiff;
    diffuse  *= attenuation;
    specular *= attenuation * 25;

    return  (specular * clamp( 1 - refractiveFactor, 0.1, 0.9) + diffuse);
}
