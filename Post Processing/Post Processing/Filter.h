#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"
#include "TextureUtilities.h"


class Filter
{

protected:
	virtual void CreateShader() = 0;
	virtual void InitializeGroupSize() = 0;
	virtual void CreateAdditional();
protected:
	void Create(MicrosoftPointer(ID3D11Device) Device, MicrosoftPointer(ID3D11DeviceContext) Context,
		ID3D11ShaderResourceView * SRV);
public:
	virtual void Apply(ID3D11ShaderResourceView * SRV);
	virtual void CreateTexture(ID3D11ShaderResourceView * SRV);

public:
	inline ID3D11ShaderResourceView* GetTextureSRV() { return mTextureSRV.Get(); };
protected:
	MicrosoftPointer(ID3D11ComputeShader) mComputeShader;
	MicrosoftPointer(ID3DBlob) mComputeShaderBlob;

	D3D11_TEXTURE2D_DESC mTextureDesc;

	UINT mGroupThreadsX;
	UINT mGroupThreadsY;
	UINT mGroupThreadsZ;

	MicrosoftPointer(ID3D11Texture2D) mTexture;
	MicrosoftPointer(ID3D11ShaderResourceView) mTextureSRV;
	MicrosoftPointer(ID3D11UnorderedAccessView) mTextureUAV;

	MicrosoftPointer(ID3D11Device) mDevice;
	MicrosoftPointer(ID3D11DeviceContext) mContext;
};
