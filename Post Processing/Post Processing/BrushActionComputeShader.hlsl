
#include "Brush.hlsli"

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	float width, height;
	ObjTexture.GetDimensions(width, height);
	float2 uv = float2(float(DTid.x) / width, float(DTid.y) / height);

	float d;
	float2 currentPixel = float2(uv.x * gWindowSize.x, uv.y * gWindowSize.y);
	float2 cursorPosition = float2(gCursorPosition.x, gCursorPosition.y);
	d = distance(currentPixel, cursorPosition);

	float4 color = ObjTexture.SampleLevel(ObjWrapSampler, uv, 0);;
	if (d <= radius)
	{
		float4 patternColor = GetPatternColor(currentPixel, d);
		color = saturate(color + patternColor * patternColor.a);
		
	}

	ObjResult[DTid.xy] = color;

}