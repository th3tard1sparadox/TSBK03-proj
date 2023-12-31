
#version 150

// Simplified Phong: No materials, only one, hard coded light source
// (in view coordinates) and no ambient

// Note: Simplified! In particular, the light source is given in view
// coordinates, which means that it will initializerfollow the camera.
// You usually give light sources in world coordinates.

out vec4 outColor;
in vec3 exNormal; // Phong
in vec3 outNormal; // Phong

void main(void)
{
	// outColor = vec4(normalize(outNormal.xyz), 1.0);
	outColor = vec4(normalize(exNormal.xyz), 1.0);
}
