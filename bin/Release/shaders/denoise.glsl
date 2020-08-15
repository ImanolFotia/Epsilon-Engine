#version 440 core

in vec2 TexCoords;
out vec4 outColor;

uniform vec2 resolution;
uniform float exponent;


uniform sampler2D texture0;
uniform sampler2D roughnessTex;
/*
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
			vec4 sample0 = texture(texture0, TexCoords + vec2(x, y) / resolution*0.5);
			float weight = 1.0 - abs(dot(sample0.rgb - center.rgb, vec3(0.25)));
			weight = pow(weight, exponent);
			color += sample0 * weight;
			total += weight;
		}
	}

	outColor = color / total;
}

*/

const float offset = 1.0 / 500.0;  

void main()
{
	vec4 center = texture(texture0, TexCoords);
	if(center.a == 0){
		outColor = center;
		return;
	}

	float roughness = texture(roughnessTex, TexCoords).a;

	if(roughness < 0.2) {
		
		outColor = center;
		return;
	}

    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

	float kernel[9] = float[](
	    1.0 / 16, 2.0 / 16, 1.0 / 16,
	    2.0 / 16, 4.0 / 16, 2.0 / 16,
	    1.0 / 16, 2.0 / 16, 1.0 / 16  
	);
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(texture0, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    outColor = center;//vec4(col, 1.0);
} 