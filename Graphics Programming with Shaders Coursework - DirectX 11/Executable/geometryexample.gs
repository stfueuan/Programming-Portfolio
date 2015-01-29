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

cbuffer PositionBuffer
{
    static float3 g_positions[4] =
    {
        float3(-0.1, 0.1, 0), //top left
        float3(0.1, 0.1, 0),  //top right
        float3(-0.1, -0.1, 0),//bottom left
        float3(0.1, -0.1, 0)  //bottom right
    };
};

struct GeometryInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;

};

struct GeometryOutType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


[maxvertexcount(4)]
void GSGeometryShader (point GeometryInputType input[1], inout TriangleStream<GeometryOutType> triStream)
{
	GeometryOutType output;

	//TEXTURE COORDINATES
	float2 texCoord[4];
	texCoord[0] = float2(0, 1); //top left
	texCoord[1] = float2(1, 1); //top right
	texCoord[2] = float2(0, 0); //bottom left
	texCoord[3] = float2(1, 0); //bottom right

	//get the direction vector from the input points position to the camera's position
	float3 vnormal = cameraPosition - input[0].position.xyz;
	vnormal = normalize(vnormal);
	
	// Get world position of point position - this is done first so the generated quad will always update relative to your position
	float4 worldPosition = mul(input[0].position, worldMatrix);

	// Geneate a quad relative to the points world position, worldMatrix of course is based on cameras current position
	float3 posit[4];
	posit[3] = worldPosition + g_positions[0];
	posit[2] = worldPosition + g_positions[1];
	posit[1] = worldPosition + g_positions[2];
	posit[0] = worldPosition + g_positions[3];
	
	for(int i = 0; i < 4; i++)
	{
		float3 vposition = posit[i];
		output.position = mul(float4(vposition,1.0), viewMatrix);
		output.position = mul(output.position, projectionMatrix); 
		output.tex = texCoord[i];
		output.normal = vnormal;
		triStream.Append(output);
	}

	triStream.RestartStrip();
}