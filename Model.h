#ifndef MODEL_H
#define MODEL_H
#include "Mesh.h"
#include "Shader.h"
#include <string>
#include <vector>
using namespace std;
class Model
{
public:
    vector<Texture> textures_loaded; // Список загруженных текстур
    vector<Mesh>    meshes; // Меши
    string directory; // Путь к папке с моделью и её текстурами
    bool gammaCorrection; // нужна ли коррекция гаммы

    Model(string const& path, bool isUV_flipped = true, bool gamma = false);
    void Draw(Shader* shader);

private:
    void loadModel(string const& path, bool isUV_flipped);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
#endif