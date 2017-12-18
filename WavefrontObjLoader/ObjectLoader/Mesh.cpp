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

void Mesh::AddVertices(float p_x, float p_y, float p_z)
{
	ObjVertexCoords vertex;
	vertex.X = p_x;
	vertex.Y = p_y;
	vertex.Z = p_z;
	m_vertices.push_back(vertex);
}
void Mesh::AddNormals(float p_x, float p_y, float p_z)
{
	ObjVertexCoords normal;
	normal.X = p_x;
	normal.Y = p_y;
	normal.Z = p_z;
	m_normals.push_back(normal);
}

void Mesh::AddTextures(float p_u, float p_v)
{
	ObjTextureCoords texture;
	texture.U = p_u;
	texture.V = p_v;
	m_textures.push_back(texture);
}

void Mesh::AddFace(Material* p_material)
{
	auto materialFound = std::find_if(m_submeshes.begin(), m_submeshes.end(), [p_material] (SubMesh* submesh) {return p_material == submesh->GetMaterial(); });
	
	if (m_submeshes.end() == materialFound)
	{
		m_submeshes.push_back(new SubMesh(p_material));
	}
	m_submeshes.back()->AddFace();
}

void Mesh::AddFaceIndices(unsigned short p_vertexIndex, unsigned short p_textureIndex, unsigned short p_normalIndex)
{
	m_submeshes.back()->AddFaceIndices(p_vertexIndex, p_textureIndex, p_normalIndex);
}