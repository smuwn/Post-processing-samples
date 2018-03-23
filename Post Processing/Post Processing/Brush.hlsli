
Texture2D ObjTexture : register(t0);
Texture2D ObjPattern : register(t1);
RWTexture2D<float4> ObjResult : register(u0);
SamplerState ObjWrapSampler : register(s0);

cbuffer cbWindowInfo : register(b0)
{
	uint2 gWindowSize;
};

cbuffer cbMouseInfo : register(b1)
{
	uint2 gCursorPosition;
	float thickness;
	float radius;
};

float4 GetPatternColor(float2 currentPixel, float d)
{
	float2 uv;
	uv.x = (currentPixel.x - gCursorPosition.x) / radius;
	uv.y = (currentPixel.y - gCursorPosition.y) / radius;

	uv = uv / 2.0f + 0.5f;

	float4 color = ObjPattern.SampleLevel(ObjWrapSampler, uv, 0);
	return color;
}