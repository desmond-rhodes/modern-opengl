#version 450 core

in vec4 vPosition;
in vec4 vColour;
out vec4 fColour;

void main() {
	gl_Position = vPosition;
	fColour = vColour;
}
