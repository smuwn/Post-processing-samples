Texture2D ObjInputTexture : register(t0);
SamplerState ObjWrapSampler : register(s0);
RWTexture2D<float4> ObjOutputTexture : register(u0);

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float width, height;
	ObjInputTexture.GetDimensions(width, height);
	width -= 1;
	height -= 1;
	float2 uv = float2(float(DTid.x) / width, float(DTid.y) / height);
	ObjOutputTexture[DTid.xy] = ObjInputTexture.SampleLevel(ObjWrapSampler, uv, 0);
}