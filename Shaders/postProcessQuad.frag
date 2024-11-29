#version 330 core
uniform sampler2D screenTex; // Input texture
uniform sampler2D depthTex;

out vec4 fragColour;       // Output color
in Vertex {
    vec2 texCoord;
} IN;

vec4 fogColour = vec4(.6,.6,.6, 1.);

float near = 0.1f;
float far = 300.0f;

float lineariseDepth(float depth) {
    return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

vec4 applyFog(float depth, vec4 colour) {
    
    float fogFactor = (1 / (1 + exp(-.01 * depth)));

    vec4 newCol = mix(colour, fogColour, fogFactor);

    return newCol;
}

void main()
{
    // Sample the texture and depth
    vec4 screenCol = texture(screenTex, IN.texCoord);
    float depth = texture(depthTex, IN.texCoord).r;

    // Linearize the depth value
    float linearDepth = lineariseDepth(depth);

    // Apply fog effect
    fragColour = applyFog(linearDepth, screenCol);
}