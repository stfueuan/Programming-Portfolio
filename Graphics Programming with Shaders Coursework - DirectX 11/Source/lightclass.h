#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_

#include <d3dx10math.h>

class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float, float);
	void SetAmbientColor(float, float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetLightPosition(float, float, float, float);

	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR4 GetDirection();
	D3DXVECTOR4 GetAmbientColor();
	D3DXVECTOR4 GetSpecularColor();
	D3DXVECTOR4 GetLightPosition();

private:
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR4 m_direction;
	D3DXVECTOR4 m_ambientColor;
	D3DXVECTOR4 m_specularColor;
	D3DXVECTOR4 m_lightPosition;
};

#endif