#include "commonincludes.h"

namespace DX
{
	MicrosoftPointer( ID3D11RasterizerState ) NoCulling;
	MicrosoftPointer( ID3D11SamplerState ) LinearWrapSampler;
	MicrosoftPointer(ID3D11SamplerState) LinearClampSampler;
	void InitializeStates( ID3D11Device * device )
	{

		ZeroMemoryAndDeclare( D3D11_RASTERIZER_DESC, rastDesc );
		rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		ThrowIfFailed( device->CreateRasterizerState( &rastDesc, &NoCulling ) );
		ZeroMemoryAndDeclare( D3D11_SAMPLER_DESC, sampDesc );
		sampDesc.AddressU =
			sampDesc.AddressV =
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.MaxAnisotropy = 16;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		sampDesc.MipLODBias = 0;
		ThrowIfFailed( device->CreateSamplerState( &sampDesc, &LinearWrapSampler ) );
		sampDesc.AddressU =
			sampDesc.AddressV =
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		ThrowIfFailed(device->CreateSamplerState(&sampDesc, &LinearClampSampler ));
	}
}