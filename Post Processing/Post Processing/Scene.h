#pragma once

#include "commonincludes.h"
#include "Input.h"
#include "HRTimer.h"
#include "Font.h"
#include "Text.h"
#include "C2DShader.h"
#include "Square.h"

ALIGN16 class Scene
{
private:
	HWND mWindow;
	HINSTANCE mInstance;
	D3D11_VIEWPORT mFullscreenViewport;

	CHRTimer mTimer;
	WCHAR* mGPUDescription;

	int mWidth;
	int mHeight;

	std::unique_ptr<Square> mChrissy;

	std::shared_ptr<CInput> mInput;
	std::shared_ptr<C2DShader> m2DShader;
	
	std::shared_ptr<CFont> m32OpenSans;

#if DEBUG || _DEBUG
	std::unique_ptr<CText> mDebugText;
#endif

	DirectX::XMMATRIX mOrthoMatrix;


private: // D3D objects
	Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mBackbuffer;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
public:
	Scene( HINSTANCE Instance, bool bFullscreen = false );
	~Scene( );
public:
	void Run( );
private:
	void InitWindow( bool bFullscreen );
	void InitD3D( bool bFullscreen );
	void InitShaders( );
	void Init2D( );
private:
	void EnableBackbuffer( );
	void Update( );
	void Render( );
public:
	static LRESULT CALLBACK WndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam );
public:
	inline void* operator new( size_t size )
	{
		return _aligned_malloc( size, 16 );
	}
	inline void operator delete( void* object )
	{
		return _aligned_free( object );
	}
};

