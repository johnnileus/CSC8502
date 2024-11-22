#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;
uniform sampler2D depthTex;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void) {
    vec4 diffuse = texture(diffuseTex, IN.texCoord);
    vec4 light = texture(diffuseLight, IN.texCoord);
    vec3 specular = texture(specularLight, IN.texCoord).xyz;

    float depth = texture(depthTex, IN.texCoord).r;
    if (depth > 10) {
        discard;
    }

    
    fragColour.xyz = diffuse.xyz * 0.1; // ambient
    fragColour.xyz += diffuse.xyz * light.xyz; // lambert
    fragColour.xyz += specular; // Specular
    fragColour.a = 0.9;
    
    fragColour = vec4(depth,0,0,1);

}
