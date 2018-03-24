#include "CopyImage.h"

CCopyImage* CCopyImage::mInstance = nullptr;

CCopyImage::CCopyImage()
{
}


CCopyImage::~CCopyImage()
{
}

void CCopyImage::CreateInstance(MicrosoftPointer(ID3D11Device) device, MicrosoftPointer(ID3D11DeviceContext) context)
{
	if (mInstance == nullptr)
	{
		mInstance = new CCopyImage();
		mInstance->Create(device, context);
	}
}

void CCopyImage::DestroyInstance()
{
	delete mInstance;
}

CCopyImage* CCopyImage::GetInstance()
{
	return mInstance;
}

void CCopyImage::Copy(ID3D11ShaderResourceView * SRV, ID3D11UnorderedAccessView * UAV)
{
	ID3D11Texture2D * inputTexture;
	SRV->GetResource(reinterpret_cast<ID3D11Resource**>(&inputTexture));
	D3D11_TEXTURE2D_DESC texDesc;
	inputTexture->GetDesc(&texDesc);
	mContext->CSSetShader(mComputeShader.Get(), nullptr, 0);
	mContext->CSSetSamplers(0, 1, DX::LinearWrapSampler.GetAddressOf());
	mContext->CSSetShaderResources(0, 1, &SRV);
	mContext->CSSetUnorderedAccessViews(0, 1, &UAV, nullptr);

	InitializeGroupSize((FLOAT)texDesc.Width, (FLOAT)texDesc.Height);

	mContext->Dispatch(mGroupThreadsX, mGroupThreadsY, mGroupThreadsZ);

	mContext->CSSetShader(nullptr, nullptr, 0);
	ID3D11UnorderedAccessView * nullUAV[] = { nullptr };
	mContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
}

void CCopyImage::Create(MicrosoftPointer(ID3D11Device) Device, MicrosoftPointer(ID3D11DeviceContext) Context)
{
	mDevice = Device;
	mContext = Context;
	try
	{
		CreateShaders();
	}
	CATCH;
}

void CCopyImage::CreateShaders()
{
	ID3D11ComputeShader ** CS = mComputeShader.GetAddressOf();
	ShaderHelper::CreateShaderFromFile((LPWSTR)L"Shaders/CopyImageComputeShader.cso", (LPSTR)"cs_5_0",
		mDevice.Get(), &mComputeShaderBlob, reinterpret_cast<ID3D11DeviceChild**>(CS));
}

void CCopyImage::InitializeGroupSize(float width, float height)
{
	mGroupThreadsX = (int)ceil(width / 32.0f);
	mGroupThreadsY = (int)ceil(height / 32.0f);
	mGroupThreadsZ = 1;
}
