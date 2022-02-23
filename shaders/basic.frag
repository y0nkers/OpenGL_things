#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec4 vertColor;
in vec3 vertNormal;
in vec3 fragPos;
in vec2 texCoords;

out vec4 outColor;

uniform sampler2D ourTexture;
uniform bool wireframeMode;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main() {
    float dist = distance(light.position, fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist); // Коэффициент затухания
    
    vec3 ambient = light.ambient * material.ambient * attenuation;

    // diffuse
    vec3 norm = normalize(vertNormal);
    vec3 lightDir = normalize(fragPos - light.position); // Направление света
    float diff_coef = max(dot(norm, -lightDir), 0.0f);
    vec3 diffuse = light.diffuse * (diff_coef * material.diffuse) * attenuation; // Вектор диффузной (матовой) освещённости

    // specular
    vec3 reflectDir = reflect(-lightDir, norm); // Отражённый относительно нормали свет
    vec3 viewDir = normalize(fragPos - viewPos); // Направление взгляда камеры
    float specular_coef = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = light.specular * (specular_coef * material.specular) * attenuation; // Вектор спекулярной (зеркальной) освещённости

    if (wireframeMode)
        outColor = vertColor;
    else
        outColor = texture(ourTexture, texCoords) * vec4(diffuse + ambient + specular, 1.0f); // * vertColor;
}