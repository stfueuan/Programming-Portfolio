struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

VertexInputType GSVertexShader(VertexInputType input)
{
	return input;
}