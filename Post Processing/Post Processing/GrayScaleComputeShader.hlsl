
Texture2D ObjTexture : register( t0 );

RWTexture2D<float4> ObjResult : register( u0 );

SamplerState ObjWrapSampler : register( s0 );

[numthreads(32, 32, 1)] // 1024 total threads
void main( uint3 DTid : SV_DispatchThreadID )
{
	float width, height;
	ObjTexture.GetDimensions( width, height );
	float2 uv = float2( float( DTid.x ) / width, float( DTid.y ) / height );
	float4 color = ObjTexture.SampleLevel( ObjWrapSampler, uv, 0 );
	float finalColor = ( color.r + color.g + color.b ) / 3.0f;
	ObjResult[ DTid.xy ] = float4( finalColor, finalColor, finalColor, 1.0f );
}