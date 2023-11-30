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
uniform vec3 samples[64];
uniform mat4 projectionMatrix;

void main(void)
{
	float shaded = 0.0;
	float depth = 0.0;
	int tot = 64;

	vec3 pos = texture(texUnit, outTexCoord).xyz;
	vec3 norm = normalize(texture(texUnit2, outTexCoord).xyz);
	vec3 randVec = normalize(vec3(18923.3213243534536, 132809491.1204, 5819168.3219847)); // TODO: def this

	vec3 tang = normalize(randVec - norm * dot(randVec, norm));
	vec3 bitang = cross(norm, tang);
	mat3 TBN = mat3(tang, bitang, norm);

	vec3 samPos;
	for(int i = 0; i < tot; i++) {
		samPos = TBN * samples[i];
		samPos = pos + samPos * 2.0;

		vec4 off = vec4(samPos, 1.0);
		off = projectionMatrix * off;
		off.xyz /= off.w;
		off.xyz = off.xyz * 0.5 + 0.5;

		depth = texture(texUnit, off.xy).z;
		if(depth >= samPos.z + 0.2) {
			shaded += 1.0;
		}
	}

	float shade = (1.0 - (shaded / 64.0));

	outColor = vec4(shade, shade, shade, 1.0);
	if(shaded > 64) {
		outColor = vec4(0.0, 1.0, 0.0, 1.0);
	}
}
