#pragma once

#include "Material.h"
#include <string>
#include <vector>

class MaterialLoader
{
public:
	MaterialLoader();
	~MaterialLoader() = default;

	void LoadMaterial(std::string p_fileName);

private:
	std::vector<Material*> m_Materials;
};

