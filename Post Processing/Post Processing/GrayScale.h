#pragma once

#include "Filter.h"


class GrayScale : public Filter
{
public:
	GrayScale( MicrosoftPointer( ID3D11Device ) Device, MicrosoftPointer( ID3D11DeviceContext ) Context,
		ID3D11ShaderResourceView * Texture );
	~GrayScale( );
protected:
	void CreateShader()									override;
	void InitializeGroupSize()							override;
};

