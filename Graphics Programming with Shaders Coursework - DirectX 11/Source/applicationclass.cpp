////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////

#include "applicationclass.h"

ApplicationClass::ApplicationClass()
{
	m_Input = 0;
	m_Direct3D = 0;
	m_Camera = 0;
	m_TextureShader = 0;
	m_Timer = 0;
	m_Position = 0;
	front_Mesh = 0;
	right_Mesh = 0;
	left_Mesh = 0;
	back_Mesh = 0;
	top_Mesh = 0;
	new_LightShader = 0;
	m_TessellationShader = 0;
	rotation = 0;
	manualRotation = 0;
	m_ManipulativeTexture = 0;
	m_GeometryShader = 0;
	m_GeometryPoint = 0;
	m_RenderTexture = 0;
	m_RenderTexture2 = 0;
	m_TargetWindow = 0;
	m_downSampleTexture = 0;
	m_upSampleTexture = 0;
	m_horizontalBlurTexture = 0;
	m_verticalBlurTexture = 0;
	m_downSampleWindow = 0;
	m_upSampleWindow = 0;
	m_horizontalBlurShader = 0;
	m_verticalBlurShader = 0;
	glow = false;
	glowType = false;
	pressed = false;
	tessellationAmount = 1;

	for (int i = 0; i < NO_OF_LIGHTS; i++)
	{
		new_Light[i] = 0;
	}
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight) 
{
	bool result;
	D3DXMATRIX baseViewMatrix;
	
	// Create the input object.  The input object will be used to handle reading the keyboard and mouse input from the user.
	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the Direct3D object.
	m_Direct3D = new D3DClass;
	if(!m_Direct3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize DirectX 11.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// Create Mesh object
	front_Mesh = new TessellationMeshClass;
	left_Mesh = new TessellationMeshClass;
	right_Mesh = new TessellationMeshClass;
	back_Mesh = new TessellationMeshClass;
	top_Mesh = new TessellationMeshClass;

	// Initialise the mesh objects - links provided will take you to the textures used from the internet
	result = front_Mesh->Initialize(m_Direct3D->GetDevice(), L"data/weave_diffuse.jpg", L"data/nodisplacementmap.png", L"data/weave_normal.jpg"); //http://away3d.com/tutorials/Introduction_to_Materials, accessed 28 November 2014
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialise the front_Mesh object.", L"Error", MB_OK);
		return false;
	}

	front_Mesh->GenerateMips(m_Direct3D->GetDeviceContext());

	result = right_Mesh->Initialize(m_Direct3D->GetDevice(), L"data/brick1.dds", L"data/nodisplacementmap.png", L"data/brick1_NRM.dds"); 
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialise the right_Mesh object.", L"Error", MB_OK);
		return false;
	}

	right_Mesh->GenerateMips(m_Direct3D->GetDeviceContext());

	result = back_Mesh->Initialize(m_Direct3D->GetDevice(), L"data/ColorMap.png", L"data/nodisplacementmap.png", L"data/Normal_Map_2.png"); //http://kay-vriend.blogspot.co.uk/2012/11/well-preserved-chesterfield.html, accessed 28 November 2014
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialise the back_Mesh object.", L"Error", MB_OK);
		return false;
	}

	back_Mesh->GenerateMips(m_Direct3D->GetDeviceContext());

	result = top_Mesh->Initialize(m_Direct3D->GetDevice(), L"data/face_height2.png", L"data/face_height2.png", L"data/face_norm.png"); //http://www.jpct.net/forum2/index.php?topic=2153.0, accessed 9 December 2014
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialise the top_Mesh object.", L"Error", MB_OK);
		return false;
	}

	top_Mesh->GenerateMips(m_Direct3D->GetDeviceContext());

	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	if(!m_TextureShader)
	{
		return false;
	}

	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the Light shader object
	new_LightShader = new LightShaderClass;
	if(!new_LightShader)
	{
		return false;
	}

	// Initialise the Light shader object
	result = new_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialise the light shader object.", L"Error", MB_OK);
		return false;
	}

	for (int i = 0; i < NO_OF_LIGHTS; i++)
	{
		// Create the light objects
		new_Light[i] = new LightClass;
		if (!new_Light[i])
		{
			MessageBox(hwnd, L"Could not initialise the light object.", L"Error", MB_OK);
			return false;
		}
	}

	// Initialise the light object - TOP RIGHT CORNER OF FRONT FACE
	new_Light[0]->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f); //Set diffuse color to red
	new_Light[0]->SetDirection(-2.0f, -2.0f, 1.0f, 1.0f); //Set the direction to point to the origin
	new_Light[0]->SetAmbientColor(0.125f, 0.125f, 0.125f, 1.0f);
	new_Light[0]->SetSpecularColor(0.0f, 0.0f, 1.0f, 1.0f);
	new_Light[0]->SetLightPosition(2.0f, 2.0f, -1.0f, 1.0f);

	// Initialise the light object - BACK TOP RIGHT CORNER OF FRONT FACE
	new_Light[1]->SetDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f); //Set diffuse color to green
	new_Light[1]->SetDirection(-2.0f, -2.0f, -2.0f, 1.0f); //Set the direction to point to the origin
	new_Light[1]->SetAmbientColor(0.125f, 0.125f, 0.125f, 1.0f);
	new_Light[1]->SetSpecularColor(0.0f, 0.0f, 1.0f, 1.0f);
	new_Light[1]->SetLightPosition(2.0f, 2.0f, 3.0f, 1.0f);

	// Initialise the light object - BACK TOP LEFT CORNER OF FRONT FACE
	new_Light[2]->SetDiffuseColor(0.0f, 0.0f, 1.0f, 1.0f); //Set diffuse color to blue
	new_Light[2]->SetDirection(2.0f, -2.0f, -2.0f, 1.0f); //Set the direction to point to the origin
	new_Light[2]->SetAmbientColor(0.125f, 0.125f, 0.125f, 1.0f);
	new_Light[2]->SetSpecularColor(0.0f, 0.0f, 1.0f, 1.0f);
	new_Light[2]->SetLightPosition(-2.0f, 2.0f, 3.0f, 1.0f);

	// Initialise the light object - TOP LEFT CORNER OF FRONT FACE
	new_Light[3]->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f); //Set diffuse color to red
	new_Light[3]->SetDirection(2.0f, -2.0f, 1.0f, 1.0f); //Set the direction to point to the origin
	new_Light[3]->SetAmbientColor(0.125f, 0.125f, 0.125f, 1.0f);
	new_Light[3]->SetSpecularColor(0.0f, 0.0f, 1.0f, 1.0f);
	new_Light[3]->SetLightPosition(-2.0f, 2.0f, -1.0f, 1.0f);

	// Initialise the light object - BOTTOM LEFT CORNER OF FRONT FACE
	new_Light[4]->SetDiffuseColor(0.0f, 0.0f, 1.0f, 1.0f); //Set diffuse color to blue
	new_Light[4]->SetDirection(2.0f, 2.0f, 1.0f, 1.0f); //Set the direction to point to the origin
	new_Light[4]->SetAmbientColor(0.125f, 0.125f, 0.125f, 1.0f);
	new_Light[4]->SetSpecularColor(1.0f, 0.0f, 0.0f, 1.0f);
	new_Light[4]->SetLightPosition(-2.0f, -2.0f, -1.0f, 1.0f);

	// Initialise the light object - BOTTOM RIGHT CORNER OF FRONT FACE
	new_Light[5]->SetDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f); //Set diffuse color to green
	new_Light[5]->SetDirection(-2.0f, 2.0f, 1.0f, 1.0f); //Set the direction to point to the origin
	new_Light[5]->SetAmbientColor(0.125f, 0.125f, 0.125f, 1.0f);
	new_Light[5]->SetSpecularColor(1.0f, 0.0f, 0.0f, 1.0f);
	new_Light[5]->SetLightPosition(2.0f, -2.0f, -1.0f, 1.0f);

	// Initialise the light object - BACK BOTTOM RIGHT CORNER OF FRONT FACE
	new_Light[6]->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f); //Set diffuse color to red
	new_Light[6]->SetDirection(-2.0f, 2.0f, -2.0f, 1.0f); //Set the direction to point to the origin
	new_Light[6]->SetAmbientColor(0.125f, 0.125f, 0.125f, 1.0f);
	new_Light[6]->SetSpecularColor(1.0f, 0.0f, 0.0f, 1.0f);
	new_Light[6]->SetLightPosition(2.0f, -2.0f, 3.0f, 1.0f);

	// Initialise the light object - BACK BOTTOM LEFT CORNER OF FRONT FACE
	new_Light[7]->SetDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f); //Set diffuse color to green
	new_Light[7]->SetDirection(2.0f, 2.0f, -2.0f, 1.0f); //Set the direction to point to the origin
	new_Light[7]->SetAmbientColor(0.125f, 0.125f, 0.125f, 1.0f);
	new_Light[7]->SetSpecularColor(1.0f, 0.0f, 0.0f, 1.0f);
	new_Light[7]->SetLightPosition(-2.0f, -2.0f, 3.0f, 1.0f);

	// Create the timer object.
	m_Timer = new TimerClass;
	if(!m_Timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
		return false;
	}

	// Create the position object.
	m_Position = new PositionClass;
	if(!m_Position)
	{
		return false;
	}

	// Set the initial position of the viewer to the same as the initial camera position.
	m_Position->SetPosition(0.0f, 0.0f, -10.0f);

	// Initialise the Tessellation Shader
	m_TessellationShader = new TessellationShaderClass;
	result = m_TessellationShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Tessellation Shader object.", L"Error", MB_OK);
		return false;
	}

	// Initialise Geometry Shader
	m_GeometryShader = new GeometryShaderClass;
	result = m_GeometryShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Geometry Shader object.", L"Error", MB_OK);
		return false;
	}

	// Initialise geometry for geometry shader
	m_GeometryPoint = new VertexGeometryMeshClass;
	result = m_GeometryPoint->Initialize(m_Direct3D->GetDevice(), L"data/cycle-treefrog0.png", L"data/nodisplacementmap.png", L"data/nobumpmap.png");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialise the point object.", L"Error", MB_OK);
		return false;
	}

	m_GeometryPoint->GenerateMips(m_Direct3D->GetDeviceContext());

	// Create new render to texture objects
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture)
	{
		return false;
	}

	m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);

	m_RenderTexture2 = new RenderTextureClass;
	if (!m_RenderTexture2)
	{
		return false;
	}

	m_RenderTexture2->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);

	//Create new target window
	m_TargetWindow = new TargetWindowClass;
	if (!m_TargetWindow)
	{
		return false;
	}

	// Initialise new target window
	m_TargetWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight);

	int downSampleWidth, downSampleHeight;

	downSampleWidth = screenWidth / 6;
	downSampleHeight = screenHeight / 6;

	// Create and intialise new sample textures
	m_downSampleTexture = new RenderTextureClass;
	m_downSampleTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);

	m_horizontalBlurTexture = new RenderTextureClass;
	m_horizontalBlurTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);

	m_verticalBlurTexture = new RenderTextureClass;
	m_verticalBlurTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);

	m_upSampleTexture = new RenderTextureClass;
	m_upSampleTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);

	m_downSampleWindow = new TargetWindowClass;
	m_downSampleWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, downSampleWidth, downSampleHeight);

	m_upSampleWindow = new TargetWindowClass;
	m_upSampleWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight);

	// Create and intialise new horizontal blur shader
	m_horizontalBlurShader = new HorizontalBlurShaderClass;
	m_horizontalBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd);

	// Create and intialise new vertical blur shader
	m_verticalBlurShader = new VerticalBlurShaderClass;
	m_verticalBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	
	// Create and initialise new glow shader 
	m_glowShader = new GlowShaderClass;
	m_glowShader->Initialize(m_Direct3D->GetDevice(), hwnd);

	return true;
}

