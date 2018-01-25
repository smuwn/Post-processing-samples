#include "GrayScale.h"



GrayScale::GrayScale( MicrosoftPointer( ID3D11Device ) Device, MicrosoftPointer( ID3D11DeviceContext ) Context,
	ID3D11ShaderResourceView * SRV ) :
	mDevice( Device ),
	mContext( Context )
{
	try
	{
		CreateShader( );
		CreateTexture( SRV );
	}
	CATCH;
}


GrayScale::~GrayScale( )
{
}


void GrayScale::CreateTexture( ID3D11ShaderResourceView * SRV )
{
	ID3D11Resource * Resource;
	SRV->GetResource( &Resource );
	ID3D11Texture2D * texture2D = static_cast< ID3D11Texture2D* >( Resource );
	texture2D->GetDesc( &mTextureDesc );
	ZeroMemoryAndDeclare( D3D11_TEXTURE2D_DESC, texDesc );
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE |
		D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
	texDesc.Format = mTextureDesc.Format;
	texDesc.Width = mTextureDesc.Width;
	texDesc.Height = mTextureDesc.Height;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc = mTextureDesc.SampleDesc;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	ThrowIfFailed( mDevice->CreateTexture2D( &texDesc, nullptr, &mTexture ) );
	ZeroMemoryAndDeclare( D3D11_SHADER_RESOURCE_VIEW_DESC, srvDesc );
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	ThrowIfFailed( mDevice->CreateShaderResourceView( mTexture.Get( ), &srvDesc, &mTextureSRV ) );
	ZeroMemoryAndDeclare( D3D11_UNORDERED_ACCESS_VIEW_DESC, uavDesc );
	uavDesc.Texture2D.MipSlice = 0;
	uavDesc.Format = texDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
	ThrowIfFailed( mDevice->CreateUnorderedAccessView( mTexture.Get( ), &uavDesc, &mTextureUAV ) );
}

void GrayScale::CreateShader( )
{
	ID3D11ComputeShader ** CS = &mComputeShader;
	ShaderHelper::CreateShaderFromFile(
		( LPWSTR ) L"Shaders/GrayScaleComputeShader.cso", ( LPSTR )"cs_5_0",
		mDevice.Get( ), &mComputeShaderBlob, reinterpret_cast< ID3D11DeviceChild** >( CS ) );
}

void GrayScale::Apply( ID3D11ShaderResourceView * SRV )
{
	mContext->CSSetShader( mComputeShader.Get( ), nullptr, 0 );
	mContext->CSSetSamplers( 0, 1, DX::LinearWrapSampler.GetAddressOf( ) );
	mContext->CSSetShaderResources( 0, 1, &SRV );
	mContext->CSSetUnorderedAccessViews( 0, 1, mTextureUAV.GetAddressOf( ), nullptr );

	int numThreadsX = ( int ) ceil( ( float ) mTextureDesc.Width / 32.0f );
	int numThreadsY = ( int ) ceil( ( float ) mTextureDesc.Height / 32.0f );

	mContext->Dispatch( numThreadsX, numThreadsY, 1 );

	mContext->CSSetShader( nullptr, nullptr, 0 );
	ID3D11UnorderedAccessView * nullUAV[ ] = { nullptr };
	mContext->CSSetUnorderedAccessViews( 0, 1, nullUAV, nullptr );
}