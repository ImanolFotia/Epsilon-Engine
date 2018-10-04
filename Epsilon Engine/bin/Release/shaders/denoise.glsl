#version 440 core

in vec2 TexCoords;
out vec4 outColor;

uniform vec2 resolution;
uniform float exponent;


uniform sampler2D texture0;

void main()
{
	vec4 center = texture(texture0, TexCoords);

	if(center.a == 0){
		outColor = center;
		return;
	}

	vec4 color = vec4(0.0);
	float total = 0.0;
	for (float x = -4.0; x <= 4.0; x += 1.0) {
		for (float y = -4.0; y <= 4.0; y += 1.0) {
			vec4 sample0 = texture(texture0, TexCoords + vec2(x, y) / resolution);
			float weight = 1.0 - abs(dot(sample0.rgb - center.rgb, vec3(0.25)));
			weight = pow(weight, exponent);
			color += sample0 * weight;
			total += weight;
		}
	}

	outColor = color / total;
}