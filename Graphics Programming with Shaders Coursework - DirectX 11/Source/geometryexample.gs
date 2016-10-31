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

	//https://www.youtube.com/watch?v=R78pWR-WCMo
	// Compute the local coordinate system of the sprite relative to the world
	// space such that the billboard is aligned with the y-axis and faces the eye.
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 look = cameraPosition - input[0].position;
	look.y = 0.0f; // reinstate this if you don't want spheres to track camera in y plane
	look = normalize(look);
	float3 left = cross(up, look);

	// Get world position of point position - this is done first so the generated quad will always update relative to your position
	float4 worldPosition = mul(input[0].position, worldMatrix);

	float4 bottomLeft = float4(worldPosition + g_positions[3].x * -left + g_positions[3].y * up, 1.0f);
	float4 topLeft = float4(worldPosition + g_positions[2].x * -left + g_positions[2].y * up, 1.0f);
	float4 bottomRight = float4(worldPosition + g_positions[1].x * -left + g_positions[1].y * up, 1.0f);
	float4 topRight = float4(worldPosition + g_positions[0].x * -left + g_positions[0].y * up, 1.0f);

	float4 v[4] = { bottomLeft, topLeft, bottomRight, topRight };

	for (int i = 0; i < 4; i++)
	{
		float3 vposition = v[i];
		output.position = mul(float4(vposition, 1.0), viewMatrix);
		output.position = mul(output.position, projectionMatrix);
		output.tex = texCoord[i];
		output.normal = look;
		triStream.Append(output);
	}

	triStream.RestartStrip();
}