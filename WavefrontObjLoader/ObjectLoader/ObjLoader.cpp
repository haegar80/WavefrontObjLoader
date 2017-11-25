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
				face = ReadObjFace(objFile);
			}
			if(face.Valid) {
				m_faces.push_back(face);
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
		for(unsigned long j = 0; j < m_faces.at(i).Vertices.size(); j++) {
			std::cout << m_faces.at(i).Vertices.at(j) << "/" << m_faces.at(i).UVs.at(j) << "/" << m_faces.at(i).Normals.at(j) << " ";
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
		std::cout << "ReadObjFaceWithNormalsAndTexture: Invalid vertex index." << std::endl;
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
	    for (unsigned int i = 0; i < countSlashes; i ++) {
		    foundPosSlash1 = faceLine.find('/', foundPosSlash1);
			foundPosSlash1++;
			foundPosSlash2 = faceLine.find('/', foundPosSlash1);
			foundPosSpace = faceLine.find(' ', foundPosSpace);
			std::size_t foundNextPosSpace = faceLine.find(' ', foundPosSpace + 1);
			if(std::string::npos == foundNextPosSpace) {
				foundNextPosSpace = faceLine.length();
			}
			unsigned int vertIndexSize = foundPosSlash1 - foundPosSpace - 1;
			unsigned int uvIndexSize = foundPosSlash2 - foundPosSlash1 - 1;
			unsigned int normalIndexSize = foundNextPosSpace - foundPosSlash2 - 1;
			std::stringstream vertIndexStream(faceLine.substr(foundPosSlash1 - vertIndexSize, vertIndexSize));
	        std::stringstream uvIndexStream(faceLine.substr(foundPosSlash1 + 1, uvIndexSize));
			std::stringstream normalIndexStream(faceLine.substr(foundPosSlash2 + 1, normalIndexSize));
			
			foundPosSlash1 = foundPosSlash2 + 1;
			foundPosSpace++;
			vertIndexStream >> vertIndex;
			uvIndexStream >> uvIndex;
			normalIndexStream >> normalIndex;

			face.Vertices.push_back(vertIndex - 1);
			face.UVs.push_back(uvIndex - 1);
			face.Normals.push_back(normalIndex - 1);
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
	if(countSlashes > countSpaces) {
		std::cout << "ReadObjFaceWithNormals: Invalid vertex index." << std::endl;
		face.Valid = false;
	}
	else {
		face.Valid = true;
		int vertIndex = 0;
        int normalIndex = 0;
		std::size_t foundPosSlash = 0;
		std::size_t foundPosSpace = 0;
	    for (unsigned int i = 0; i < countSlashes; i ++) {
		    foundPosSlash = faceLine.find('/', foundPosSlash);
			foundPosSpace = faceLine.find(' ', foundPosSpace);
			std::size_t foundNextPosSpace = faceLine.find(' ', foundPosSpace + 1);
			if(std::string::npos == foundNextPosSpace) {
				foundNextPosSpace = faceLine.length();
			}
			unsigned int vertIndexSize = foundPosSlash - foundPosSpace - 1;
			unsigned int normalIndexSize = foundNextPosSpace - foundPosSlash - 1;
			std::stringstream vertIndexStream(faceLine.substr(foundPosSlash - vertIndexSize, vertIndexSize));
	        std::stringstream normalIndexStream(faceLine.substr(foundPosSlash + 1, normalIndexSize));
			
			foundPosSlash++;
			foundPosSpace++;
			vertIndexStream >> vertIndex;
			normalIndexStream >> normalIndex;

			face.Vertices.push_back(vertIndex - 1);
			face.Normals.push_back(normalIndex - 1);
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
	else {
		face.Valid = true;
		int vertIndex = 0;
        int uvIndex = 0;
		std::size_t foundPosSlash = 0;
		std::size_t foundPosSpace = 0;
	    for (unsigned int i = 0; i < countSlashes; i ++) {
		    foundPosSlash = faceLine.find('/', foundPosSlash);
			foundPosSpace = faceLine.find(' ', foundPosSpace);
			std::size_t foundNextPosSpace = faceLine.find(' ', foundPosSpace + 1);
			if(std::string::npos == foundNextPosSpace) {
				foundNextPosSpace = faceLine.length();
			}
			unsigned int vertIndexSize = foundPosSlash - foundPosSpace - 1;
			unsigned int uvIndexSize = foundNextPosSpace - foundPosSlash - 1;
			std::stringstream vertIndexStream(faceLine.substr(foundPosSlash - vertIndexSize, vertIndexSize));
	        std::stringstream uvIndexStream(faceLine.substr(foundPosSlash + 1, uvIndexSize));
			
			foundPosSlash++;
			foundPosSpace++;
			vertIndexStream >> vertIndex;
			uvIndexStream >> uvIndex;

			face.Vertices.push_back(vertIndex - 1);
			face.UVs.push_back(uvIndex - 1);
		}
	}

	return face;
}

ObjFace ObjLoader::ReadObjFace(std::ifstream& p_file)
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

		face.Vertices.push_back(vertIndex - 1);
	}

	return face;
}