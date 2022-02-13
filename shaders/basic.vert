#version 330 core
layout (location = 0) in vec3 aPos; // in - входные данные
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec2 inTexCords;

out vec4 vertColor;
out vec2 texCoords;

// @vecN
// @:
//   - float
// i - int
// u - unsigned int
// d - double
// b - bool
// N - 1/2/3/4

//uniform vec3 uniformPos;
uniform mat4 pvm;

void main() {
	gl_Position = pvm * vec4(aPos, 1.0f);
	vertColor = inColor;
	texCoords = inTexCords;
}