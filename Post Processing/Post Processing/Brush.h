#pragma once

#include "ShaderHelper.h"
#include "TextureUtilities.h"
#include "commonincludes.h"
#include "Filter.h"
#include "Texture.h"
#include "CopyImage.h"

class Brush : public Filter
{
public:
	struct SWindowInfo
	{
		UINT width;
		UINT height;
		DirectX::XMFLOAT2 pad;
	};
	struct SCursorInfo
	{
		UINT cursorX;
		UINT cursorY;
		float thickness;
		float radius;
	};
public:
	Brush(MicrosoftPointer(ID3D11Device) Device, MicrosoftPointer(ID3D11DeviceContext) Context,
		ID3D11ShaderResourceView * SRV);
	~Brush();
public:
	void CreateShader()								override;
	void InitializeGroupSize()						override;
	void CreateAdditional()							override;
public:
	void SetPattern(LPWSTR lpPath);
	void SetWindowInfo(SWindowInfo const& info);
	void SetCursorInfo(SCursorInfo const& info);
public:
	void Apply(ID3D11ShaderResourceView *)			override;

	void Hover(ID3D11ShaderResourceView *);
	void Action(ID3D11ShaderResourceView *, ID3D11UnorderedAccessView *);
private:
	MicrosoftPointer(ID3D11ComputeShader) mHoverComputeShader;
	MicrosoftPointer(ID3DBlob) mHoverComputeShaderBlob;

	MicrosoftPointer(ID3D11ComputeShader) mActionComputeShader;
	MicrosoftPointer(ID3DBlob) mActionComputeShaderBlob;

	MicrosoftPointer(ID3D11Buffer) mCursorInfoBuffer;
	MicrosoftPointer(ID3D11Buffer) mWindowInfoBuffer;

	std::unique_ptr<CTexture> mBrushPattern;
};

