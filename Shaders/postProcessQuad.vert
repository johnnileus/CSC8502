#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
in vec3 position;
in vec2 texCoord;

out Vertex {
	vec2 texCoord;
} OUT;



void main(void) {
	vec3 pos = position;
	mat4 invproj = inverse(projMatrix);
	pos.xy *= vec2 (invproj[0][0], invproj[1][1]);
	pos.z = -1.0f;
	gl_Position = vec4(position,1.0);
	OUT.texCoord = texCoord;
}
