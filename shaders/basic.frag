#version 330 core
in vec4 vertColor;
in vec2 texCoords;
out vec4 outColor;

//uniform vec3 uniformColor; // uniform - глобальная переменная
uniform sampler2D ourTexture;
uniform bool wireframeMode;

void main() {
    //outColor = vertColor;
    if (wireframeMode)
        outColor = vertColor;
    else
        outColor = texture(ourTexture, texCoords); // * vertColor;
    //outColor = vec4(uniformColor, 1.0f);
}