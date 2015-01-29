#include "lightclass.h"


LightClass::LightClass()
{
}


LightClass::LightClass(const LightClass& other)
{
}


LightClass::~LightClass()
{
}


void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}


void LightClass::SetDirection(float x, float y, float z, float w)
{
	m_direction = D3DXVECTOR4(x, y, z, w);
	return;
}


void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void LightClass::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void LightClass::SetLightPosition(float x, float y, float z, float w)
{
	m_lightPosition = D3DXVECTOR4(x, y, z, w);
	return;
}


D3DXVECTOR4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}


D3DXVECTOR4 LightClass::GetDirection()
{
	return m_direction;
}


D3DXVECTOR4 LightClass::GetAmbientColor()
{
	return m_ambientColor; 
}

D3DXVECTOR4 LightClass::GetSpecularColor()
{
	return m_specularColor;
}

D3DXVECTOR4 LightClass::GetLightPosition()
{
	return m_lightPosition;
}