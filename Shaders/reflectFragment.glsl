#version 330 core
uniform sampler2D diffuseTex;
uniform samplerCube cubeTex;
uniform vec4 waterColour;
uniform float time;

uniform vec3 cameraPos;

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColour;

float rand(int seed) {
    return fract(sin(float(seed)) * 43758.5453);
}

vec3 rotatePos(vec3 pos, float rotation) {
	float cosTheta = cos(rotation);
	float sinTheta = sin(rotation);

	float rotatedX = pos.x * cosTheta - pos.z * sinTheta;
	float rotatedZ = pos.x * sinTheta + pos.z * cosTheta;

	return vec3(rotatedX, pos.y, rotatedZ);
}

float calculateAmp(vec3 pos, int amt) {
	float amp = 0;
	float timeFactor = time * 0.002;
	for (int i = 0; i < amt; i++) {
		float dir = radians(rand(i) * 25);
		float frequency = float(i*0.2 + 1) * 0.2;
		float amplitude = 1.0 / (i*.2 + 1);
		vec3 rotated = rotatePos(pos, dir);
		amp += sin(rotated.x * frequency + timeFactor) * amplitude / 5;
	}

	return amp;
}

void main(void) {
	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	vec3 viewDir = normalize(cameraPos - IN.worldPos);

	float waveAmp = calculateAmp(IN.worldPos, 15);
	
	vec3 perturbedNormal = normalize(IN.normal + vec3(waveAmp, 0.0, 0.0));
	
	vec3 reflectDir = reflect(-viewDir, perturbedNormal);
	vec4 reflectTex = texture(cubeTex, reflectDir);

	fragColour = reflectTex + (diffuse * 0.25);
	fragColour *= waterColour;


}