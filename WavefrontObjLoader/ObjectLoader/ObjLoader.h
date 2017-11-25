#ifndef _ObjLoader_H
#define _ObjLoader_H
 
#include <fstream>
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

struct ObjFace
{
	std::vector<long> Vertices;
    std::vector<long> Normals;
    std::vector<long> UVs;
	bool Valid;
};
 
class ObjLoader{
public:
        ObjLoader();
        ~ObjLoader();
 
        long GetNumVertices() const;
        long GetNumNormals() const;
        long GetNumUVs() const;
        long GetNumFaces() const;
 
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
 
        void LoadOBJ(std::string p_file_name);
        void DumpOBJ(void);
 
private:
	ObjVertexCoords ReadObjVertexCoords(std::ifstream& p_file);
        ObjTextureCoords ReadObjTextureCoords(std::ifstream& p_file);
		ObjFace ReadObjFaceWithNormalsAndTexture(std::ifstream& p_file);
		ObjFace ReadObjFaceWithNormals(std::ifstream& p_file);
		ObjFace ReadObjFaceWithTexture(std::ifstream& p_file);
		ObjFace ReadObjFace(std::ifstream& p_file);

		std::vector<ObjVertexCoords> m_vertices;
        std::vector<ObjVertexCoords> m_normals;
        std::vector<ObjTextureCoords> m_uvs;
        std::vector<ObjFace> m_faces;
};
 
#endif