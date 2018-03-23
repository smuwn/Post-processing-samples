#include "Texture.h"



CTexture::CTexture( LPWSTR lpPath, ID3D11Device* device )
{
	// Using D3DX is not recommended anymore, but it'll do the job for now
	ThrowIfFailed(
		D3DX11CreateShaderResourceViewFromFile( device, lpPath, nullptr, nullptr, &mTextureSRV, nullptr )
		);
}

CTexture::CTexture(LPWSTR lpPath, ID3D11Device * device, ID3D11DeviceContext * context, bool hasUAV) :
	mDevice(device)
{
	if (hasUAV)
	{
		ThrowIfFailed(
			CreateWICTextureFromFile(device, context, lpPath,
				reinterpret_cast<ID3D11Resource**>(mTexture.GetAddressOf()), &mTextureSRV,
				D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS)
			 );
		CreateUAV();
	}
	else
	{
		ThrowIfFailed(
			CreateWICTextureFromFile(device, context, lpPath,
				reinterpret_cast<ID3D11Resource**>(mTexture.GetAddressOf()), &mTextureSRV)
		);
	}
}


CTexture::~CTexture( )
{
	mTextureSRV.Reset( );
}

void CTexture::CreateUAV()
{
	ID3D11Texture2D * texture;
	mTextureSRV->GetResource(reinterpret_cast<ID3D11Resource**>(&texture));
	if (texture == nullptr)
		throw std::exception("Can't create a UAV from a null texture");

	TextureUtilities::CreateUAVFromTexture(mDevice, texture, &mTextureUAV);
}
