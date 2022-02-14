#version 330 core
layout (location = 0) in vec3 inPos; // in - входные данные
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCords;
layout (location = 3) in vec4 inColor;

out vec4 vertColor;
out vec3 vertNormal;
out vec3 fragPos; // Позиция фрагмента в мировых координатах
out vec2 texCoords;

// @vecN
// @:
//   - float
// i - int
// u - unsigned int
// d - double
// b - bool
// N - 1/2/3/4

uniform mat4 pv;
uniform mat4 model;

void main() {
	vec4 vertPos = model * vec4(inPos, 1.0f);
	gl_Position = pv * vertPos;
	vertColor = inColor;
	texCoords = inTexCords;
	vertNormal = mat3(model) * inNormal;
	fragPos = vertPos.xyz;
}