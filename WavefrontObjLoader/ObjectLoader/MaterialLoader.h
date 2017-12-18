#pragma once

#include "Material.h"
#include <string>
#include <vector>

class MaterialLoader
{
public:
	MaterialLoader();
	virtual ~MaterialLoader();

	void LoadMaterial(std::string& p_fileName);

	Material* GetMaterialByName(std::string& p_name);

private:
	std::vector<Material*> m_materials;
};

