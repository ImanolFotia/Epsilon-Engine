#define BLOCKER_SEARCH_NUM_SAMPLES 16
#define PCF_NUM_SAMPLES 32
float LIGHT_WORLD_SIZE = 100.0;//ubo.lightPosition.x; // 1.0
float LIGHT_FRUSTUM_WIDTH = 7.3;//ubo.lightPosition.y; // 50.0
// Assuming that LIGHT_FRUSTUM_WIDTH == LIGHT_FRUSTUM_HEIGHT
float LIGHT_SIZE_UV = (LIGHT_WORLD_SIZE / LIGHT_FRUSTUM_WIDTH);

vec2 poissonDisk[64] = {
    vec2(-0.613392, 0.617481),
    vec2(0.170019, -0.040254),
    vec2(-0.299417, 0.791925),
    vec2(0.645680, 0.493210),
    vec2(-0.651784, 0.717887),
    vec2(0.421003, 0.027070),
    vec2(-0.817194, -0.271096),
    vec2(-0.705374, -0.668203),
    vec2(0.977050, -0.108615),
    vec2(0.063326, 0.142369),
     vec2(0.203528, 0.214331),
     vec2(-0.667531, 0.326090),
     vec2(-0.098422, -0.295755),
     vec2(-0.885922, 0.215369),
     vec2(0.566637, 0.605213),
     vec2(0.039766, -0.396100),
     vec2(0.751946, 0.453352),
     vec2(0.078707, -0.715323),
     vec2(-0.075838, -0.529344),
     vec2(0.724479, -0.580798),
     vec2(0.222999, -0.215125),
     vec2(-0.467574, -0.405438),
     vec2(-0.248268, -0.814753),
     vec2(0.354411, -0.887570),
     vec2(0.175817, 0.382366),
     vec2(0.487472, -0.063082),
     vec2(-0.084078, 0.898312),
     vec2(0.488876, -0.783441),
     vec2(0.470016, 0.217933),
     vec2(-0.696890, -0.549791),
     vec2(-0.149693, 0.605762),
     vec2(0.034211, 0.979980),
     vec2(0.503098, -0.308878),
     vec2(-0.016205, -0.872921),
     vec2(0.385784, -0.393902),
     vec2(-0.146886, -0.859249),
     vec2(0.643361, 0.164098),
     vec2(0.634388, -0.049471),
     vec2(-0.688894, 0.007843),
     vec2(0.464034, -0.188818),
     vec2(-0.440840, 0.137486),
     vec2(0.364483, 0.511704),
     vec2(0.034028, 0.325968),
     vec2(0.099094, -0.308023),
     vec2(0.693960, -0.366253),
     vec2(0.678884, -0.204688),
     vec2(0.001801, 0.780328),
     vec2(0.145177, -0.898984),
     vec2(0.062655, -0.611866),
     vec2(0.315226, -0.604297),
     vec2(-0.780145, 0.486251),
     vec2(-0.371868, 0.882138),
     vec2(0.200476, 0.494430),
     vec2(-0.494552, -0.711051),
     vec2(0.612476, 0.705252),
     vec2(-0.578845, -0.768792),
     vec2(-0.772454, -0.090976),
     vec2(0.504440, 0.372295),
     vec2(0.155736, 0.065157),
     vec2(0.391522, 0.849605),
     vec2(-0.620106, -0.328104),
     vec2(0.789239, -0.419965),
     vec2(-0.545396, 0.538133),
     vec2(-0.178564, -0.596057)
};

float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5433);
}

float rand(vec4 co) {
    float dot_product = dot(co, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dot_product) * 43758.5433);
}

float PenumbraSize(float zReceiver, float zBlocker) // Parallel plane estimation
{
    return (zReceiver - zBlocker) / zBlocker;
}

void FindBlocker(in sampler2D depthSampler, out float avgBlockerDepth,
                 out float numBlockers,
                 vec2 uv, float zReceiver, vec3 shadowCoord)
{
    // This uses similar triangles to compute what
    // area of the shadow map we should search
    float searchWidth =  LIGHT_SIZE_UV; //*((zReceiver - NEAR_PLANE) / zReceiver);
    float blockerSum = 0;
    numBlockers = 0;



    ivec2 depthSize = textureSize(depthSampler, 0);
    vec2 newShadowCoord = depthSize * shadowCoord.xy;

    for (int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES; ++i)
    {
        ivec2 coord = ivec2(newShadowCoord + depthSize * vec2(((poissonDisk[i]/1000.0)) * searchWidth));
        float shadowMapDepth = texelFetch (depthSampler, coord, 0).r;

        if (shadowMapDepth < zReceiver)
        {
            blockerSum += shadowMapDepth;
            numBlockers++;
        }
    }

    avgBlockerDepth = blockerSum / numBlockers;
}
float PCF_Filter(sampler2DShadow shadowSampler, vec2 uv, vec2 texCoords, in vec3 shadowCoord, float zReceiver, float filterRadiusUV)
{
    float sum = 0.0;
    float theta = rand(vec4(uv, texCoords.xy));
    mat2 rotation = mat2(vec2(cos(theta), sin(theta)), vec2(-sin(theta), cos(theta)));
    for (int i = 0; i < PCF_NUM_SAMPLES; ++i)
    {
        vec2 offset = (rotation*(poissonDisk[i]/1000.0)) * filterRadiusUV;
        vec2 texOffset = uv * offset;
        bvec2 outside = greaterThan(texOffset, vec2(1.0));
        bvec2 inside = lessThan(texOffset, vec2(0.0));
        float isShadow = texture(shadowSampler, shadowCoord + vec3(texOffset, 0.0));
        float z = texture(shadowSampler, shadowCoord + vec3(offset, 0.0)).r;
        sum += z;//((any(outside)||any(inside)) ? 1.0 : isShadow);//z;// > shadowCoord.z + 0.01 ? 1.0 : 0.0;
    }
    return sum / PCF_NUM_SAMPLES;
}
float PCSS(sampler2D depthSampler, sampler2DShadow shadowSampler, vec4 coords, vec3 shadowCoord, vec2 texCoords)
{
    vec2 uv = coords.xy / coords.w;
    float zReceiver = shadowCoord.z; // Assumed to be eye-space z in this code
    // STEP 1: blocker search
    float avgBlockerDepth = 0;
    float numBlockers = 0;
    FindBlocker(depthSampler, avgBlockerDepth, numBlockers, uv, zReceiver, shadowCoord);
    //avgBlockerDepth = avgBlockerDepth;
    if (numBlockers < 1)
        // There are no occluders so early out (this saves filtering)
        return 1.0;
    // STEP 2: penumbra size
    float penumbraRatio = PenumbraSize(zReceiver, avgBlockerDepth);
    
    float filterRadiusUV = min(penumbraRatio * /*ubo.lightPosition.z*/13.2, 9.0); /** NEAR_PLANE / zReceiver*/;
    // STEP 3: filtering
    return PCF_Filter(shadowSampler, uv, texCoords, shadowCoord, zReceiver, filterRadiusUV);
}