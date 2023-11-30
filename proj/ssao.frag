#version 150

// Simplified Phong: No materials, only one, hard coded light source
// (in view coordinates) and no ambient

// Note: Simplified! In particular, the light source is given in view
// coordinates, which means that it will initializerfollow the camera.
// You usually give light sources in world coordinates.

out vec4 outColor;
in vec2 outTexCoord;

uniform sampler2D texUnit; //pos
uniform sampler2D texUnit2; //norm
uniform sampler2D texUnit3; //noise
uniform vec3 samples[200];
uniform mat4 projectionMatrix;

void main(void)
{
	float shaded = 0.0;
	float depth = 0.0;
	int tot = 200;

	vec3 pos = texture(texUnit, outTexCoord).xyz;
	vec3 norm = normalize(texture(texUnit2, outTexCoord).xyz);
	vec3 randVec = normalize(texture(texUnit3, outTexCoord).xyz);

	vec3 tang = normalize(randVec - norm * dot(randVec, norm));
	vec3 bitang = cross(norm, tang);
	mat3 TBN = mat3(tang, bitang, norm);

	vec3 samPos;
	for(int i = 0; i < tot; i++) {
		samPos = TBN * samples[i];
		samPos = pos + samPos * 0.5;

		vec4 off = vec4(samPos, 1.0);
		off = projectionMatrix * off;
		off.xyz /= off.w;
		off.xyz = off.xyz * 0.5 + 0.5;

		depth = texture(texUnit, off.xy).z;
		float range = smoothstep(0.0, 1.0, 0.5 / abs(samPos.z - depth));
		if(depth >= samPos.z + 0.025) {
			shaded += 1.0 * range;
		}
	}

	float shade = (1.0 - (shaded / float(tot)));

	outColor = vec4(shade, shade, shade, 1.0);
}
