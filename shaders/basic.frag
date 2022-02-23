#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    int type;

    vec3 position;
    vec3 direction;
    float cutOff;
    
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
#define MAX_LIGHTS 4
uniform int lightsCount;
uniform Light light[MAX_LIGHTS];

float getAttenuation(int i) {
    float dist = distance(light[i].position, fragPos);
    float attenuation = 1.0 / (light[i].constant + light[i].linear * dist + light[i].quadratic * dist * dist); // Коэффициент затухания
    return attenuation;
}

vec3 calculateDiffusePlusSpecular(int i, vec3 lightDir) {
    // diffuse
    vec3 norm = normalize(vertNormal);
    float diff_coef = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = light[i].diffuse * (diff_coef * material.diffuse); // Вектор диффузной (матовой) освещённости

    // specular
    vec3 reflectDir = reflect(lightDir, norm); // Отражённый относительно нормали свет
    vec3 viewDir = normalize(fragPos - viewPos); // Направление взгляда камеры
    float specular_coef = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = light[i].specular * (specular_coef * material.specular); // Вектор спекулярной (зеркальной) освещённости 
    
    return diffuse + specular;
}

void main() {
    if (wireframeMode) outColor = vertColor;
    else 
    {
        outColor = vec4(0, 0, 0, 0);
        vec3 lightResult;
        for (int i =0; i < lightsCount; i++) {
            if (light[i].type == 1) // Directional Light
            {
                vec3 lightDir = -light[i].direction;
                vec3 ambient = light[i].ambient * material.ambient;
                vec3 diffspec = calculateDiffusePlusSpecular(i, lightDir);
                
                lightResult = ambient + diffspec;
            }
            else 
            {
                vec3 lightDir = -normalize(fragPos - light[i].position); // Направление света
                if (light[i].type == 2) // Point Light
                {
                    float attenuation = getAttenuation(i);
                    vec3 ambient = light[i].ambient * material.ambient;
                    vec3 diffspec = calculateDiffusePlusSpecular(i, lightDir);
                    
                    lightResult = (ambient + diffspec) * attenuation;
                }
                else if (light[i].type == 3) { // SpotLight
                    float angle = acos(dot(lightDir, normalize(-light[i].direction)));

                    if (angle <= light[i].cutOff * 2.0f) {
                        float coef = 1.0f;
                        if (angle >= light[i].cutOff) 
                        {
                            coef = (light[i].cutOff * 2.0f - angle) / light[i].cutOff;
                        }

                        float attenuation = getAttenuation(i);
                        vec3 ambient = light[i].ambient * material.ambient;
                        vec3 diffspec = calculateDiffusePlusSpecular(i, lightDir) * coef;
                        
                        lightResult = (ambient + diffspec) * attenuation;
                    }
                    else {
                        lightResult = material.ambient * light[i].ambient;
                    }
                }
            }
            outColor += texture(ourTexture, texCoords) * vec4(lightResult, 1.0f);
        }
    }
}