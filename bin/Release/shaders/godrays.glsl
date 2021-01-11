
// Falloff, as we radiate outwards.
const float DECAY = 1.0;
// Controls the sample density, which in turn, controls the sample spread.
const float DENSITY = 1.0;
// Sample weight. Decays as we radiate outwards.
const float WEIGHT = 0.1;

const float SAMPLES = 10.;

const vec4 LightColor = normalize(vec4(68.0, 88.0, 135.0, 1.0));

bool isSourceVisible(in vec4 sBlurOrigin) {
  return  !(sBlurOrigin.x > 1.0   || 
          sBlurOrigin.x < -1.0  || 
          sBlurOrigin.y > 1.0   || 
          sBlurOrigin.y < -1.0  ||
          sBlurOrigin.z < 0.0);
}

float hash(vec2 p) { return fract(sin(dot(p, vec2(41, 289))) * 45758.5453); }

vec4 godRays(in sampler2D brightBuffer, in vec3 blurOrigin) {
  // Light offset. Kind of fake. See above.
  vec4 outColor = vec4(0.0);
  vec4 ScreenPosition = projection * choppedView * vec4(blurOrigin, 1.0f);

  if(!lightShafts)
    return outColor;

  if(!isSourceVisible(ScreenPosition))
    return outColor;

  if(blurOrigin.y < 0.0)
    return outColor;

  ScreenPosition = ScreenPosition / ScreenPosition.w;
  vec2 FragCoord = gl_FragCoord.xy / Resolution;
  //ScreenPosition.xy -= normalize(ScreenPosition.xy - FragCoord) * 0.1;
  vec2 deltaTextCoord = vec2((FragCoord - .5) - ScreenPosition.xy * 0.5);
  deltaTextCoord *= 1.0 / float(SAMPLES) * DENSITY;
  float illuminationDecay = 1.0;
  FragCoord += deltaTextCoord * hash(FragCoord*0.1);

  for(int i = 0; i < SAMPLES; i++) {
    FragCoord -= deltaTextCoord;
    vec4 TexSample = texture(brightBuffer, FragCoord);

    TexSample *= illuminationDecay * WEIGHT;

    outColor += TexSample;

    illuminationDecay *= DECAY;
  }
  
  outColor = mix(outColor, vec4(0.0), length(vec2(0.0) - ScreenPosition.xy));
  outColor = vec4(clamp(outColor, vec4(0.0), vec4(1.0))) * 0.15 /** LightColor*/;
  return outColor;
}