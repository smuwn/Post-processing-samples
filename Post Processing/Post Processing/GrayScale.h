#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"


class GrayScale
{
private:
	MicrosoftPointer( ID3D11ComputeShader ) mComputeShader;
	MicrosoftPointer( ID3DBlob ) mComputeShaderBlob;

	D3D11_TEXTURE2D_DESC mTextureDesc;

	MicrosoftPointer( ID3D11Texture2D ) mTexture;
	MicrosoftPointer( ID3D11ShaderResourceView ) mTextureSRV;
	MicrosoftPointer( ID3D11UnorderedAccessView ) mTextureUAV;

	MicrosoftPointer( ID3D11Device ) mDevice;
	MicrosoftPointer( ID3D11DeviceContext ) mContext;
public:
	GrayScale( MicrosoftPointer( ID3D11Device ) Device, MicrosoftPointer( ID3D11DeviceContext ) Context,
		ID3D11ShaderResourceView * Texture );
	~GrayScale( );
public:
	void Apply( ID3D11ShaderResourceView * SRV );
	inline ID3D11ShaderResourceView* GetTexture( ) { return mTextureSRV.Get( ); };
private:
	void CreateTexture( ID3D11ShaderResourceView * SRV );
	void CreateShader( );
};

