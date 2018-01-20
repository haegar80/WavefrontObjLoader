#include "MaterialLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>

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
	std::ifstream mtlFile;

	mtlFile.open(p_fileName.c_str(), std::ios_base::in);
	if (!mtlFile) {
		std::cout << "File " << p_fileName.c_str() << " could not be opened!!" << std::endl;
		return;
	}

	while (!mtlFile.eof()) {
		std::string line;
		std::vector<std::string> tokens;

		std::getline(mtlFile, line, '\n');
		std::size_t foundPosSpace = line.find_first_of(' ', 0);
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

	mtlFile.close();
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

void MaterialLoader::EvaluateAndExecuteCommand(std::vector<std::string> p_lineTokens)
{
	if (0 == p_lineTokens[0].compare("newmtl"))
	{
		if (2 == p_lineTokens.size())
		{
			std::string materialName = p_lineTokens[1];
			m_materials.push_back(new Material(materialName));
		}
	}
	else if (0 == p_lineTokens[0].compare("Ka"))
	{
		if (4 == p_lineTokens.size())
		{
			MaterialRGBValue rgbValue = ReadRGBValues(std::move(p_lineTokens));
			m_materials.back()->setAmbientColor(rgbValue);
		}
	}
	else if (0 == p_lineTokens[0].compare("Kd"))
	{
		if (4 == p_lineTokens.size())
		{
			MaterialRGBValue rgbValue = ReadRGBValues(std::move(p_lineTokens));
			m_materials.back()->setDiffuseColor(rgbValue);
		}
	}
	else if (0 == p_lineTokens[0].compare("Ks"))
	{
		if (4 == p_lineTokens.size())
		{
			MaterialRGBValue rgbValue = ReadRGBValues(std::move(p_lineTokens));
			m_materials.back()->setSpecularColor(rgbValue);
		}
	}
	else if (0 == p_lineTokens[0].compare("map_Ka"))
	{
		if (2 == p_lineTokens.size())
		{
			std::string path = "Wavefront/building/" + p_lineTokens[1];
			m_materials.back()->setAmbientTexturePath(path);
		}
	}
	else if (0 == p_lineTokens[0].compare("map_Kd"))
	{
		if (2 == p_lineTokens.size())
		{
			std::string path = "Wavefront/building/" + p_lineTokens[1];
			m_materials.back()->setDiffuseTexturePath(path);
		}
	}
	else if (0 == p_lineTokens[0].compare("map_Ks"))
	{
		if (2 == p_lineTokens.size())
		{
			std::string path = "Wavefront/building/" + p_lineTokens[1];
			m_materials.back()->setSpecularTexturePath(path);
		}
	}
}

MaterialRGBValue MaterialLoader::ReadRGBValues(std::vector<std::string> p_lineTokens)
{
	std::stringstream rValueStream(p_lineTokens[1]);
	std::stringstream gValueStream(p_lineTokens[2]);
	std::stringstream bValueStream(p_lineTokens[3]);

	MaterialRGBValue rgbValue;
	rValueStream >> rgbValue.R;
	gValueStream >> rgbValue.G;
	bValueStream >> rgbValue.B;

	return rgbValue;
}