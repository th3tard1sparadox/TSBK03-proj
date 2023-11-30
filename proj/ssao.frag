#version 150

out vec4 outColor;
in vec2 outTexCoord;

uniform sampler2D texUnit; //pos
uniform sampler2D texUnit2; //norm
uniform sampler2D texUnit3; //noise
uniform vec3 samples[200];
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main(void)
{
	float shaded = 0.0;
	float depth = 0.0;
	int tot = 200;
	float radius = 0.5;
	float bias = 0.025;

	vec3 pos = texture(texUnit, outTexCoord).xyz;
	vec3 norm = normalize(texture(texUnit2, outTexCoord).xyz);
	vec3 randVec = normalize(texture(texUnit3, outTexCoord).xyz);
	vec3 worldPos = (inverse(viewMatrix) * vec4(pos, 1.0)).xyz;

	vec3 tang = normalize(randVec - norm * dot(randVec, norm));
	vec3 bitang = cross(norm, tang);
	mat3 TBN = mat3(tang, bitang, norm);

	vec3 samPos;
	for(int i = 0; i < tot; i++) {
		samPos = TBN * samples[i].xyz;
		samPos = worldPos + samPos * radius;
		samPos = (viewMatrix * vec4(samPos, 1.0)).xyz;

		vec4 off = vec4(samPos, 1.0);
		off = projectionMatrix * off;
		off.xyz /= off.w;
		off.xyz = off.xyz * 0.5 + 0.5;

		depth = texture(texUnit, off.xy).z;
		float range = smoothstep(0.0, 1.0, radius / abs(samPos.z - depth));
		if(depth >= samPos.z + bias) {
			shaded += 1.0 * range;
		}
	}

	float shade = (1.0 - (shaded / float(tot)));
	// shade = pow(shade, 4.0);

	outColor = vec4(shade, shade, shade, 1.0);
}
