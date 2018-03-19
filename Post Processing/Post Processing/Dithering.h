#pragma once

#include "Filter.h"


class Dithering : public Filter
{
public:
	struct SPaletteInfo
	{
		DirectX::XMFLOAT4 Colors[8];
		UINT numColors;
		DirectX::XMFLOAT3 pad;
	};
private:
	MicrosoftPointer(ID3D11Buffer) mPaletteInfoBuffer;
	SPaletteInfo mPaletteInfo;
public:
	Dithering(MicrosoftPointer(ID3D11Device) Device, MicrosoftPointer(ID3D11DeviceContext) Context,
		ID3D11ShaderResourceView * Texture);
	~Dithering();
public:
	void Apply(ID3D11ShaderResourceView * SRV)			override;
public:
	void SetPaletteInfo(SPaletteInfo const& pal);
	SPaletteInfo& GetPaletteInfo();
protected:
	void CreateShader()									override;
	void CreateAdditional()								override;
	void InitializeGroupSize()							override;
};

