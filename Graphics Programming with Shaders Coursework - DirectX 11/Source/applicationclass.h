////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "textureshaderclass.h"
#include "timerclass.h"
#include "positionclass.h"
#include "meshclass.h"
#include "lightclass.h"
#include "lightshaderclass.h"
#include "tessellationshaderclass.h"
#include "tessellationMesh.h"
#include "geometryshaderclass.h"
#include "VertexGeometryMeshClass.h"
#include "targetwindowclass.h"
#include "rendertextureclass.h"
#include "verticalblurshaderclass.h"
#include "horizontalblurshaderclass.h"
#include "glowshaderclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
#define NO_OF_LIGHTS 8

public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

private:
	bool HandleInput(float);
	bool RenderGraphics();
	bool RenderObjects(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);
	D3DXMATRIX RotateObject(D3DXMATRIX, D3DXMATRIX);
	bool RenderTessellation(TessellationMeshClass*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);
	float Rotate();
	float ManualRotation(bool, string);
	void WireFrame(bool);
	bool RenderToTexture(RenderTextureClass*);
	bool DownSampleTexture();
	bool RenderSceneToTexture();
	bool RenderHorizontalBlurToTexture();
	bool RenderVerticalBlurToTexture();
	bool UpSampleTexture();
	bool Render2DTextureScene();
	bool BlendTexture();
	bool RenderNormal();
	bool RenderGlow();

private:
	InputClass* m_Input;
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	TextureShaderClass* m_TextureShader;
	TimerClass* m_Timer;
	PositionClass* m_Position;
	LightClass* new_Light[NO_OF_LIGHTS];
	LightShaderClass* new_LightShader;
	TessellationShaderClass* m_TessellationShader;
	TessellationMeshClass* front_Mesh;
	TessellationMeshClass* right_Mesh;
	TessellationMeshClass* left_Mesh;
	TessellationMeshClass* back_Mesh;
	TessellationMeshClass* top_Mesh;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ID3D11Resource* m_ManipulativeTexture;
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_SUBRESOURCE_DATA textureSubResource;
	GeometryShaderClass* m_GeometryShader;
	VertexGeometryMeshClass* m_GeometryPoint;
	TargetWindowClass* m_TargetWindow;
	RenderTextureClass* m_RenderTexture;
	RenderTextureClass* m_RenderTexture2;
	RenderTextureClass* m_downSampleTexture;
	RenderTextureClass* m_horizontalBlurTexture;
	RenderTextureClass* m_verticalBlurTexture;
	RenderTextureClass* m_upSampleTexture;
	TargetWindowClass* m_downSampleWindow;
	TargetWindowClass* m_upSampleWindow;
	HorizontalBlurShaderClass* m_horizontalBlurShader;
	VerticalBlurShaderClass* m_verticalBlurShader;
	GlowShaderClass* m_glowShader;

	//'global' variables for use in several functions
	float rotation;
	float manualRotation;
	D3DXVECTOR4 colour;
	bool glow;
	bool glowType;
	bool pressed;
	float tessellationAmount;
};

#endif