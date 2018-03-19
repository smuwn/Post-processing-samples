#include "Dithering.h"



Dithering::Dithering(MicrosoftPointer(ID3D11Device) Device, MicrosoftPointer(ID3D11DeviceContext) Context,
	ID3D11ShaderResourceView * Texture) :
	Filter()
{
	Create(Device, Context, Texture);
}


Dithering::~Dithering()
{
	mPaletteInfoBuffer.Reset();
}

void Dithering::Apply(ID3D11ShaderResourceView * SRV)
{
	mContext->CSSetConstantBuffers(0, 1, mPaletteInfoBuffer.GetAddressOf());
	Filter::Apply(SRV);
}

void Dithering::SetPaletteInfo(SPaletteInfo const & pal)
{
	mContext->UpdateSubresource(mPaletteInfoBuffer.Get(), 0, nullptr,
		&pal, 0, 0);
	mPaletteInfo = pal;
}

Dithering::SPaletteInfo & Dithering::GetPaletteInfo()
{
	return mPaletteInfo;
}

void Dithering::CreateShader()
{
	ID3D11ComputeShader ** CS = &mComputeShader;
	ShaderHelper::CreateShaderFromFile(
		(LPWSTR)L"Shaders/DitheringComputeShader.cso", (LPSTR)"cs_5_0",
		mDevice.Get(), &mComputeShaderBlob, reinterpret_cast< ID3D11DeviceChild** >(CS));
}

void Dithering::CreateAdditional()
{
	ShaderHelper::CreateBuffer(mDevice.Get(), &mPaletteInfoBuffer,
		D3D11_USAGE::D3D11_USAGE_DEFAULT, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof(SPaletteInfo), 0);
}

void Dithering::InitializeGroupSize()
{
	mGroupThreadsX = (int)ceil((float)mTextureDesc.Width / 16.0f);
	mGroupThreadsY = (int)ceil((float)mTextureDesc.Height / 16.0f);
	mGroupThreadsZ = 1;
}
