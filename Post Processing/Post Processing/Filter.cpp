#include "Filter.h"


void Filter::CreateAdditional()
{
	return;
}

void Filter::Create(MicrosoftPointer(ID3D11Device) Device, MicrosoftPointer(ID3D11DeviceContext) Context,
	ID3D11ShaderResourceView * SRV)
{
	mDevice = Device;
	mContext = Context;
	try
	{
		CreateShader();
		CreateTexture(SRV);
		CreateAdditional();
	}
	CATCH;
}


void Filter::Apply(ID3D11ShaderResourceView * SRV)
{
	mContext->CSSetShader(mComputeShader.Get(), nullptr, 0);
	mContext->CSSetSamplers(0, 1, DX::LinearWrapSampler.GetAddressOf());
	mContext->CSSetShaderResources(0, 1, &SRV);
	mContext->CSSetUnorderedAccessViews(0, 1, mTextureUAV.GetAddressOf(), nullptr);
	
	InitializeGroupSize();

	mContext->Dispatch(mGroupThreadsX, mGroupThreadsY, mGroupThreadsZ);

	mContext->CSSetShader(nullptr, nullptr, 0);
	ID3D11UnorderedAccessView * nullUAV[] = { nullptr };
	mContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
}

void Filter::CreateTexture(ID3D11ShaderResourceView * SRV)
{
	ID3D11Resource * Resource;
	SRV->GetResource(&Resource);
	ID3D11Texture2D * texture2D = static_cast< ID3D11Texture2D* >(Resource);
	texture2D->GetDesc(&mTextureDesc);
	ZeroMemoryAndDeclare(D3D11_TEXTURE2D_DESC, texDesc);
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE |
		D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
	texDesc.Format = mTextureDesc.Format;
	texDesc.Width = mTextureDesc.Width;
	texDesc.Height = mTextureDesc.Height;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc = mTextureDesc.SampleDesc;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	ThrowIfFailed(mDevice->CreateTexture2D(&texDesc, nullptr, &mTexture));
	TextureUtilities::CreateSRVFromTexture(mDevice.Get(), mTexture.Get(), &mTextureSRV);
	TextureUtilities::CreateUAVFromTexture(mDevice.Get(), mTexture.Get(), &mTextureUAV);
}