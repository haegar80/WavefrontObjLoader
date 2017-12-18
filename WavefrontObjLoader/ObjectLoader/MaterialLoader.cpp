#include "MaterialLoader.h"
#include <iostream>
#include <fstream>

MaterialLoader::MaterialLoader()
{
	m_materials.push_back(new Material(std::string("default")));
}

MaterialLoader::~MaterialLoader()
{
	for each(Material* material in m_materials)
	{
		delete material;
	}
}

void MaterialLoader::LoadMaterial(std::string& p_fileName)
{
	std::ifstream objFile;

	objFile.open(p_fileName.c_str(), std::ios_base::in);
	if (!objFile) {
		std::cout << "File " << p_fileName.c_str() << " could not be opened!!" << std::endl;
		return;
	}

	while (!objFile.eof()) {
		std::string line;
		std::getline(objFile, line);
		std::size_t foundPosSpace = line.find_first_of(' ', 0);
		if (foundPosSpace > 0)
		{
			std::string command = line.substr(0, foundPosSpace);
			if (0 == command.compare("newmtl"))
			{
				std::string name = line.substr(foundPosSpace + 1, line.length() - (foundPosSpace + 1));
				m_materials.push_back(new Material(name));
			}
		}

	}

	objFile.close();
}

Material* MaterialLoader::GetMaterialByName(std::string& p_name)
{
	Material* foundMaterial{ nullptr };
	for each (Material* material in m_materials)
	{
		if (0 == material->getName().compare(p_name))
		{
			foundMaterial = material;
			break;
		}
	}
	return foundMaterial;
}
