#include "Brush.h"



Brush::Brush(MicrosoftPointer(ID3D11Device) Device, MicrosoftPointer(ID3D11DeviceContext) Context,
	ID3D11ShaderResourceView * SRV) :
	Filter()
{
	Create(Device, Context, SRV);
}


Brush::~Brush()
{
}

void Brush::CreateShader()
{
	ID3D11ComputeShader ** CS = &mHoverComputeShader;
	ShaderHelper::CreateShaderFromFile(
		(LPWSTR)L"Shaders/BrushHoverComputeShader.cso", (LPSTR)"cs_5_0",
		mDevice.Get(), &mHoverComputeShaderBlob, reinterpret_cast< ID3D11DeviceChild** >(CS));

	CS = &mActionComputeShader;
	ShaderHelper::CreateShaderFromFile(
		(LPWSTR)L"Shaders/BrushActionComputeShader.cso", (LPSTR)"cs_5_0",
		mDevice.Get(), &mActionComputeShaderBlob, reinterpret_cast< ID3D11DeviceChild** >(CS));
}

void Brush::InitializeGroupSize()
{
	mGroupThreadsX = (int)ceil((float)mTextureDesc.Width / 32.f);
	mGroupThreadsY = (int)ceil((float)mTextureDesc.Height / 32.f);
	mGroupThreadsZ = 1;
}

void Brush::CreateAdditional()
{
	ShaderHelper::CreateBuffer(mDevice.Get(), mCursorInfoBuffer.GetAddressOf(),
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof(SCursorInfo), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE);
	ShaderHelper::CreateBuffer(mDevice.Get(), mWindowInfoBuffer.GetAddressOf(),
		D3D11_USAGE::D3D11_USAGE_DEFAULT, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof(SWindowInfo), 0);
}

void Brush::SetPattern(LPWSTR lpPath)
{
	mBrushPattern = std::make_unique<CTexture>(lpPath, mDevice.Get(), mContext.Get());
}

void Brush::SetWindowInfo(SWindowInfo const & info)
{
	mContext->UpdateSubresource(mWindowInfoBuffer.Get(), 0, nullptr, &info, 0, 0);
}

void Brush::SetCursorInfo(SCursorInfo const & info)
{
	static D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	mContext->Map(mCursorInfoBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy_s(mappedSubresource.pData, sizeof(SCursorInfo), (void*)&info, sizeof(SCursorInfo));
	mContext->Unmap(mCursorInfoBuffer.Get(), 0);
}

void Brush::Apply(ID3D11ShaderResourceView *)
{
	return;
}

void Brush::Hover(ID3D11ShaderResourceView * SRV)
{
	mContext->CSSetShader(mHoverComputeShader.Get(), nullptr, 0);
	mContext->CSSetSamplers(0, 1, DX::LinearWrapSampler.GetAddressOf());
	mContext->CSSetShaderResources(0, 1, &SRV);
	ID3D11ShaderResourceView * brushPatternSRV = mBrushPattern->GetTextureSRV();
	mContext->CSSetShaderResources(1, 1, &brushPatternSRV);
	mContext->CSSetUnorderedAccessViews(0, 1, mTextureUAV.GetAddressOf(), nullptr);

	mContext->CSSetConstantBuffers(0, 1, mWindowInfoBuffer.GetAddressOf());
	mContext->CSSetConstantBuffers(1, 1, mCursorInfoBuffer.GetAddressOf());

	InitializeGroupSize();

	mContext->Dispatch(mGroupThreadsX, mGroupThreadsY, mGroupThreadsZ);

	mContext->CSSetShader(nullptr, nullptr, 0);
	ID3D11UnorderedAccessView * nullUAV[] = { nullptr };
	mContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
}

void Brush::Action(ID3D11ShaderResourceView * SRV, ID3D11UnorderedAccessView * UAV)
{
	mContext->CSSetShader(mActionComputeShader.Get(), nullptr, 0);
	mContext->CSSetSamplers(0, 1, DX::LinearWrapSampler.GetAddressOf());
	mContext->CSSetShaderResources(0, 1, &SRV);
	ID3D11ShaderResourceView * brushPatternSRV = mBrushPattern->GetTextureSRV();
	mContext->CSSetShaderResources(1, 1, &brushPatternSRV);
	mContext->CSSetUnorderedAccessViews(0, 1, mTextureUAV.GetAddressOf(), nullptr);

	mContext->CSSetConstantBuffers(0, 1, mWindowInfoBuffer.GetAddressOf());
	mContext->CSSetConstantBuffers(1, 1, mCursorInfoBuffer.GetAddressOf());

	InitializeGroupSize();

	mContext->Dispatch(mGroupThreadsX, mGroupThreadsY, mGroupThreadsZ);

	mContext->CSSetShader(nullptr, nullptr, 0);
	ID3D11UnorderedAccessView * nullUAV[] = { nullptr };
	mContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

	ID3D11Resource * inputTexture;
	SRV->GetResource(&inputTexture);

	mContext->CopySubresourceRegion(inputTexture, 0, 0, 0, 0,
		mTexture.Get(), 0, NULL);
}
