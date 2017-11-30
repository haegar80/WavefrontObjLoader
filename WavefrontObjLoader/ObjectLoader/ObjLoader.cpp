#include "OBJLoader.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
  
ObjLoader::ObjLoader()
{
}
 
void ObjLoader::LoadOBJ(std::string p_fileName)
{
	std::ifstream objFile;
	ObjTextureCoords texture;
 
	objFile.open(p_fileName.c_str(), std::ios_base::in);
	if(!objFile) {
	    std::cout << "File " << p_fileName.c_str() << " could not be opened!!" << std::endl;
		return;
	}
 
	while(!objFile.eof()) {
		char firstChar;
		std::string line;
		std::vector<std::string> tokens;

		std::getline(objFile, line, '\n');
		std::size_t start = 0;
		std::size_t end = line.length();
		std::size_t foundDelimiterPos = 0;
		while (std::string::npos != (foundDelimiterPos = line.find_first_of(' ', start)))
		{
			if (foundDelimiterPos > start)
			{
				tokens.push_back(line.substr(start, foundDelimiterPos - start));
				start = foundDelimiterPos + 1;
			}
		}
		tokens.push_back(line.substr(start, end));

		if (tokens.size() > 0)
		{
			EvaluateAndExecuteCommand(std::move(tokens));
			tokens.clear();
		}
	}
 
	objFile.close();
}
 
void ObjLoader::DumpOBJ(void)
{
	std::cout << "Dump *.obj content" << std::endl;
	for(unsigned long i = 0; i < m_vertices.size(); i++) {
		std::cout << m_vertices.at(i).X << " " << m_vertices.at(i).Y << " " << m_vertices.at(i).Z << std::endl;
	}
	for(unsigned long i = 0; i < m_normals.size(); i++) {
		std::cout << m_normals.at(i).X << " " << m_normals.at(i).Y << " " << m_normals.at(i).Z << std::endl;
	}
	for(unsigned long i = 0; i < m_uvs.size(); i++) {
		std::cout << m_uvs.at(i).U << " " << m_uvs.at(i).V << std::endl;
	}
	for(unsigned long i = 0; i < m_faces.size(); i++) {
		for(unsigned long j = 0; j < m_faces.at(i).Indices.size(); j++) {
			std::cout << m_faces.at(i).Indices.at(j).VertexIndex << "/" << m_faces.at(i).Indices.at(j).TextureIndex << "/" << m_faces.at(i).Indices.at(j).NormalIndex << " ";
            std::cout << std::endl;
		}
	}
}

void ObjLoader::EvaluateAndExecuteCommand(std::vector<std::string> p_lineTokens)
{
	if (0 == p_lineTokens[0].compare("mtllib"))
	{
		std::string path = p_lineTokens[1];
		MaterialLoader materialLoader;
		materialLoader.LoadMaterial(path);
	}
	else if (0 == p_lineTokens[0].compare("v")) // Add position vertex
	{
		if (4 == p_lineTokens.size())
		{
			ObjVertexCoords vertex = ReadObjVertexCoords(std::move(p_lineTokens));
			m_vertices.push_back(vertex);
		}
	}
	else if (0 == p_lineTokens[0].compare("vn")) // Add normal
	{
		if (4 == p_lineTokens.size())
		{
			ObjVertexCoords vertex = ReadObjVertexCoords(std::move(p_lineTokens));
			m_normals.push_back(vertex);
		}
	}
	else if (0 == p_lineTokens[0].compare("vt")) // Add texture
	{
		if (3 == p_lineTokens.size())
		{
			ObjTextureCoords texture = ReadObjTextureCoords(std::move(p_lineTokens));
			m_uvs.push_back(texture);
		}
	}
	else if (0 == p_lineTokens[0].compare("f")) // Add face
	{
		ObjFace face;
		face.Valid = false;
		if (m_normals.size() > 0 && m_uvs.size() > 0) {
			face = ReadObjFaceWithNormalsAndTexture(std::move(p_lineTokens));
		}
		else if (m_normals.size() > 0 && 0 == m_uvs.size()) {
			face = ReadObjFaceWithNormals(std::move(p_lineTokens));
		}
		else if (0 == m_normals.size() && m_uvs.size() > 0) {
			face = ReadObjFaceWithTexture(std::move(p_lineTokens));
		}
		else if (0 == m_normals.size() && 0 == m_uvs.size()) {
			face = ReadObjFaceVertexOnly(std::move(p_lineTokens));
		}
		if (face.Valid) {
			AddTriangledFace(face);
		}
	}
}

ObjVertexCoords ObjLoader::ReadObjVertexCoords(std::vector<std::string> p_lineTokens)
{
	std::stringstream vertexXStream(p_lineTokens[1]);
	std::stringstream vertexYStream(p_lineTokens[2]);
	std::stringstream vertexZStream(p_lineTokens[3]);

	ObjVertexCoords vertex;
	vertexXStream >> vertex.X;
	vertexYStream >> vertex.Y;
	vertexZStream >> vertex.Z;

	return vertex;
}

ObjTextureCoords ObjLoader::ReadObjTextureCoords(std::vector<std::string> p_lineTokens)
{
	std::stringstream textureUStream(p_lineTokens[1]);
	std::stringstream textureVStream(p_lineTokens[2]);

	ObjTextureCoords texture;
	textureUStream >> texture.U;
	textureVStream >> texture.V;

	return texture;
}

