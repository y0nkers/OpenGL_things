#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
using namespace std;

struct Vertex { // Точка
    glm::vec3 Position; // Позиция
    glm::vec3 Normal; // Нормаль из точки
    glm::vec2 TexCoords; // Текстурная координата
    glm::vec3 Tangent; //  
    glm::vec3 Bitangent; //
};

struct Texture { // Текстура
    unsigned int id; // id текстуры
    string type; // Тип текстуры
    string path; // Путь к файлу с текстурой
};

class Mesh {
public:
    vector<Vertex>       vertices; // массив точек
    vector<unsigned int> indices; // массив индексов точек
    vector<Texture>      textures; // массив текстур
    unsigned int VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

    void Draw(Shader* shader);

private:
    unsigned int VBO, EBO;

    void setupMesh();
};
#endif