void ApplicationClass::Shutdown()
{
	
	// Release the position object.
	if(m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	// Release the timer object.
	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the color shader object.
	if(m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the mesh objects
	if(front_Mesh)
	{
		front_Mesh->Shutdown();
		delete front_Mesh;
		front_Mesh = 0;
	}

	if(right_Mesh)
	{
		right_Mesh->Shutdown();
		delete right_Mesh;
		right_Mesh = 0;
	}

	if (left_Mesh)
	{
		left_Mesh->Shutdown();
		delete left_Mesh;
		left_Mesh = 0;
	}

	if (back_Mesh)
	{
		back_Mesh->Shutdown();
		delete back_Mesh;
		back_Mesh = 0;
	}

	if (top_Mesh)
	{
		top_Mesh->Shutdown();
		delete top_Mesh;
		top_Mesh = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if(m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	// Release the input object.
	if(m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	// Release the light object
	for (int i = 0; i < NO_OF_LIGHTS; i++)
	{
		if (new_Light[i])
		{
			delete new_Light[i];
			new_Light[i] = 0;
		}
	}

	// Release the light shader object
	if (new_LightShader)
	{
		new_LightShader->Shutdown();
		delete new_LightShader;
		new_LightShader = 0;
	}
	
	// Release the Tessellation shader object
	if (m_TessellationShader)
	{
		m_TessellationShader->Shutdown();
		delete m_TessellationShader;
		m_TessellationShader = 0;
	}

	if (m_GeometryShader)
	{
		m_GeometryShader->Shutdown();
		delete m_GeometryShader;
		m_GeometryShader = 0;
	}

	if (m_GeometryPoint)
	{
		m_GeometryPoint->Shutdown();
		delete m_GeometryPoint; 
		m_GeometryPoint = 0;
	}

	if (m_TargetWindow)
	{
		m_TargetWindow->Shutdown();
		delete m_TargetWindow;
		m_TargetWindow = 0;
	}

	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	if (m_RenderTexture2)
	{
		m_RenderTexture2->Shutdown();
		delete m_RenderTexture2;
		m_RenderTexture2 = 0;
	}

	if (m_downSampleTexture)
	{
		m_downSampleTexture->Shutdown();
		delete m_downSampleTexture;
		m_downSampleTexture = 0;
	}

	if (m_verticalBlurTexture)
	{
		m_verticalBlurTexture->Shutdown();
		delete m_verticalBlurTexture;
		m_verticalBlurTexture = 0;
	}

	if (m_horizontalBlurTexture)
	{
		m_horizontalBlurTexture->Shutdown();
		delete m_horizontalBlurTexture;
		m_horizontalBlurTexture = 0;
	}

	if (m_upSampleTexture)
	{
		m_upSampleTexture->Shutdown();
		delete m_upSampleTexture;
		m_upSampleTexture = 0;
	}

	if (m_upSampleWindow)
	{
		m_upSampleWindow->Shutdown();
		delete m_upSampleWindow;
		m_upSampleWindow = 0;
	}

	if (m_horizontalBlurShader)
	{
		m_horizontalBlurShader->Shutdown();
		delete m_horizontalBlurShader;
		m_horizontalBlurShader = 0;
	}

	if (m_verticalBlurShader)
	{
		m_verticalBlurShader->Shutdown();
		delete m_verticalBlurShader;
		m_verticalBlurShader = 0;
	}

	if (m_downSampleWindow)
	{
		m_downSampleWindow->Shutdown();
		delete m_downSampleWindow;
		m_downSampleWindow = 0;
	}

	return;
}

bool ApplicationClass::Frame()
{
	bool result;

	// Read the user input.
	result = m_Input->Frame();
	if(!result)
	{
		return false;
	}
	
	// Check if the user pressed escape and wants to exit the application.
	if(m_Input->IsEscapePressed() == true)
	{
		return false;
	}

	// Update the system stats.
	m_Timer->Frame();
	
	// Do the frame input processing.
	result = HandleInput(m_Timer->GetTime());
	if(!result)
	{
		return false;
	}

	// Render the graphics.
	result = RenderGraphics();
	if(!result)
	{
		return false;
	}

	return result;
}

bool ApplicationClass::HandleInput(float frameTime)
{
	bool keyDown, mouseDown;
	float posX, posY, posZ, rotX, rotY, rotZ;
	string clockWise = "ClockWise";
	string antiClockWise = "Anti-ClockWise";

	// Set the frame time for calculating the updated position.
	m_Position->SetFrameTime(frameTime);

	// Handle the input.
	keyDown = m_Input->IsLeftPressed();
	m_Position->TurnLeft(keyDown);

	keyDown = m_Input->IsRightPressed();
	m_Position->TurnRight(keyDown);

	keyDown = m_Input->IsUpPressed();
	m_Position->MoveForward(keyDown);

	keyDown = m_Input->IsDownPressed();
	m_Position->MoveBackward(keyDown);

	keyDown = m_Input->IsAPressed();
	m_Position->MoveUpward(keyDown);

	keyDown = m_Input->IsZPressed();
	m_Position->MoveDownward(keyDown);

	keyDown = m_Input->IsPgUpPressed();
	m_Position->LookUpward(keyDown);

	keyDown = m_Input->IsPgDownPressed();
	m_Position->LookDownward(keyDown);

	keyDown = m_Input->IsEPressed();
	ManualRotation(keyDown, clockWise);

	keyDown = m_Input->IsQPressed();
	ManualRotation(keyDown, antiClockWise);

	keyDown = m_Input->IsSpacePressed();
	WireFrame(keyDown);

	//Ensure that the button press only registers once - now acts as a toggle
	if (!pressed)
	{
		keyDown = m_Input->IsEnterPressed();
		if (keyDown)
		{
			glow = !glow;
		}

		keyDown = m_Input->IsShiftPressed();
		if (keyDown)
		{
			glowType = !glowType;
		}

		pressed = true;
	}

	//if neither of them are being pressed, flip pressed bool
	if (!m_Input->IsEnterPressed() && !m_Input->IsShiftPressed())
	{
		pressed = false;
	}

	keyDown = m_Input->IsNumPad2Pressed();
	if (keyDown)
	{
		tessellationAmount--;

		if (tessellationAmount < 1)
		{
			tessellationAmount = 1;
		}
	}

	keyDown = m_Input->IsNumPad8Pressed();
	if (keyDown)
	{
		tessellationAmount++;

		if (tessellationAmount > 64)
		{
			tessellationAmount = 64;
		}
	}

	mouseDown = m_Input->IsLeftMouseButtonPressed();
	ManualRotation(mouseDown, clockWise);

	mouseDown = m_Input->IsRightMouseButtonPressed();
	ManualRotation(mouseDown, antiClockWise);
	
	// Get the view point position/rotation.
	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	// Set the position of the camera.
	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	return true;
}

bool ApplicationClass::RenderGraphics()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, baseViewMatrix;
	bool result;
	
	if (!glow)
	{
		//Render full colour scene with no glow effect
		result = RenderNormal();
		if (!result)
		{
			return false;
		}
	}
	else
	{
		//Render full colour scene with glow effect
		result = RenderGlow();
		if (!result)
		{
			return false;
		}
	}

	return true;
}

bool ApplicationClass::RenderNormal()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, baseViewMatrix;
	bool result;

	result = RenderToTexture(m_RenderTexture);
	if (!result)
	{
		return false;
	}
	// Clear the scene.
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	// Turn Z buffer off for target window
	m_Direct3D->TurnZBufferOff();

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	m_TargetWindow->Render(m_Direct3D->GetDeviceContext());
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_TargetWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView(), 0.0f);
	if (!result)
	{
		return false;
	}

	m_Direct3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}

bool ApplicationClass::RenderGlow()
{
	bool result;

	// Render greyscale scene to texture
	result = RenderToTexture(m_RenderTexture);
	if (!result)
	{
		return false;
	}

	// Down sample the new texture
	result = DownSampleTexture();
	if (!result)
	{
		return false;
	}

	// Perform horizontal blur on down sampled texture
	result = RenderHorizontalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// Now perform a vertical blur on the horizontal blur render texture.
	result = RenderVerticalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// Up sample the final blurred render texture to screen size again.
	result = UpSampleTexture();
	if (!result)
	{
		return false;
	}

	// Render the blurred up sampled render texture to the screen.
	result = Render2DTextureScene();
	if (!result)
	{
		return false;
	}

	// Render full colour scene to texture
	result = RenderToTexture(m_RenderTexture2);
	if (!result)
	{
		return false;
	}

	// Glow!
	result = BlendTexture();
	if (!result)
	{
		return false;
	}

	return true;
}

float ApplicationClass::Rotate()
{
	//Rotate slowly clockwise
	rotation += (float)D3DX_PI * 0.001f;

	if(rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	return rotation;
}

float ApplicationClass::ManualRotation(bool keyDown, string direction)
{
	// Rotate the cube using keypresses
	if (keyDown && direction.compare("ClockWise"))
	{
		manualRotation += (float)D3DX_PI * 0.002f;
	}
	else if (keyDown && direction.compare("Anti-ClockWise"))
	{
		manualRotation -= (float)D3DX_PI * 0.002f;
	}

	// Have the cube rotate properly in either direction, even if it rotates more than 360 degrees or less than 0
	if (manualRotation > 2 * (float)D3DX_PI)
	{
		manualRotation -= 2 * (float)D3DX_PI;
	}

	if (manualRotation < 0.0f)
	{
		manualRotation += 2 * (float)D3DX_PI;
	}

	return manualRotation;
}

bool ApplicationClass::RenderObjects(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	D3DXMATRIX TranslationMatrix, RotationMatrix;
	bool result;

	// Push mesh data onto gfx hardware
	worldMatrix = RotateObject(worldMatrix, TranslationMatrix);

	// Render front mesh
	result = RenderTessellation(front_Mesh, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// Translate and rotate right mesh
	D3DXMatrixTranslation(&TranslationMatrix, 1.0f, 0.0f, 1.0f);
	D3DXMatrixRotationY(&RotationMatrix, -(float)D3DX_PI / 2.0f);

	D3DXMatrixMultiply(&worldMatrix, &RotationMatrix, &TranslationMatrix);

	worldMatrix = RotateObject(worldMatrix, TranslationMatrix);

	// Render right mesh
	result = RenderTessellation(right_Mesh, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// Translate and rotate geometry shaded side
	D3DXMatrixTranslation(&TranslationMatrix, -1.0f, 0.0f, 1.0f);
	D3DXMatrixRotationY(&RotationMatrix, (float)D3DX_PI / 2.0f);

	D3DXMatrixMultiply(&worldMatrix, &RotationMatrix, &TranslationMatrix);

	worldMatrix = RotateObject(worldMatrix, TranslationMatrix);

	//Render the Geometry shader side
	m_GeometryPoint->Render(m_Direct3D->GetDeviceContext(), m_GeometryPoint->GetTexture());
	result = m_GeometryShader->Render(m_Direct3D->GetDeviceContext(), m_GeometryPoint->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_GeometryPoint->GetTexture(), m_Camera->GetPosition());
	if (!result)
	{
		return false;
	}

	// Translate and rotate back side
	D3DXMatrixTranslation(&TranslationMatrix, 0.0f, 0.0f, 2.0f);
	D3DXMatrixRotationY(&RotationMatrix, (float)D3DX_PI);

	D3DXMatrixMultiply(&worldMatrix, &RotationMatrix, &TranslationMatrix);

	worldMatrix = RotateObject(worldMatrix, TranslationMatrix);

	// Render back side
	result = RenderTessellation(back_Mesh, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// Translate and rotate top mesh - all rotation done here since it requires different axis of rotation to other meshes
	D3DXMatrixTranslation(&TranslationMatrix, 0.0f, 1.0f, 1.0f);
	D3DXMatrixRotationX(&RotationMatrix, (float)D3DX_PI / 2.0f);

	D3DXMatrixMultiply(&worldMatrix, &RotationMatrix, &TranslationMatrix);

	D3DXMatrixTranslation(&TranslationMatrix, 0.0f, 0.0f, 1.0f);
	D3DXMatrixMultiply(&worldMatrix, &TranslationMatrix, &worldMatrix);

	D3DXMATRIX matRot;
	D3DXMATRIX matTrans;
	D3DXMatrixRotationZ(&matRot, -manualRotation);

	D3DXMatrixMultiply(&worldMatrix, &matRot, &worldMatrix);

	D3DXMatrixTranslation(&TranslationMatrix, 0.0f, 0.0f, -1.0f);
	D3DXMatrixMultiply(&worldMatrix, &TranslationMatrix, &worldMatrix);

	// Render top mesh
	result = RenderTessellation(top_Mesh, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}

D3DXMATRIX ApplicationClass::RotateObject(D3DXMATRIX worldMatrix, D3DXMATRIX translationMatrix)
{
	// Rotate objects at radius
	D3DXMatrixTranslation(&translationMatrix, 0.0f, 0.0f, 1.0f);
	D3DXMatrixMultiply(&worldMatrix, &translationMatrix, &worldMatrix);

	D3DXMATRIX RotationMatrix;
	D3DXMatrixRotationY(&RotationMatrix, manualRotation);

	D3DXMatrixMultiply(&worldMatrix, &RotationMatrix, &worldMatrix);

	// Translate mesh back to initial position
	D3DXMatrixTranslation(&translationMatrix, 0.0f, 0.0f, -1.0f);
	D3DXMatrixMultiply(&worldMatrix, &translationMatrix, &worldMatrix);

	return worldMatrix;
}

bool ApplicationClass::RenderTessellation(TessellationMeshClass* tessellationMesh, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	bool result;

	tessellationMesh->Render(m_Direct3D->GetDeviceContext());

	result = m_TessellationShader->Render(m_Direct3D->GetDeviceContext(), tessellationMesh->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, tessellationAmount, tessellationMesh->GetTexture(), tessellationMesh->GetDisplacementMap(), tessellationMesh->GetBumpMap(), new_Light, m_Camera->GetPosition());
	if (!result)
	{
		return false;
	}

	return true;
}

void ApplicationClass::WireFrame(bool wireFrame)
{
	D3D11_RASTERIZER_DESC rasterDesc;
	ID3D11RasterizerState* rasterState;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	// Setup the raster description which will determine how and what polygons will be drawn.
	if (!wireFrame)
	{
		// No wireframe
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_NONE;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;
	}
	else
	{
		// Wireframe
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_NONE;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;
	}

	// Create the rasterizer state from the description we just filled out.
	device = m_Direct3D->GetDevice(); 
	device->CreateRasterizerState(&rasterDesc, &rasterState);

	// Now set the rasterizer state.
	deviceContext = m_Direct3D->GetDeviceContext();
	deviceContext->RSSetState(rasterState);
}

bool ApplicationClass::RenderToTexture(RenderTextureClass* renderTexture)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Set and clear render target
	renderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	renderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world and view and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Render all objects in scene
	result = RenderObjects(worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// Force wireframe to be off for target windows
	WireFrame(false);

	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}

bool ApplicationClass::DownSampleTexture()
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;

	bool result;

	// Set the render target to be the render to texture.
	m_downSampleTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_downSampleTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 1.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world and view matrices from the camera and d3d objects.
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	m_downSampleTexture->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Put the small ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_downSampleWindow->Render(m_Direct3D->GetDeviceContext());

	// Render the small ortho window using the texture shader and the render to texture of the scene asthe texture resource.
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_downSampleWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView(), 1);
	if (!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}

bool ApplicationClass::RenderHorizontalBlurToTexture()
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float screenSizeX;
	bool result;

	// Store the screen width in a float that will be used in the horizontal blur shader.
	screenSizeX = (float)m_horizontalBlurTexture->GetTextureWidth();

	// Set the render target to be the render to texture.
	m_horizontalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_horizontalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world and view matrices from the camera and d3d objects.
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Get the ortho matrix from the render to texture since texture has different dimensions.
	m_horizontalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Put the down sample window vertex buffer on the graphics pipeline to prepare them for drawing.
	m_downSampleWindow->Render(m_Direct3D->GetDeviceContext());

	// Render the down sample window using the horizontal blur shader and the down sampled render to texture resource.
	result = m_horizontalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_downSampleWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_downSampleTexture->GetShaderResourceView(), screenSizeX);
	if (!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}

bool ApplicationClass::RenderVerticalBlurToTexture()
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float screenSizeY;
	bool result;

	// Store the screen height in a float that will be used in the vertical blur shader.
	screenSizeY = (float)m_verticalBlurTexture->GetTextureHeight();

	// Set the render target to be the render to texture.
	m_verticalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_verticalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world and view matrices from the camera and d3d objects.
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Get the ortho matrix from the render to texture since texture has different dimensions.
	m_verticalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Put the down sample window vertex bufferon the graphics pipeline to prepare them for drawing.
	m_downSampleWindow->Render(m_Direct3D->GetDeviceContext());

	// Render the down sample window using the vertical blur shader and the horizontal blurred render to texture resource.
	result = m_verticalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_downSampleWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_horizontalBlurTexture->GetShaderResourceView(), screenSizeY);
	if (!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}

bool ApplicationClass::UpSampleTexture()
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	// Set the render target to be the render to texture.
	m_upSampleTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_upSampleTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world and view matrices from the camera and d3d objects.
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Get the ortho matrix from the render to texture since texture has different dimensions.
	m_upSampleTexture->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Put the up sample window vertex buffer on the graphics pipeline to prepare them for drawing.
	m_upSampleWindow->Render(m_Direct3D->GetDeviceContext());

	// Render the up sample window using the texture shader and the small sized final blurred render totexture resource.
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_upSampleWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_verticalBlurTexture->GetShaderResourceView(), 0);
	if (!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}

bool ApplicationClass::Render2DTextureScene()
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and ortho matrices from the camera and d3d objects.
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// Put the up sample window vertex buffer on the graphics pipeline to prepare them for drawing.
	m_upSampleWindow->Render(m_Direct3D->GetDeviceContext());

	// Render the up sample window using the texture shader and the full screen sized blurred render to texture resource.
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_upSampleWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_upSampleTexture->GetShaderResourceView(), 0);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ApplicationClass::BlendTexture()
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(1.0f, 0.0f, 0.0f, 0.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Put the up sample window vertex buffer on the graphics pipeline to prepare them for drawing.
	m_upSampleWindow->Render(m_Direct3D->GetDeviceContext());

	if (!glowType)
	{
		//Dim, night-time style
		result = m_glowShader->Render(m_Direct3D->GetDeviceContext(), m_upSampleWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture2->GetShaderResourceView(), m_upSampleTexture->GetShaderResourceView(), 0);
		if (!result)
		{
			return false;
		}
	}
	else
	{
		//Rediculous, bling-style
		result = m_glowShader->Render(m_Direct3D->GetDeviceContext(), m_upSampleWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture2->GetShaderResourceView(), m_upSampleTexture->GetShaderResourceView(), 1);
		if (!result)
		{
			return false;
		}
	}
	

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}