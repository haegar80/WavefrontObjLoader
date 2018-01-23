#pragma once

#include "Material.h"
#include <vector>

struct ObjFaceIndices
{
	unsigned short VertexIndex;
	unsigned short NormalIndex;
	unsigned short TextureIndex;
};

struct ObjFace
{
	std::vector<ObjFaceIndices> Indices;
};

class SubMesh
{
public:
    SubMesh(Material* p_Material);
    ~SubMesh() = default;

    void AddFace();
    void AddFaceIndices(unsigned short p_vertexIndex, unsigned short p_textureIndex = 0, unsigned short p_normalIndex = 0);

    Material* GetMaterial() const {
        return m_material;
    }

    std::vector<ObjFace> GetFaces() const {
        return m_faces;
    }

private:
    Material* m_material{ nullptr };
    std::vector<ObjFace> m_faces;

    void AddTriangledFace(ObjFace p_originalFace);
};

