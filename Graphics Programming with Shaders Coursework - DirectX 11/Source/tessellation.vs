struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct HullInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

HullInputType ColorVertexShader(VertexInputType input)
{
    HullInputType output;

	// Pass the vertex position into the hull shader.
	output.position.w = 1.0; 
    output.position = input.position;
    
    // Pass the input color into the hull shader.
    output.tex = input.tex;

	output.normal = input.normal;
    
    return output;
}
