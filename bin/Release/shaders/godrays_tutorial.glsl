const int SAMPLES = 10;

const float WEIGHT = 0.1;

const float DECAY = 1.0;

const float DENSITY = 1.0;

const vec4 LightColor = normalize(vec4(68.0, 88.0, 135.0, 1.0));

float hash(vec2 p) { return fract(sin(dot(p, vec2(41, 289))) * 45785.5453); }

bool isSourceVisible(in vec4 sBlurOrigin){
	return !(sBlurOrigin.x > 1.0  ||
			 sBlurOrigin.x < -1.0 ||
			 sBlurOrigin.y > 1.0  ||
			 sBlurOrigin.y < -1.0 ||
			 sBlurOrigin.z < 0.0 );
}

vec4 godRays(in sampler2D brightBuffer, in vec3 blurOrigin){
	vec4 outColor = vec4(0.0);

	vec4 ScreenPosition = projection * choppedView * vec4(blurOrigin, 1.0);

	if(!lightShafts)
		return outColor;

	if(blurOrigin.y > 0.0)
		return outColor;

	if(isSourceVisible(ScreenPosition))
		return outColor;

	ScreenPosition = ScreenPosition / ScreenPosition.w;
	vec2 FragCoord = gl_FragCoord.xy / Resolution;
	vec2 deltaTexCoord = vec2((FragCoord - 0.5) - ScreenPosition.xy * 0.5);
	deltaTexCoord *= 1.0 / float(SAMPLES) * DENSITY;
	float illuminationDecay = 1.0;

	FragCoord += deltaTexCoord * hash(FragCoord);

	for(int i = 0; i < SAMPLES; i++){
		FragCoord -= deltaTexCoord;
		vec4 TexSample = texture(brightBuffer, FragCoord);

		TexSample *= illuminationDecay * WEIGHT;

		outColor += TexSample;

		illuminationDecay *= DECAY;
	}

	outColor = mix(outColor, vec4(0.0), length(vec2(0.0) - ScreenPosition.xy));
	outColor = vec4(clamp(outColor, vec4(0.0), vec4(1.0))) * 0.25 * LightColor;

	return outColor;
}