#pragma once


#include "commonincludes.h"
#include "ShaderHelper.h"
#include "TextureUtilities.h"


class CCopyImage
{
	static CCopyImage * mInstance;
private:
	CCopyImage();
	CCopyImage(CCopyImage const&) = delete;
	const CCopyImage& operator = (CCopyImage const&) = delete;
	~CCopyImage();
public:
	void Copy(ID3D11ShaderResourceView * FROM, ID3D11UnorderedAccessView * TO);
public:
	static void CreateInstance(MicrosoftPointer(ID3D11Device) Device, MicrosoftPointer(ID3D11DeviceContext) Context);
	static CCopyImage* GetInstance();
	static void DestroyInstance();
protected:
	void Create(MicrosoftPointer(ID3D11Device) Device, MicrosoftPointer(ID3D11DeviceContext) Context);

private:
	void CreateShaders();
	void InitializeGroupSize(float width, float height);
protected:
	MicrosoftPointer(ID3D11Device) mDevice;
	MicrosoftPointer(ID3D11DeviceContext) mContext;

	MicrosoftPointer(ID3D11ComputeShader) mComputeShader;
	MicrosoftPointer(ID3DBlob) mComputeShaderBlob;

	UINT mGroupThreadsX;
	UINT mGroupThreadsY;
	UINT mGroupThreadsZ;
};

