
#include "Brush.hlsli"

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	float width, height;
	ObjTexture.GetDimensions(width, height);
	//width -= 1; // X e [0,width)
	//height -= 1; // Y e [0,heigh)
	float2 uv = float2(DTid.xy) / float2(width, height);

	float d;
	float2 currentPixel = float2(uv.x * gWindowSize.x, uv.y * gWindowSize.y);
	float2 cursorPosition = float2(gCursorPosition.x, gCursorPosition.y);
	d = distance(currentPixel, cursorPosition);

	//float4 color = ObjTexture.SampleLevel(ObjWrapSampler, uv, 0);
	float4 color = ObjTexture.Load(DTid.xyz);
	if (d <= radius)
	{
		float4 patternColor = GetPatternColor(currentPixel, d);
		color = saturate(color + patternColor * patternColor.a);	
	}

	ObjResult[DTid.xy] = color;
}