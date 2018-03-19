
Texture2D ObjTexture : register(t0);
RWTexture2D<float4> ObjResult : register(u0);
SamplerState ObjWrapSampler : register(s0);

#include "../Post Processing/ColorConversions.hlsli"

cbuffer cbPaletteInfo : register(b0)
{
	float4 gPaletteColors[8];
	uint gPaletteNumColors;
	float3 gPalettePad0;
}


float hueDistance(float h1, float h2)
{
	float diff = abs(h1 - h2);
	float inverseDiff = abs(1.0f - diff);

	return min(diff, inverseDiff);
}

float getIndexValue(uint x, uint y)
{
	static const uint size = 4;
	static const uint indexMatrix[] =
	{
		0,  8,  2,  10,
		12, 4,  14, 6,
		3,  11, 1,  9,
		15, 7,  13, 5
	};

	int xIndex = x % size;
	int yIndex = y % size;

	return indexMatrix[xIndex + yIndex * size] / 16.0f;
}

void getClosestColors(float hue, out float3 bestColor, out float3 secondBestColor)
{
	float3 closestColor = float3(-2, 0, 0);
	float3 secondClosestColor = float3(-2, 0, 0);
	float3 temp;
	float tempDistance;

	for (uint i = 0; i < gPaletteNumColors; ++i)
	{
		temp = gPaletteColors[i].rgb;
		temp = RGBtoHSL(temp);

		tempDistance = hueDistance(temp.r, hue);
		if (tempDistance < hueDistance(closestColor.r, hue))
		{
			secondClosestColor = closestColor;
			closestColor = temp;
		}
		else
		{
			if (tempDistance < hueDistance(secondClosestColor.r, hue))
			{
				secondClosestColor = closestColor;
			}
		}
	}

	bestColor = closestColor;
	secondBestColor = secondClosestColor;
}

float lightnessStep(float l)
{
	const int lightnessSteps = 4;
	return floor(0.5 + l * lightnessSteps) / lightnessSteps;
}

[numthreads(16, 16, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint width, height;
	ObjTexture.GetDimensions(width, height);
	float2 uv = float2(float(DTid.x) / width, float(DTid.y) / height);
	float2 xy = float2(width, height);

	float4 color = ObjTexture.SampleLevel(ObjWrapSampler, uv, 0);
	
	//color = float4(rgb2hsl(color.xyz), 1.0f);

	float3 hsvColor = RGBtoHSL(color.rgb);
	float3 bestColor, secondBestColor;

	getClosestColors(hsvColor.r, bestColor, secondBestColor);

	float hueDiff = hueDistance(hsvColor.r, bestColor.r) /
		hueDistance(secondBestColor.r, bestColor.r);

	float d = getIndexValue(DTid.x, DTid.y);

	float4 finalColor;
	[flatten]
	if (hueDiff < d)
	{
		float l1 = max(lightnessStep(bestColor.z) - 0.125f, 0.0f);
		float l2 = min(lightnessStep(bestColor.z) + 0.125f, 1.0f);
		float lightnessDiff = hueDistance(hsvColor.z, l1) / hueDistance(l2, l1);

		[flatten]
		if (lightnessDiff < d)
		{
			bestColor.z = l1;
		}
		else
			bestColor.z = l2;
		finalColor = float4(HSLtoRGB(bestColor), 1.0f);
	}
	else
	{
		float l1 = max(lightnessStep(bestColor.z) - 0.125f, 0.0f);
		float l2 = min(lightnessStep(bestColor.z) + 0.125f, 1.0f);
		float lightnessDiff = hueDistance(hsvColor.z, l1) / hueDistance(l2, l1);

		[flatten]
		if (lightnessDiff < d)
		{
			bestColor.z = l1;
		}
		else
			bestColor.z = l2;
		finalColor = float4(HSLtoRGB(secondBestColor), 1.0f);
	}
	ObjResult[DTid.xy] = finalColor;
}