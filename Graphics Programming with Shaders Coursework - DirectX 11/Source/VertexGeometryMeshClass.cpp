////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "VertexGeometryMeshClass.h"

VertexGeometryMeshClass::VertexGeometryMeshClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_displacementMap = 0;
	m_bumpMap = 0;
}


VertexGeometryMeshClass::VertexGeometryMeshClass(const VertexGeometryMeshClass& other)
{
}

VertexGeometryMeshClass::~VertexGeometryMeshClass()
{
}

bool VertexGeometryMeshClass::Initialize(ID3D11Device* device, WCHAR* textureFilename, WCHAR* displacementMapName, WCHAR* bumpMapName)
{
	bool result;


	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	// Load the textures for this model.
	result = LoadTexture(device, textureFilename, displacementMapName, bumpMapName);
	if(!result)
	{
		return false;
	}

	return true;
}

void VertexGeometryMeshClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();
	
	// Release the vertex and index buffers.
	ShutdownBuffers();

	return;
}

void VertexGeometryMeshClass::Render(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext, texture);

	return;
}

int VertexGeometryMeshClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* VertexGeometryMeshClass::GetTexture()
{
	return m_Texture->GetTexture();
}

ID3D11ShaderResourceView* VertexGeometryMeshClass::GetDisplacementMap()
{
	return m_displacementMap->GetTexture();
}

ID3D11ShaderResourceView* VertexGeometryMeshClass::GetBumpMap()
{
	return m_bumpMap->GetTexture();
}

void VertexGeometryMeshClass::GenerateMips(ID3D11DeviceContext* deviceContext)
{
	deviceContext->GenerateMips(GetTexture());
}

bool VertexGeometryMeshClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	// Set the number of vertices in the vertex array.
	m_vertexCount = 100;

	// Set the number of indices in the index array.
	m_indexCount = 100;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	int vertexNumber = 0;
	float u = 0;
	float v = 0;

	//create a 10 x 10 grid of points to generate 'mesh' of squares for billboarding
	for (float i = -0.9f; i < 1.1f; i += 0.2f)
	{
		for (float j = -0.9f; j < 1.1f; j += 0.2f)
		{
			vertices[vertexNumber].position = D3DXVECTOR3(i, j, 0.0f);
			vertices[vertexNumber].texture = D3DXVECTOR2(u, v);
			vertices[vertexNumber].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			vertexNumber++;
			u += 0.05f;
		}

		v += 0.05f;
	}

	for (int i = 0; i < vertexNumber; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

void VertexGeometryMeshClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void VertexGeometryMeshClass::RenderBuffers(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture) 
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case points.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	return;
}

bool VertexGeometryMeshClass::LoadTexture(ID3D11Device* device, WCHAR* filename, WCHAR* displacementMapName, WCHAR* bumpMapName)
{
	bool result;

	// Create the texture object.
	m_Texture = new TextureClass;
	if(!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	// Create the texture object.
	m_displacementMap = new TextureClass;
	if(!m_displacementMap)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_displacementMap->Initialize(device, displacementMapName);
	if(!result)
	{
		return false;
	}

	// Create the texture object.
	m_bumpMap = new TextureClass;
	if(!m_bumpMap)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_bumpMap->Initialize(device, bumpMapName);
	if(!result)
	{
		return false;
	}

	return true;
}

void VertexGeometryMeshClass::ReleaseTexture()
{
	// Release the texture object.
	if(m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	if(m_displacementMap)
	{
		m_displacementMap->Shutdown();
		delete m_displacementMap;
		m_displacementMap = 0;
	}

	if(m_bumpMap)
	{
		m_bumpMap->Shutdown();
		delete m_bumpMap;
		m_bumpMap = 0;
	}

	return;
}


