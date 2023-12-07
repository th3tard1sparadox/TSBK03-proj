#version 150

out vec4 outColor;
in vec2 outTexCoord;

uniform sampler2D texUnit; //pos
uniform sampler2D texUnit2; //norm
uniform sampler2D texUnit3; //noise
uniform vec3 samples[200];
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

float hemishade() {
	float shaded = 0.0;
	float depth = 0.0;
	int tot = 200;
	float radius = 0.5;
	float bias = 0.0;
	vec2 nScale = vec2(512.0/10.0);

	vec3 pos = texture(texUnit, outTexCoord).xyz;
	// vec3 norm = normalize(2.0 * texture(texUnit2, outTexCoord).xyz - vec3(1.0));
	vec3 norm = normalize(texture(texUnit2, outTexCoord).xyz);
	vec3 randVec = normalize(2.0 * texture(texUnit3, outTexCoord * nScale).xyz - vec3(1.0, 1.0, 0.0));
	vec3 worldPos = (inverse(viewMatrix) * vec4(pos, 1.0)).xyz;

	vec3 tang = normalize(randVec - norm * dot(randVec, norm));
	vec3 bitang = normalize(cross(norm, tang));
	mat3 TBN = mat3(tang, bitang, norm);

	vec3 samPos;
	for(int i = 0; i < tot; i++) {
		samPos = TBN * (samples[i].xyz * radius);
		// samPos = worldPos + samPos * radius;
		samPos = pos + samPos; // * radius;
		// samPos = (viewMatrix * vec4(samPos, 1.0)).xyz;

		vec4 off = vec4(samPos, 1.0);
		off = projectionMatrix * off;
		off.xyz /= off.w;
		off.xyz = off.xyz * 0.5 + 0.5;
		
		samPos = (vec4(samPos, 1.0)).xyz;

		depth = texture(texUnit, off.xy).z;
		float range = smoothstep(0.0, 1.0, radius / abs(samPos.z - depth));
		if(depth >= samPos.z + bias) {
			shaded += 1.0 * range;
		}
	}

	return (1.0 - (shaded / float(tot)));
}

float approxshade() {
	vec3 pos = texture(texUnit, outTexCoord).xyz;
	vec3 coord1;
	vec3 coord2;
	float diff = 0.0;
	float cutoff = 0.8;
	for(int i = 1; i < 4; i++) {
		coord1 = vec3(outTexCoord.x + float(i) / 512.0, outTexCoord.y, 0.0);
		coord2 = vec3(outTexCoord.x - float(i) / 512.0, outTexCoord.y, 0.0);
		if(coord1.x >= 0.0 && coord2.x >= 0.0 && coord1.x <= 1.0 && coord2.x <= 1.0 &&
		   coord1.y >= 0.0 && coord2.y >= 0.0 && coord1.y <= 1.0 && coord2.y <= 1.0) {
			coord1.z = texture(texUnit, coord1.xy).z;
			coord2.z = texture(texUnit, coord2.xy).z;
			float expectz = (coord1.z + coord2.z) / 2;
			if(abs(pos.z - expectz) < cutoff)
				diff += pos.z - expectz;
		}
	}
	for(int i = 1; i < 4; i++) {
		coord1 = vec3(outTexCoord.x, outTexCoord.y + float(i) / 512.0, 0.0);
		coord2 = vec3(outTexCoord.x, outTexCoord.y - float(i) / 512.0, 0.0);
		if(coord1.x >= 0.0 && coord2.x >= 0.0 && coord1.x <= 1.0 && coord2.x <= 1.0 &&
		   coord1.y >= 0.0 && coord2.y >= 0.0 && coord1.y <= 1.0 && coord2.y <= 1.0) {
			coord1.z = texture(texUnit, coord1.xy).z;
			coord2.z = texture(texUnit, coord2.xy).z;
			float expectz = (coord1.z + coord2.z) / 2;
			if(abs(pos.z - expectz) < cutoff)
				diff += pos.z - expectz;
		}
	}
	for(int i = 1; i < 4; i++) {
		coord1 = vec3(outTexCoord.x + float(i) / 512.0, outTexCoord.y + float(i) / 512.0, 0.0);
		coord2 = vec3(outTexCoord.x - float(i) / 512.0, outTexCoord.y - float(i) / 512.0, 0.0);
		if(coord1.x >= 0.0 && coord2.x >= 0.0 && coord1.x <= 1.0 && coord2.x <= 1.0 &&
		   coord1.y >= 0.0 && coord2.y >= 0.0 && coord1.y <= 1.0 && coord2.y <= 1.0) {
			coord1.z = texture(texUnit, coord1.xy).z;
			coord2.z = texture(texUnit, coord2.xy).z;
			float expectz = (coord1.z + coord2.z) / 2;
			if(abs(pos.z - expectz) < cutoff)
				diff += pos.z - expectz;
		}
	}
	for(int i = 1; i < 4; i++) {
		coord1 = vec3(outTexCoord.x + float(i) / 512.0, outTexCoord.y - float(i) / 512.0, 0.0);
		coord2 = vec3(outTexCoord.x - float(i) / 512.0, outTexCoord.y + float(i) / 512.0, 0.0);
		if(coord1.x >= 0.0 && coord2.x >= 0.0 && coord1.x <= 1.0 && coord2.x <= 1.0 &&
		   coord1.y >= 0.0 && coord2.y >= 0.0 && coord1.y <= 1.0 && coord2.y <= 1.0) {
			coord1.z = texture(texUnit, coord1.xy).z;
			coord2.z = texture(texUnit, coord2.xy).z;
			float expectz = (coord1.z + coord2.z) / 2;
			if(abs(pos.z - expectz) < cutoff)
				diff += pos.z - expectz;
		}
	}

	return 1.0 - (max(min(-diff / 10.0, 1.0), 0.0));
}

void main(void)
{
	float shade = approxshade();

	outColor = vec4(shade, shade, shade, 1.0);
}
