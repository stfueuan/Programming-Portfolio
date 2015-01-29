#define NO_OF_LIGHTS 8

cbuffer MatrixBuffer : register(cb0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer : register(cb1)
{
	float3 cameraPosition;
	float padding;
};

cbuffer LightPosition : register(cb2)
{
	float4 lightPosition[NO_OF_LIGHTS];
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct HullInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 viewDirection : TEXCOORD1; 
	float3 lightDir[NO_OF_LIGHTS] : TEXCOORD2;
};

HullInputType LightVertexShader(VertexInputType input)
{
    HullInputType output;
	float3 worldPosition;

	input.position.w = 1.0f; // Change the position vector to be 4 units for proper matrix calculations.

	// Calculate the position of the pixel in the world
	worldPosition = mul(input.position, worldMatrix).xyz;
	output.position = input.position;

	// Calculate light direction based on position in 3D space and light position
	for (int i = 0; i < NO_OF_LIGHTS; i++)
	{
		output.lightDir[i] = worldPosition - lightPosition[i];
	}

	// Determine the view direction based on the position of the camera and the position of the vertex in the world
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

	//Normalise the viewing direction vector
	output.viewDirection = normalize(output.viewDirection);
    
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// This is not multiplied against the worldmatrix as this would mess up the recalculation of normals in the tessellation domain shader
	output.normal = input.normal;

	output.tangent = mul(input.tangent, worldMatrix).xyz;

	output.binormal = mul(input.binormal, worldMatrix).xyz;

	return output;
}