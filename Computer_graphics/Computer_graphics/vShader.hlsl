#include "sceneBuffer.hlsli"

struct GeomBuffer
{
	float4x4 model;
	float4x4 norm;
	float4 shineSpeedTexIdNM; // x - shininess, y - rotation speed, z - texture id, w - normal map presence
	float4 angle; // xyz � position, w - current angle
};
cbuffer GeomBufferInst : register (b1)
{
	GeomBuffer geomBuffer[100];
};

struct VSInput
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	uint instanceId : SV_InstanceID;
};

struct VSOutput
{
	float4 position : SV_Position;
	float4 worldPos : POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	nointerpolation uint instanceId : INST_ID;
};

VSOutput main(VSInput input)
{
	VSOutput output;
	unsigned int idx = indexBuffer[input.instanceId].x;
	output.worldPos = mul(world, float4(input.position, 1.0f));
	output.position = mul(viewProj, output.worldPos);
	output.texCoord = input.texCoord;
	output.normal = mul(world, float4(input.normal, 1.0f)).xyz;
	output.tangent = mul(world, float4(input.tangent, 1.0f)).xyz;
	output.instanceId = idx;


	return output;
}