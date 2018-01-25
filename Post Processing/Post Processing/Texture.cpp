#include "Texture.h"



CTexture::CTexture( LPWSTR lpPath, ID3D11Device* device )
{
	// Using D3DX is not recommended anymore, but it'll do the job for now
	ThrowIfFailed(
		D3DX11CreateShaderResourceViewFromFile( device, lpPath, nullptr, nullptr, &mTextureSRV, nullptr )
		);
}


CTexture::~CTexture( )
{
	mTextureSRV.Reset( );
}
