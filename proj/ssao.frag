#version 150

// Simplified Phong: No materials, only one, hard coded light source
// (in view coordinates) and no ambient

// Note: Simplified! In particular, the light source is given in view
// coordinates, which means that it will initializerfollow the camera.
// You usually give light sources in world coordinates.

out vec4 outColor;
in vec3 exNormal; // Phong
in vec3 exSurface; // Phong (specular)
in vec2 outTexCoord;
uniform sampler2D texUnit;

void main(void)
{
	int shaded = 0;
	float depth = 0.0;
	int tot = 16;
	float h = 0.0001;
	float w = 2.0;
	float w2 = w/2;
	float w4 = w/4;
	float l1 = sqrt(h * h - w2 * w2);
	float l2 = sqrt(h * h - w4 * w4);
	float w2h = w2 / sqrt(2);
	float w4h = w4 / sqrt(2);
	int width = 512;
	int height = 512;


	vec3 sampleCoords[16];
	for(int i = 0; i < tot; i++) {
		sampleCoords[i] = exSurface;
	}
	sampleCoords[0] += vec3(0.0, w2, -l1);
	sampleCoords[1] += vec3(w2h, w2h, -l1);
	sampleCoords[2] += vec3(w2, 0.0, -l1);
	sampleCoords[3] += vec3(w2h, -w2h, -l1);
	sampleCoords[4] += vec3(0.0, -w2, -l1);
	sampleCoords[5] += vec3(-w2h, -w2h, -l1);
	sampleCoords[6] += vec3(-w2, 0.0, -l1);
	sampleCoords[7] += vec3(-w2h, w2h, -l1);
	sampleCoords[8] += vec3(0.0, w4, -l2);
	sampleCoords[9] += vec3(w4h, w4h, -l2);
	sampleCoords[10] += vec3(w4, 0.0, -l2);
	sampleCoords[11] += vec3(w4h, -w4h, -l2);
	sampleCoords[12] += vec3(0.0, -w4, -l2);
	sampleCoords[13] += vec3(-w4h, -w4h, -l2);
	sampleCoords[14] += vec3(-w4, 0.0, -l2);
	sampleCoords[15] += vec3(-w4h, w4h, -l2);

	for(int i = 0; i < tot; i++) {
		vec2 texCoord = vec2(sampleCoords[i].x / width, sampleCoords[i].y / height);
		depth = texture(texUnit, texCoord).x;
		if (depth < sampleCoords[i].z) {
			shaded++;
		}
	}

	float shade = (1.0 - (shaded / tot));

	outColor = vec4(shade, shade, shade, 1.0);
	if(shaded > 16) {
		outColor = vec4(0.0, 1.0, 0.0, 1.0);
	}
}
