#include <cstdio>
#include <iostream>
#include <sstream>
#include "OBJLoader.h"
  
ObjLoader::ObjLoader()
{
}
 
ObjLoader::~ObjLoader()
{
}
 
void ObjLoader::LoadOBJ(std::string file_name)
{
	//FILE* objFile;
	std::ifstream objFile; 
	bool has_normals = false;
	bool has_uvs = false;
	ObjVertexCoords vertex;
	ObjTextureCoords texture;
 
	objFile.open(file_name.c_str(), std::ios_base::in);
	if(!objFile) {
	    std::cout << "File " << file_name.c_str() << " could not be opened!!" << std::endl;
		return;
	}
 
	while(!objFile.eof()) {
		char firstChar;
		std::string line;
		objFile.get(firstChar);
		switch(firstChar) {
		case '#': // Comment. Fall through
		case 'u': // Fall through
		case 's': // Fall through
		case 'g': // Group. Not supported. Fall through
		default:
			std::getline(objFile, line); // Skip to the next line
			break;
		case 'v':
			char secondChar;
			objFile.get(secondChar); // The next character determines what type of vertex we are loading
			switch(secondChar)
			{
			case ' ': // Loading vertices
				vertex = ReadObjVertexCoords(objFile);
				m_vertices.push_back(vertex);
				break;
			case 'n': // Loading normals
				has_normals = true;
				vertex = ReadObjVertexCoords(objFile);
                m_normals.push_back(vertex);
				break;
			case 't': // Loading UVs
				has_uvs = true;
				texture = ReadObjTextureCoords(objFile);
				m_uvs.push_back(texture);
				break;
			default: // Uh oh... What are we trying to read here? Someone screwed up their OBJ exporter...
				std::cout << "Invalid vertex type: " << "v" << secondChar << " Should be of type \"v \", \"vn\" or \"vt\"." << std::endl;
				return;
				break;
			}
			break;
		case 'f':
			ObjFace face;
			face.Valid = false;
			if(has_normals && has_uvs) {
				face = ReadObjFaceWithNormalsAndTexture(objFile);
			} else if(has_normals && !has_uvs) {
				face = ReadObjFaceWithNormals(objFile);
			} else if(!has_normals && has_uvs) {
				face = ReadObjFaceWithTexture(objFile);
			} else if(!has_normals && !has_uvs) {
				face = ReadObjFaceVertexOnly(objFile);
			}
			if(face.Valid) {
				AddTriangledFace(face);
			}
			break;
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

ObjVertexCoords ObjLoader::ReadObjVertexCoords(std::ifstream& p_file)
{
	std::string vertexX;
	std::getline(p_file, vertexX, ' ');
	std::string vertexY;
	std::getline(p_file, vertexY, ' ');
	std::string vertexZ;
	std::getline(p_file, vertexZ, '\n');
	std::size_t foundPos = vertexZ.find(' ');
	if(std::string::npos != foundPos) {
		vertexZ = vertexZ.substr(foundPos);
	}
	
	std::stringstream vertexXStream(vertexX);
	std::stringstream vertexYStream(vertexY);
	std::stringstream vertexZStream(vertexZ);

	ObjVertexCoords vertex;
	vertexXStream >> vertex.X;
	vertexYStream >> vertex.Y;
	vertexZStream >> vertex.Z;

	return vertex;
}

ObjTextureCoords ObjLoader::ReadObjTextureCoords(std::ifstream& p_file)
{
	std::string ignore;
	std::getline(p_file, ignore, ' ');
	std::string textureU;
	std::getline(p_file, textureU, ' ');
	std::string textureV;
	std::getline(p_file, textureV, ' ');
	std::size_t foundPos = textureV.find(' ');
	if(std::string::npos != foundPos) {
		textureV = textureV.substr(foundPos);
	}

	std::stringstream textureUStream(textureU);
	std::stringstream textureVStream(textureV);

	ObjTextureCoords texture;
	textureUStream >> texture.U;
	textureVStream >> texture.V;

	return texture;
}

ObjFace ObjLoader::ReadObjFaceWithNormalsAndTexture(std::ifstream& p_file)
{
	std::string faceLine;
	std::getline(p_file, faceLine, '\n');
	std::size_t countSlashes = std::count(faceLine.begin(), faceLine.end(), '/');
	std::size_t countSpaces = std::count(faceLine.begin(), faceLine.end(), ' ');
	ObjFace face;
	if(countSlashes > (2 * countSpaces)) {
		std::cout << "ReadObjFaceWithNormalsAndTexture: Invalid number of slashes." << std::endl;
		face.Valid = false;
	}
	else if (0 == countSlashes)
	{
		face.Valid = false;
	}
	else {
		face.Valid = true;
		int vertIndex = 0;
		int uvIndex = 0;
        int normalIndex = 0;
		std::size_t foundPosSlash1 = 0;
		std::size_t foundPosSlash2 = 0;
		std::size_t foundPosSpace = 0;
		std::size_t foundNextPosSpace = 0;
	    for (unsigned int i = 0; foundNextPosSpace < faceLine.length(); i ++) {
		    foundPosSlash1 = faceLine.find('/', foundPosSlash1);
			foundPosSlash2 = faceLine.find('/', foundPosSlash1 + 1);
			foundPosSpace = faceLine.find(' ', foundPosSpace);
			foundNextPosSpace = faceLine.find(' ', foundPosSpace + 1);
			if(std::string::npos == foundNextPosSpace) {
				foundNextPosSpace = faceLine.length();
			}

			unsigned short vertIndexPos = foundPosSpace + 1;
			unsigned short uvIndexPos = foundPosSlash1 + 1;
			unsigned short normalIndexPos = foundPosSlash2 + 1;

			unsigned short vertIndexSize = foundPosSlash1 - vertIndexPos;
			unsigned short uvIndexSize = foundPosSlash2 - uvIndexPos;
			unsigned short normalIndexSize = foundNextPosSpace - normalIndexPos;

			std::stringstream vertIndexStream(faceLine.substr(vertIndexPos, vertIndexSize));
	        std::stringstream uvIndexStream(faceLine.substr(uvIndexPos, uvIndexSize));
			std::stringstream normalIndexStream(faceLine.substr(normalIndexPos, normalIndexSize));
			
			foundPosSlash1 = foundPosSlash2 + 1;
			foundPosSpace = foundNextPosSpace;
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

ObjFace ObjLoader::ReadObjFaceWithNormals(std::ifstream& p_file)
{
	std::string faceLine;
	std::getline(p_file, faceLine, '\n');
	std::size_t countSlashes = std::count(faceLine.begin(), faceLine.end(), '/');
	std::size_t countSpaces = std::count(faceLine.begin(), faceLine.end(), ' ');
	ObjFace face;
	if(countSlashes > (2 * countSpaces)) {
		std::cout << "ReadObjFaceWithNormals: Invalid number of slashes." << std::endl;
		face.Valid = false;
	}
	else if (0 == countSlashes)
	{
		face.Valid = false;
	}
	else {
		face.Valid = true;
		int vertIndex = 0;
        int normalIndex = 0;
		std::size_t foundPosSlash = 0;
		std::size_t foundPosSpace = 0;
		std::size_t foundNextPosSpace = 0;
	    for (unsigned int i = 0; foundNextPosSpace < faceLine.length(); i ++) {
		    foundPosSlash = faceLine.find('/', foundPosSlash);
			foundPosSpace = faceLine.find(' ', foundPosSpace);
			foundNextPosSpace = faceLine.find(' ', foundPosSpace + 1);
			if(std::string::npos == foundNextPosSpace) {
				foundNextPosSpace = faceLine.length();
			}

			unsigned short vertIndexPos = foundPosSpace + 1;
			unsigned short normalIndexPos = foundPosSlash + 2;

			unsigned short vertIndexSize = foundPosSlash - vertIndexPos;
			unsigned short normalIndexSize = foundNextPosSpace - normalIndexPos;

			std::stringstream vertIndexStream(faceLine.substr(vertIndexPos, vertIndexSize));
	        std::stringstream normalIndexStream(faceLine.substr(normalIndexPos, normalIndexSize));
			
			foundPosSlash += 2;
			foundPosSpace = foundNextPosSpace;
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

ObjFace ObjLoader::ReadObjFaceWithTexture(std::ifstream& p_file)
{
	std::string faceLine;
	std::getline(p_file, faceLine, '\n');
	std::size_t countSlashes = std::count(faceLine.begin(), faceLine.end(), '/');
	std::size_t countSpaces = std::count(faceLine.begin(), faceLine.end(), ' ');
	ObjFace face;
	if(countSlashes > countSpaces) {
		std::cout << "ReadObjFaceWithTexture: Invalid vertex index." << std::endl;
		face.Valid = false;
	}
	else if (0 == countSlashes)
	{
		face.Valid = false;
	}
	else {
		face.Valid = true;
		int vertIndex = 0;
        int uvIndex = 0;
		std::size_t foundPosSlash = 0;
		std::size_t foundPosSpace = 0;
		std::size_t foundNextPosSpace = 0;
		for (unsigned int i = 0; foundNextPosSpace < faceLine.length(); i++) {
		    foundPosSlash = faceLine.find('/', foundPosSlash);
			foundPosSpace = faceLine.find(' ', foundPosSpace);
			foundNextPosSpace = faceLine.find(' ', foundPosSpace + 1);
			if(std::string::npos == foundNextPosSpace) {
				foundNextPosSpace = faceLine.length();
			}

			unsigned short vertIndexPos = foundPosSpace + 1;
			unsigned short uvIndexPos = foundPosSlash + 1;

			unsigned short vertIndexSize = foundPosSlash - vertIndexPos;
			unsigned short uvIndexSize = foundNextPosSpace - uvIndexPos;

			std::stringstream vertIndexStream(faceLine.substr(vertIndexPos, vertIndexSize));
	        std::stringstream uvIndexStream(faceLine.substr(uvIndexPos, uvIndexSize));
			
			foundPosSlash++;
			foundPosSpace = foundNextPosSpace;
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

ObjFace ObjLoader::ReadObjFaceVertexOnly(std::ifstream& p_file)
{
	std::string faceLine;
	std::getline(p_file, faceLine, '\n');
	std::size_t countSpaces = std::count(faceLine.begin(), faceLine.end(), ' ');
	ObjFace face;
	face.Valid = true;
	int vertIndex = 0;

	std::size_t foundPosSpace = 0;
	for (unsigned int i = 0; i < countSpaces; i ++) {
		foundPosSpace = faceLine.find(' ', foundPosSpace);
		std::size_t foundNextPosSpace = faceLine.find(' ', foundPosSpace + 1);
		if(std::string::npos == foundNextPosSpace) {
			foundNextPosSpace = faceLine.length();
		}
		unsigned int vertIndexSize = foundNextPosSpace - foundPosSpace - 1;
		std::stringstream vertIndexStream(faceLine.substr(foundPosSpace + 1, vertIndexSize));
			
		foundPosSpace++;
		vertIndexStream >> vertIndex;

		ObjFaceIndices indices;
		indices.VertexIndex = vertIndex - 1;
		face.Indices.push_back(indices);
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