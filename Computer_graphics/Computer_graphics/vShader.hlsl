cbuffer WorldMatrixBuffer : register (b0) {
	float4x4 worldMatrix;
};

cbuffer SceneMatrixBuffer : register (b1) {
	float4x4 viewProjMatrix;
}

struct VSInput {
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VSOutput {
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VSOutput main(VSInput vertex) {
	VSOutput result;
	result.position = mul(viewProjMatrix, mul(worldMatrix, float4(vertex.position, 1.0f)));
	result.color = vertex.color;

	return result;
}