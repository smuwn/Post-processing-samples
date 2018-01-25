#pragma once

#include "commonincludes.h"



class CTexture
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureSRV;
public:
	CTexture( ) = default;
	CTexture( LPWSTR lpPath, ID3D11Device * );
	~CTexture( );
public:
	ID3D11ShaderResourceView* GetTexture() const
	{
		return mTextureSRV.Get( );
	}
	void SetTexture( ID3D11ShaderResourceView * newSRV )
	{
		mTextureSRV.Reset( );
		mTextureSRV = Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>( newSRV );
	}
};