ObjFace ObjLoader::ReadObjFaceWithNormalsAndTexture(std::vector<std::string> p_lineTokens)
{
	ObjFace face;
	face.Valid = false;

	for each(std::string token in p_lineTokens)
	{
		std::size_t countSlashes = std::count(token.begin(), token.end(), '/');
		if (countSlashes > 2)
		{
			std::cout << "ReadObjFaceWithNormalsAndTexture: Invalid number of slashes." << std::endl;
		}
		else if (countSlashes > 0)
		{
			face.Valid = true;
			int vertIndex = 0;
			int uvIndex = 0;
			int normalIndex = 0;

			std::size_t foundPosSlash1 = token.find('/', 0);
			std::size_t foundPosSlash2 = token.find('/', foundPosSlash1 + 1);
			
			unsigned short vertIndexPos = 0;
			unsigned short uvIndexPos = foundPosSlash1 + 1;
			unsigned short normalIndexPos = foundPosSlash2 + 1;

			unsigned short vertIndexSize = foundPosSlash1 - vertIndexPos;
			unsigned short uvIndexSize = foundPosSlash2 - uvIndexPos;
			unsigned short normalIndexSize = token.length() - normalIndexPos;

			std::stringstream vertIndexStream(token.substr(vertIndexPos, vertIndexSize));
			std::stringstream uvIndexStream(token.substr(uvIndexPos, uvIndexSize));
			std::stringstream normalIndexStream(token.substr(normalIndexPos, normalIndexSize));

			vertIndexStream >> vertIndex;
			uvIndexStream >> uvIndex;
			normalIndexStream >> normalIndex;

			ObjFaceIndices indices;
			indices.VertexIndex = vertIndex - 1;
			indices.TextureIndex = uvIndex - 1;
			indices.NormalIndex = normalIndex - 1;
			face.Indices.push_back(indices);
		}
	}

	return face;
}

ObjFace ObjLoader::ReadObjFaceWithNormals(std::vector<std::string> p_lineTokens)
{
	ObjFace face;
	face.Valid = false;

	for each(std::string token in p_lineTokens)
	{
		std::size_t countSlashes = std::count(token.begin(), token.end(), '/');
		if (countSlashes > 2)
		{
			std::cout << "ReadObjFaceWithNormals: Invalid number of slashes." << std::endl;
		}
		else if (countSlashes > 0)
		{
			face.Valid = true;
			int vertIndex = 0;
			int normalIndex = 0;
			std::size_t foundPosSlash = token.find('/', 0);

			unsigned short vertIndexPos = 0;
			unsigned short normalIndexPos = foundPosSlash + 2;

			unsigned short vertIndexSize = foundPosSlash - vertIndexPos;
			unsigned short normalIndexSize = token.length() - normalIndexPos;

			std::stringstream vertIndexStream(token.substr(vertIndexPos, vertIndexSize));
			std::stringstream normalIndexStream(token.substr(normalIndexPos, normalIndexSize));

			vertIndexStream >> vertIndex;
			normalIndexStream >> normalIndex;

			ObjFaceIndices indices;
			indices.VertexIndex = vertIndex - 1;
			indices.NormalIndex = normalIndex - 1;
			face.Indices.push_back(indices);
		}
	}

	return face;
}

ObjFace ObjLoader::ReadObjFaceWithTexture(std::vector<std::string> p_lineTokens)
{
	ObjFace face;
	face.Valid = false;

	for each(std::string token in p_lineTokens)
	{
		std::size_t countSlashes = std::count(token.begin(), token.end(), '/');
		if (countSlashes > 1)
		{
			std::cout << "ReadObjFaceWithTexture: Invalid number of slashes." << std::endl;
		}
		else if (countSlashes > 0)
		{
			face.Valid = true;
			int vertIndex = 0;
			int uvIndex = 0;
			std::size_t foundPosSlash = token.find('/', 0);

			unsigned short vertIndexPos = 0;
			unsigned short uvIndexPos = foundPosSlash + 1;

			unsigned short vertIndexSize = foundPosSlash - vertIndexPos;
			unsigned short uvIndexSize = token.length() - uvIndexPos;

			std::stringstream vertIndexStream(token.substr(vertIndexPos, vertIndexSize));
			std::stringstream uvIndexStream(token.substr(uvIndexPos, uvIndexSize));

			vertIndexStream >> vertIndex;
			uvIndexStream >> uvIndex;

			ObjFaceIndices indices;
			indices.VertexIndex = vertIndex - 1;
			indices.TextureIndex = uvIndex - 1;
			face.Indices.push_back(indices);
		}
	}

	return face;
}

ObjFace ObjLoader::ReadObjFaceVertexOnly(std::vector<std::string> p_lineTokens)
{
	ObjFace face;
	face.Valid = true;
	int vertIndex = 0;

	for each(std::string token in p_lineTokens)
	{
		if (0 != p_lineTokens[0].compare("f"))
		{
			std::stringstream vertIndexStream(token.substr(0, token.length()));

			vertIndexStream >> vertIndex;

			ObjFaceIndices indices;
			indices.VertexIndex = vertIndex - 1;
			face.Indices.push_back(indices);
		}
	}
	return face;
}

void ObjLoader::AddTriangledFace(ObjFace p_originalFace)
{
	ObjFace triangledFace1;
	ObjFaceIndices indices0 = p_originalFace.Indices.at(0);
	ObjFaceIndices indices1 = p_originalFace.Indices.at(1);
	ObjFaceIndices indices2 = p_originalFace.Indices.at(2);
	triangledFace1.Indices.push_back(indices0);
	triangledFace1.Indices.push_back(indices1);
	triangledFace1.Indices.push_back(indices2);

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