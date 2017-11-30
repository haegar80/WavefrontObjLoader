#pragma once

#include "MaterialLoader.h"
#include <vector>
#include <string>

struct ObjVertexCoords
{
    float X;
    float Y;
    float Z;
};

struct ObjTextureCoords
{
    float U;
	float V;
};

struct ObjFaceIndices
{
	unsigned short VertexIndex;
	unsigned short NormalIndex;
	unsigned short TextureIndex;
};

struct ObjFace
{
	std::vector<ObjFaceIndices> Indices;
	bool Valid;
};
 
class ObjLoader{
public:
        ObjLoader();
        virtual ~ObjLoader() = default;
 
		std::vector<ObjVertexCoords> GetVertices() const {
			return m_vertices;
		}
		std::vector<ObjVertexCoords> GetNormals() const {
			return m_normals;
		}
        std::vector<ObjTextureCoords> GetTextures() const {
			return m_uvs;
		}
		std::vector<ObjFace> GetFaces() const {
			return m_faces;
		}
 
        void LoadOBJ(std::string p_fileName);
        void DumpOBJ(void);
 
private:
	std::vector<ObjVertexCoords> m_vertices;
	std::vector<ObjVertexCoords> m_normals;
	std::vector<ObjTextureCoords> m_uvs;
	std::vector<ObjFace> m_faces;

	void EvaluateAndExecuteCommand(std::vector<std::string> p_lineTokens);
	ObjVertexCoords ReadObjVertexCoords(std::vector<std::string> p_lineTokens);
    ObjTextureCoords ReadObjTextureCoords(std::vector<std::string> p_lineTokens);
	ObjFace ReadObjFaceWithNormalsAndTexture(std::vector<std::string> p_lineTokens);
	ObjFace ReadObjFaceWithNormals(std::vector<std::string> p_lineTokens);
	ObjFace ReadObjFaceWithTexture(std::vector<std::string> p_lineTokens);
	ObjFace ReadObjFaceVertexOnly(std::vector<std::string> p_lineTokens);
	void AddTriangledFace(ObjFace p_originalFace);
};