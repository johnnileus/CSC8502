#version 330 core

in Vertex {
	vec4 colour;
} IN;

out vec4 outCol;

void main(void) {
	outCol = IN.colour;
}