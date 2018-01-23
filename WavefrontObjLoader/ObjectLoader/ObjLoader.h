#pragma once

#include "MaterialLoader.h"
#include "Mesh.h"
#include <vector>
#include <string>
 
class ObjLoader {
public:
    ObjLoader();
    virtual ~ObjLoader();

    std::vector<Mesh*> GetMeshes() const {
        return std::move(m_meshes);
    }

    void LoadOBJ(std::string& p_dirPath, std::string& p_fileName);

private:
    std::string m_currentDirPath{ "" };
    std::vector<Mesh*> m_meshes;
    MaterialLoader m_materialLoader;
    Material* m_currentMaterial{ nullptr };
    Mesh* m_currentMesh{ nullptr };
    bool m_hasNormals{ false };
    bool m_hasTextures{ false };

    void EvaluateAndExecuteCommand(std::vector<std::string> p_lineTokens);
    void ReadObjVertexCoords(std::vector<std::string> p_lineTokens, bool p_isPosition);
    void ReadObjTextureCoords(std::vector<std::string> p_lineTokens);
    void ReadObjFaceWithNormalsAndTexture(std::vector<std::string> p_lineTokens);
    void ReadObjFaceWithNormals(std::vector<std::string> p_lineTokens);
    void ReadObjFaceWithTexture(std::vector<std::string> p_lineTokens);
    void ReadObjFaceVertexOnly(std::vector<std::string> p_lineTokens);
};