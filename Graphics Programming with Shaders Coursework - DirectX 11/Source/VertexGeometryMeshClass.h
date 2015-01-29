////////////////////////////////////////////////////////////////////////////////
// Filename: VertexGeometryMeshClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GEOMETRYSHADERMESHCLASS_H_
#define _GEOMETRYSHADERMESHCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: VertexGeometryMeshClass
////////////////////////////////////////////////////////////////////////////////
class VertexGeometryMeshClass
{
protected:

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

public:
	VertexGeometryMeshClass();
	VertexGeometryMeshClass(const VertexGeometryMeshClass&);
	~VertexGeometryMeshClass();

	bool Initialize(ID3D11Device*, WCHAR*, WCHAR*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*, ID3D11ShaderResourceView*);

	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView* GetDisplacementMap();
	ID3D11ShaderResourceView* GetBumpMap();
	void GenerateMips(ID3D11DeviceContext*);

protected:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*, ID3D11ShaderResourceView*);
	bool LoadTexture(ID3D11Device*, WCHAR*, WCHAR*, WCHAR*);
	void ReleaseTexture();

protected:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;
	TextureClass* m_displacementMap;
	TextureClass* m_bumpMap;
};

#endif