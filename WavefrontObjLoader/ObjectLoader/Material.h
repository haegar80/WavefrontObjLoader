#pragma once

#include <QtGui/QVector3D>
#include <string>

struct MaterialRGBValue
{
	float R;
	float G;
	float B;
};

class Material
{
public:
	Material(std::string& p_name);
	virtual ~Material() = default;

	std::string& getName()
	{
		return m_name;
	}

	MaterialRGBValue getAmbientColor()
	{
		return m_ambientColor;
	}

	void setAmbientColor(MaterialRGBValue p_rgbValue)
	{
		m_ambientColor = p_rgbValue;
	}

	MaterialRGBValue getDiffuseColor()
	{
		return m_diffuseColor;
	}

	void setDiffuseColor(MaterialRGBValue p_rgbValue)
	{
		m_diffuseColor = p_rgbValue;
	}

	MaterialRGBValue getSpecularColor()
	{
		return m_specularColor;
	}

	void setSpecularColor(MaterialRGBValue p_rgbValue)
	{
		m_specularColor = p_rgbValue;
	}

private:
	std::string& m_name;

	MaterialRGBValue m_ambientColor;
	MaterialRGBValue m_diffuseColor;
	MaterialRGBValue m_specularColor;
};

