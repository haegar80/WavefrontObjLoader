#pragma once

#include "Material.h"
#include <string>
#include <vector>

class MaterialLoader
{
public:
    MaterialLoader();
    virtual ~MaterialLoader();

    void LoadMaterial(std::string& p_dirPath, std::string& p_fileName);

    Material* GetMaterialByName(const std::string& p_name);

private:
    std::string m_currentDirPath{ "" };
    std::vector<Material*> m_materials;

    void EvaluateAndExecuteCommand(std::vector<std::string> p_lineTokens);
    MaterialRGBValue ReadRGBValues(std::vector<std::string> p_lineTokens);
};

