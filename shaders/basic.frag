#version 330 core
in vec4 vertColor;
in vec3 vertNormal;
in vec3 fragPos;
in vec2 texCoords;

out vec4 outColor;

uniform sampler2D ourTexture;
uniform bool wireframeMode;
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambientColor;

void main() {
    vec3 ambient = ambientColor * 0.2f;

    vec3 norm = normalize(vertNormal);
    vec3 lightDir = normalize(fragPos - lightPos); // Направление света

    float diff_coef = max(dot(norm, -lightDir), 0.0f);
    vec3 diffuse = diff_coef * lightColor; // Вектор диффузной (матовой) освещённости

    vec3 reflectDir = reflect(-lightDir, norm); // Отражённый относительно нормали свет
    vec3 viewDir = normalize(fragPos - viewPos); // Направление взгляда камеры
    
    float specular_multiplier = 2.0f;
    float specular_coef = pow(max(dot(viewDir, reflectDir), 0.0f), 512);
    vec3 specular = specular_multiplier * specular_coef * lightColor; // Вектор спекулярной (зеркальной) освещённости

    if (wireframeMode)
        outColor = vertColor;
    else
        outColor = texture(ourTexture, texCoords) * vec4(diffuse + ambient + specular, 1.0f); // * vertColor;
}