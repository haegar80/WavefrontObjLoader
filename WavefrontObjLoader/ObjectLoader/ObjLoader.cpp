#include "OBJLoader.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
  
ObjLoader::ObjLoader()
{
	m_currentMaterial = m_materialLoader.GetMaterialByName(std::string("default"));
	Mesh* mesh = new Mesh;
	m_currentMesh = mesh;
	m_meshes.push_back(mesh);
}

ObjLoader::~ObjLoader()
{
	for each(Mesh* mesh in m_meshes)
	{
		delete mesh;
	}
}
 
void ObjLoader::LoadOBJ(std::string& p_fileName)
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

void ObjLoader::EvaluateAndExecuteCommand(std::vector<std::string> p_lineTokens)
{
	if (0 == p_lineTokens[0].compare("o"))
	{
		std::string meshName = p_lineTokens[1];
		Mesh* mesh = new Mesh(meshName);
		m_currentMesh = mesh;
		m_meshes.push_back(mesh);
	}
	else if(0 == p_lineTokens[0].compare("mtllib"))
	{
		if (2 == p_lineTokens.size())
		{
			std::string path = "Wavefront/stove1/" + p_lineTokens[1];
			m_materialLoader.LoadMaterial(path);
		}
	}
	else if (0 == p_lineTokens[0].compare("usemtl")) // Use this material as current
	{
		if (2 == p_lineTokens.size())
		{
			std::string materialName = p_lineTokens[1];
			Material* currentMaterial = m_materialLoader.GetMaterialByName(materialName);
			if (nullptr != currentMaterial) {
				m_currentMaterial = currentMaterial;
			}
		}
	}
	else if (0 == p_lineTokens[0].compare("v")) // Add position vertex
	{
		if (4 == p_lineTokens.size())
		{
			ReadObjVertexCoords(std::move(p_lineTokens), true);

			// Reset old values
			m_hasNormals = false;
			m_hasTextures = false;
		}
	}
	else if (0 == p_lineTokens[0].compare("vn")) // Add normal
	{
		if (4 == p_lineTokens.size())
		{
			ReadObjVertexCoords(std::move(p_lineTokens), false);
			m_hasNormals = true;
		}
	}
	else if (0 == p_lineTokens[0].compare("vt")) // Add texture
	{
		if (3 == p_lineTokens.size())
		{
			ReadObjTextureCoords(std::move(p_lineTokens));
			m_hasTextures = true;
		}
	}
	else if (0 == p_lineTokens[0].compare("f")) // Add face
	{
		if (m_hasNormals && m_hasTextures) {
			ReadObjFaceWithNormalsAndTexture(std::move(p_lineTokens));
		}
		else if (m_hasNormals && !m_hasTextures) {
			ReadObjFaceWithNormals(std::move(p_lineTokens));
		}
		else if (!m_hasNormals && m_hasTextures) {
			ReadObjFaceWithTexture(std::move(p_lineTokens));
		}
		else if (!m_hasNormals && !m_hasTextures) {
			ReadObjFaceVertexOnly(std::move(p_lineTokens));
		}
	}
}

void ObjLoader::ReadObjVertexCoords(std::vector<std::string> p_lineTokens, bool p_isPosition)
{
	std::stringstream vertexXStream(p_lineTokens[1]);
	std::stringstream vertexYStream(p_lineTokens[2]);
	std::stringstream vertexZStream(p_lineTokens[3]);

	float x;
	float y;
	float z;

	vertexXStream >> x;
	vertexYStream >> y;
	vertexZStream >> z;

	if (p_isPosition) {
		m_currentMesh->AddVertex(x, y, z);
	}
	else {
		m_currentMesh->AddNormal(x, y, z);
	}
}

void ObjLoader::ReadObjTextureCoords(std::vector<std::string> p_lineTokens)
{
	std::stringstream textureUStream(p_lineTokens[1]);
	std::stringstream textureVStream(p_lineTokens[2]);

	float u;
	float v;

	textureUStream >> u;
	textureVStream >> v;

	m_currentMesh->AddTexture(u, v);
}

void ObjLoader::ReadObjFaceWithNormalsAndTexture(std::vector<std::string> p_lineTokens)
{
	bool needsCreatingFace = true;
	for each(std::string token in p_lineTokens)
	{
		std::size_t countSlashes = std::count(token.begin(), token.end(), '/');
		if (countSlashes > 2)
		{
			std::cout << "ReadObjFaceWithNormalsAndTexture: Invalid number of slashes." << std::endl;
		}
		else if (countSlashes > 0)
		{
			if (needsCreatingFace)
			{
				m_currentMesh->AddFace(m_currentMaterial);
				needsCreatingFace = false;
			}

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

			m_currentMesh->AddFaceIndices(vertIndex - 1, uvIndex - 1, normalIndex - 1);
		}
	}
}

void ObjLoader::ReadObjFaceWithNormals(std::vector<std::string> p_lineTokens)
{
	bool needsCreatingFace = true;
	for each(std::string token in p_lineTokens)
	{
		std::size_t countSlashes = std::count(token.begin(), token.end(), '/');
		if (countSlashes > 2)
		{
			std::cout << "ReadObjFaceWithNormals: Invalid number of slashes." << std::endl;
		}
		else if (countSlashes > 0)
		{
			if (needsCreatingFace)
			{
				m_currentMesh->AddFace(m_currentMaterial);
				needsCreatingFace = false;
			}

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

			m_currentMesh->AddFaceIndices(vertIndex - 1, 0, normalIndex - 1);
		}
	}
}

void ObjLoader::ReadObjFaceWithTexture(std::vector<std::string> p_lineTokens)
{
	bool needsCreatingFace = true;
	for each(std::string token in p_lineTokens)
	{
		std::size_t countSlashes = std::count(token.begin(), token.end(), '/');
		if (countSlashes > 1)
		{
			std::cout << "ReadObjFaceWithTexture: Invalid number of slashes." << std::endl;
		}
		else if (countSlashes > 0)
		{
			if (needsCreatingFace)
			{
				m_currentMesh->AddFace(m_currentMaterial);
				needsCreatingFace = false;
			}

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

			m_currentMesh->AddFaceIndices(vertIndex - 1, uvIndex - 1, 0);
		}
	}
}

void ObjLoader::ReadObjFaceVertexOnly(std::vector<std::string> p_lineTokens)
{
	bool needsCreatingFace = true;
	int vertIndex = 0;

	for each(std::string token in p_lineTokens)
	{
		if (0 == token.compare("f"))
		{
			continue;
		}

		if (needsCreatingFace)
		{
			m_currentMesh->AddFace(m_currentMaterial);
			needsCreatingFace = false;
		}

		std::stringstream vertIndexStream(token.substr(0, token.length()));

		vertIndexStream >> vertIndex;

		m_currentMesh->AddFaceIndices(vertIndex - 1, 0, 0);
	}
}