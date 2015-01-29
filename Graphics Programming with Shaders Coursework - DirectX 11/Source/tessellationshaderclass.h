#ifndef _TESSELLATIONSHADERCLASS_H_
#define _TESSELLATIONSHADERCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include "lightclass.h"
using namespace std;

class TessellationShaderClass
{
#define NO_OF_LIGHTS 8

private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct TessellationBufferType
	{
		float tessellationFactor; // pass tessellation factor to shader
		D3DXVECTOR3 padding;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 diffuseColor[NO_OF_LIGHTS];
		D3DXVECTOR4 lightDirection[NO_OF_LIGHTS];
		D3DXVECTOR4 specularPower[NO_OF_LIGHTS];
		D3DXVECTOR4 ambientColor[NO_OF_LIGHTS];
		D3DXVECTOR4 specularColor[NO_OF_LIGHTS];
	};

	struct CameraBufferType
	{
		D3DXVECTOR3 cameraPosition;
		float padding;
	};

	struct LightPosBufferType
	{
		D3DXVECTOR4 lightPosition[NO_OF_LIGHTS];
	};

public:
	TessellationShaderClass();
	TessellationShaderClass(const TessellationShaderClass&);
	~TessellationShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, float, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, LightClass*[], D3DXVECTOR3);

private:
	const D3DXVECTOR4 DEFAULT_SPECULAR_POWER; 

	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, float, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, LightClass*[], D3DXVECTOR3);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11HullShader* m_hullShader;
	ID3D11DomainShader* m_domainShader;
	ID3D11PixelShader* m_pixelShader;

	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_lightPosBuffer;
	ID3D11Buffer* m_tessellationBuffer;
};

#endif

