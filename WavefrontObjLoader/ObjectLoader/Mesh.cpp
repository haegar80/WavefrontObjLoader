#include "Mesh.h"

Mesh::Mesh() :
	m_name(std::string("default"))
{
}

Mesh::Mesh(std::string& p_name) : 
	m_name(p_name)
{
}

Mesh::~Mesh()
{
	for each(SubMesh* subMesh in m_submeshes)
	{
		delete subMesh;
	}
}

void Mesh::AddVertex(float p_x, float p_y, float p_z)
{
	ObjVertexCoords vertex;
	vertex.X = p_x;
	vertex.Y = p_y;
	vertex.Z = p_z;
	m_tempVertices.push_back(vertex);
}
void Mesh::AddNormal(float p_x, float p_y, float p_z)
{
	ObjVertexCoords normal;
	normal.X = p_x;
	normal.Y = p_y;
	normal.Z = p_z;
	m_tempNormals.push_back(normal);
}

void Mesh::AddTexture(float p_u, float p_v)
{
	ObjTextureCoords texture;
	texture.U = p_u;
	texture.V = p_v;
	m_tempTextures.push_back(texture);
}

void Mesh::AddFace(Material* p_material)
{
	auto subMeshWithMatchingMaterial = std::find_if(m_submeshes.begin(), m_submeshes.end(), [p_material] (SubMesh* submesh) {return p_material == submesh->GetMaterial(); });
	
	if (m_submeshes.end() == subMeshWithMatchingMaterial)
	{
		m_submeshes.push_back(new SubMesh(p_material));
	}
	m_submeshes.back()->AddFace();
}

void Mesh::AddFaceIndices(unsigned short p_vertexIndex, unsigned short p_textureIndex, unsigned short p_normalIndex)
{
	AddVertexFromFaceIndex(p_vertexIndex);
	AddTextureFromFaceIndex(p_textureIndex);
	AddNormalFromFaceIndex(p_normalIndex);
	m_submeshes.back()->AddFaceIndices(m_totalFaceIndices, m_totalFaceIndices, m_totalFaceIndices);
	m_totalFaceIndices++;
}

void Mesh::AddVertexFromFaceIndex(unsigned short p_vertexIndex)
{
	m_vertices.push_back(m_tempVertices.at(p_vertexIndex));
}

void Mesh::AddTextureFromFaceIndex(unsigned short p_textureIndex)
{
	m_textures.push_back(m_tempTextures.at(p_textureIndex));
}

void Mesh::AddNormalFromFaceIndex(unsigned short p_normalIndex)
{
	m_normals.push_back(m_tempNormals.at(p_normalIndex));
}