#pragma once

#include "WICTextureLoader.h"
#include "TextureUtilities.h"
#include "commonincludes.h"




class CTexture
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureSRV;

	bool mHasUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mTextureUAV;

	ID3D11Device * mDevice;
public:
	CTexture( ) = default;
	[[deprecated]]
	CTexture( LPWSTR lpPath, ID3D11Device * );
	CTexture(LPWSTR lpPath, ID3D11Device *, ID3D11DeviceContext *, bool hasUAV = false);
	~CTexture( );
public:
	ID3D11ShaderResourceView* GetTextureSRV() const
	{
		return mTextureSRV.Get( );
	}
	void SetTexture( ID3D11ShaderResourceView * newSRV )
	{
		mTextureSRV.Reset( );
		mTextureSRV = Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>( newSRV );
	}
private:
	void CreateUAV();
};

