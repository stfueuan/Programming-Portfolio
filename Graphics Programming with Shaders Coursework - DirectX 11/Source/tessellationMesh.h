#ifndef _TESSELLATIONMESHCLASS_H_
#define _TESSELLATIONMESHCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <math.h>
#include "meshclass.h"

class TessellationMeshClass : public MeshClass
{
private:

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
	};

public:
	TessellationMeshClass();
	TessellationMeshClass(const TessellationMeshClass&);
	~TessellationMeshClass();

	bool Initialize(ID3D11Device*, WCHAR*, WCHAR*, WCHAR*);
	void Render(ID3D11DeviceContext*);
	
private:
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);

};

#endif