#include "SubMesh.h"

SubMesh::SubMesh(Material* p_material) :
    m_material(p_material)
{
}

void SubMesh::AddFace()
{
    std::vector<ObjFaceIndices> indices;
    ObjFace face;
    face.Indices = std::move(indices);
    m_faces.push_back(face);
}

void SubMesh::AddFaceIndices(unsigned short p_vertexIndex, unsigned short p_textureIndex, unsigned short p_normalIndex)
{
    ObjFaceIndices indices;
    indices.VertexIndex = p_vertexIndex;
    indices.TextureIndex = p_textureIndex;
    indices.NormalIndex = p_normalIndex;
    ObjFace& face = m_faces.back();
    face.Indices.push_back(indices);
    if (face.Indices.size() > 3)
    {
        AddTriangledFace(face);
    }
}

void SubMesh::AddTriangledFace(ObjFace p_originalFace)
{
    ObjFace triangledFace1;
    ObjFaceIndices indices0 = p_originalFace.Indices.at(0);
    ObjFaceIndices indices1 = p_originalFace.Indices.at(1);
    ObjFaceIndices indices2 = p_originalFace.Indices.at(2);
    triangledFace1.Indices.push_back(indices0);
    triangledFace1.Indices.push_back(indices1);
    triangledFace1.Indices.push_back(indices2);

    m_faces.pop_back();
    m_faces.push_back(triangledFace1);

    constexpr uint8_t TriangleSize = 3;
    if (p_originalFace.Indices.size() > TriangleSize)
    {
        for (int i = 3; i < p_originalFace.Indices.size(); i++)
        {
            ObjFace triangledFaceN;
            indices1 = indices2;
            indices2 = p_originalFace.Indices.at(i);
            triangledFaceN.Indices.push_back(indices0);
            triangledFaceN.Indices.push_back(indices1);
            triangledFaceN.Indices.push_back(indices2);

            m_faces.push_back(triangledFaceN);
        }
    }
}