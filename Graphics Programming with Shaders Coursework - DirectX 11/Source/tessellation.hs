#define NO_OF_LIGHTS 8

cbuffer TessellationBuffer : register(cb0)
{
    float tessellationFactor;
    float3 padding;
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

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct HullOutputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 viewDirection : TEXCOORD1; 
	float3 lightDir[NO_OF_LIGHTS] : TEXCOORD2;
};

ConstantOutputType ColorPatchConstantFunction(InputPatch<HullInputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{    
    ConstantOutputType output;

    // Set the tessellation factors for the three edges of the triangle.
    output.edges[0] = tessellationFactor;
    output.edges[1] = tessellationFactor;
    output.edges[2] = tessellationFactor;
	output.edges[3] = tessellationFactor;

    // Set the tessellation factor for tessallating inside the triangle.
    output.inside[0] = tessellationFactor;
	output.inside[1] = tessellationFactor;

    return output;
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ColorPatchConstantFunction")]

HullOutputType ColorHullShader(InputPatch<HullInputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    HullOutputType output;

    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;

    // Set the input color as the output color.
    output.tex = patch[pointId].tex;

	output.normal = patch[pointId].normal;

	output.tangent = patch[pointId].tangent;

	output.binormal = patch[pointId].binormal;

	output.viewDirection = patch[pointId].viewDirection;

	for(int i = 0; i < NO_OF_LIGHTS; i++)
	{
		output.lightDir[i] = patch[pointId].lightDir[i];
	}

    return output;
}