#pragma once

#include <QtGui/QVector3D>
#include <string>

class Material
{
public:
	Material(std::string p_name);
	virtual ~Material() = default;

	std::string getName()
	{
		return m_name;
	}

	QVector3D getAmbientColor()
	{
		return m_ambientColor;
	}

	void setAmbientColor(float p_r, float p_g, float p_b)
	{
		m_ambientColor.setX(p_r);
		m_ambientColor.setY(p_g);
		m_ambientColor.setZ(p_b);
	}

	QVector3D getDiffuseColor()
	{
		return m_diffuseColor;
	}

	void setDiffuseColor(float p_r, float p_g, float p_b)
	{
		m_diffuseColor.setX(p_r);
		m_diffuseColor.setY(p_g);
		m_diffuseColor.setZ(p_b);
	}

	QVector3D getSpecularColor()
	{
		return m_specularColor;
	}

	void setSpecularColor(float p_r, float p_g, float p_b)
	{
		m_specularColor.setX(p_r);
		m_specularColor.setY(p_g);
		m_specularColor.setZ(p_b);
	}

private:
	std::string& m_name;

	QVector3D m_ambientColor;
	QVector3D m_diffuseColor;
	QVector3D m_specularColor;
};

