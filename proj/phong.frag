#version 150

// Simplified Phong: No materials, only one, hard coded light source
// (in view coordinates) and no ambient

// Note: Simplified! In particular, the light source is given in view
// coordinates, which means that it will initializerfollow the camera.
// You usually give light sources in world coordinates.

out vec4 outColor;
in vec3 exNormal; // Phong
in vec3 exSurface; // Phong (specular)

// uniform mat4 modelviewMatrix;
uniform vec3 lights[70];
uniform int num_lights;

// vec3 col[5];
// col[0] = vec3(1.0,0.0,0.0);
// col[1] = vec3(0.0,1.0,0.0);
// col[2] = vec3(0.0,0.0,1.0);
// col[3] = vec3(1.0,1.0,0.0);
// col[4] = vec3(1.0,0.0,1.0);

void main(void)
{
	float diffuse, specular;
	float shade = 0.0;
	for(int i = 0; i < num_lights; i++) {
		// vec3 light = vec3(modelviewMatrix * vec4(lights[i], 1.0)); // Given in VIEW coordinates! You usually specify light sources in world coordinates.
		vec3 light = normalize(lights[i] - exSurface); // Given in VIEW coordinates! You usually specify light sources in world coordinates.
		// vec3 light = vec3(0.0, 0.0, 1.0); // Given in VIEW coordinates! You usually specify light sources in world coordinates.
		
		// Diffuse
		diffuse = dot(normalize(exNormal), light);
		diffuse = max(0.0, diffuse); // No negative light
		
		// Specular
		vec3 r = reflect(-light, normalize(exNormal));
		vec3 v = normalize(-exSurface); // View direction
		specular = dot(r, v);
		if (specular > 0.0)
			specular = 1.0 * pow(specular, 150.0);
		specular = max(specular, 0.0);
		// Increased 1.0 to 2.0
		shade += (0.7*diffuse + 1.0*specular) / num_lights;
	}

	shade = shade * 1.0;

	outColor = vec4(shade, shade, shade, 1.0);
    // outColor = vec4(1.0);
}
