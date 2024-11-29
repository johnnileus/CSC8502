#version 330 core
uniform sampler2D screenTex; // Input texture
uniform sampler2D depthTex;
uniform bool enableFog;

out vec4 fragColour;       // Output color
in Vertex {
    vec2 texCoord;
} IN;

vec4 fogColour = vec4(.6,.6,.6, 1.);

float near = 0.1f;
float far = 30000.0f;

float lineariseDepth(float depth) {
    return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

vec4 applyFog(float depth, vec4 colour) {
    
    float fogFactor = (1 / (1 + exp(-.005 * depth)));

    vec4 newCol = mix(colour, fogColour, fogFactor);

    return newCol;
}

void main()
{
    vec4 screenCol = texture(screenTex, IN.texCoord);
    float depth = texture(depthTex, IN.texCoord).r;

    float linearDepth = lineariseDepth(depth);
    if (enableFog) {
        fragColour = applyFog(linearDepth, screenCol);
    
    } else {
        fragColour = screenCol;
    }
}