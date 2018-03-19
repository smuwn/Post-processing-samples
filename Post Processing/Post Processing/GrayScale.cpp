#include "GrayScale.h"



GrayScale::GrayScale(MicrosoftPointer(ID3D11Device) Device, MicrosoftPointer(ID3D11DeviceContext) Context,
	ID3D11ShaderResourceView * SRV) :
	Filter()
{
	Create(Device, Context, SRV);
}


GrayScale::~GrayScale( )
{
}

void GrayScale::CreateShader( )
{
	ID3D11ComputeShader ** CS = &mComputeShader;
	ShaderHelper::CreateShaderFromFile(
		( LPWSTR ) L"Shaders/GrayScaleComputeShader.cso", ( LPSTR )"cs_5_0",
		mDevice.Get( ), &mComputeShaderBlob, reinterpret_cast< ID3D11DeviceChild** >( CS ) );
}

void GrayScale::InitializeGroupSize()
{
	mGroupThreadsX = (int)ceil((float)mTextureDesc.Width / 32.0f);
	mGroupThreadsY = (int)ceil((float)mTextureDesc.Height / 32.0f);
	mGroupThreadsZ = 1;
}
