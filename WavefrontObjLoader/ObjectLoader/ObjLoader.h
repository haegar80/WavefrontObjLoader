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
	std::vector<unsigned short> Vertices;
    std::vector<unsigned short> Normals;
    std::vector<unsigned short> UVs;
	bool Valid;
};
 
class ObjLoader{
public:
        ObjLoader(int p_xWindowSize, int p_yWindowsSize);
        ~ObjLoader();
 
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
	int m_xWindowSize{ 0 };
	int m_yWindowSize{ 0 };
	float m_scaleFactor{ 0.0f };
	bool m_scaleFactorDetermined{ false };

	std::vector<ObjVertexCoords> m_vertices;
	std::vector<ObjVertexCoords> m_normals;
	std::vector<ObjTextureCoords> m_uvs;
	std::vector<ObjFace> m_faces;

	void DetermineScaleFactor(float xVertex, float yVertex);

	ObjVertexCoords ReadObjVertexCoords(std::ifstream& p_file);
    ObjTextureCoords ReadObjTextureCoords(std::ifstream& p_file);
	ObjFace ReadObjFaceWithNormalsAndTexture(std::ifstream& p_file);
	ObjFace ReadObjFaceWithNormals(std::ifstream& p_file);
	ObjFace ReadObjFaceWithTexture(std::ifstream& p_file);
	ObjFace ReadObjFace(std::ifstream& p_file);
};
 
#endif