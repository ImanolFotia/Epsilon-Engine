float linstep(float min, float max, float v) {
    return clamp((v-min) / (max-min), 0.0, 1.0);
}

float VSM(sampler2D shadowSampler, vec3 coords) {
    vec2 moments = texture(shadowSampler, coords.xy).xy;

    float p = step(coords.z, moments.x);
    float variance = max(moments.y - moments.x * moments.x, 0.00001);

    float d = coords.z - moments.x;
    float pMax = linstep(0.2, 1.0, variance / (variance + d*d));

    
    return smoothstep(0.95, 1.0, min(max(p, pMax), 1.0));
}
/*
// Where to split the value. 8 bits works well for most situations.    
float g_DistributeFactor = 256; 
vec4 DistributePrecision(vec2 Moments) {   
    float FactorInv = 1 / g_DistributeFactor;   // Split precision    
    vec2 IntPart;   
    vec2 FracPart = modf(Value * g_DistributeFactor, IntPart);   // Compose outputs to make reconstruction cheap.    
    return vec4(IntPart * FactorInv, FracPart); 
} 

vec2 RecombinePrecision(vec4 Value) {   
    float FactorInv = 1 / g_DistributeFactor;   
    return (Value.zw * FactorInv + Value.xy); 
}*/

