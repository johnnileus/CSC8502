#version 330 core

out Vertex {
	vec4 colour;
} OUT;

void main(void) {
	OUT.colour = vec4(1.0,0.0,1.0,0.0);
}