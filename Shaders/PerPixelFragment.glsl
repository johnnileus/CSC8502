#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform sampler2D grassTex;
uniform sampler2D grassBump;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex {
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {

    float dotProduct = dot(IN.normal, vec3(0,1,0)); // for texture
    vec4 diffuse = 0.0f.rrrr;
    vec3 bumpSample = 0.0f.rrr;

    if (dotProduct < 0.8f) {
        diffuse = texture(diffuseTex, IN.texCoord);
        bumpSample = texture(bumpTex, IN.texCoord).rgb;
    } else {
        diffuse = texture(grassTex, IN.texCoord);
        bumpSample = texture(grassBump, IN.texCoord).rgb;
    }

    
    vec3 tangentNormal = bumpSample * 2.0 - 1.0;

    vec3 tangent = normalize(vec3(1.0, 0.0, 0.0)); // This would typically be provided per-vertex
    vec3 bitangent = normalize(cross(IN.normal, tangent));
    mat3 TBN = mat3(tangent, bitangent, IN.normal);

    vec3 perturbedNormal = normalize(TBN * tangentNormal);

    vec3 incident = normalize(lightPos - IN.worldPos);
    vec3 viewDir = normalize(cameraPos - IN.worldPos);
    vec3 halfDir = normalize(incident + viewDir);



    float lambert = max(dot(incident, perturbedNormal), 0.0f);
    float distance = length(lightPos - IN.worldPos);
    float attenuation = 1.0 - clamp(distance / lightRadius, 0.0, 1.0);

    float specFactor = pow(max(dot(halfDir, perturbedNormal), 0.0), 16.0); // Specular factor (adjust shininess)

    vec3 surface = (diffuse.rgb * lightColour.rgb);
    fragColour.rgb = surface * lambert * attenuation; // Diffuse lighting
    fragColour.rgb += (lightColour.rgb * specFactor) * attenuation * 0.33; // Specular lighting
    fragColour.rgb += surface * 0.1f; // Ambient lighting
    fragColour.a = diffuse.a;


}