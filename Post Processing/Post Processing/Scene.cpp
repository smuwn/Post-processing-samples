#include "Scene.h"

Scene * Scene::mSceneInstance = nullptr;

Scene::Scene( HINSTANCE Instance, bool bFullscreen ) :
	mInstance( Instance )
{
	try
	{
		InitWindow( bFullscreen );
		InitD3D( bFullscreen );
		InitShaders( );
		Init2D( );
		InitFilters( );
	}
	CATCH;
}


Scene::~Scene( )
{
	mDevice.Reset( );
	mImmediateContext.Reset( );
	mBackbuffer.Reset( );
	mSwapChain.Reset( );

	mInput.reset( );

	UnregisterClass( ENGINE_NAME, mInstance );
	DestroyWindow( mWindow );
}

void Scene::Create(HINSTANCE instance, bool bFullscreen)
{
	if (mSceneInstance == nullptr)
		mSceneInstance = new Scene(instance, bFullscreen);
}

Scene* Scene::GetSceneInstance()
{
	return mSceneInstance;
}

void Scene::Destroy()
{
	delete mSceneInstance;
}

void Scene::InitWindow( bool bFullscreen )
{
	ZeroMemoryAndDeclare( WNDCLASSEX, wndClass );
	wndClass.cbSize = sizeof( WNDCLASSEX );
	wndClass.hbrBackground = ( HBRUSH ) ( GetStockObject( DKGRAY_BRUSH ) );
	wndClass.hInstance = mInstance;
	wndClass.lpfnWndProc = Scene::WndProc;
	wndClass.lpszClassName = ENGINE_NAME;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	if ( !RegisterClassEx( &wndClass ) )
		throw std::exception( "Couldn't register window class" );
	
	if ( !bFullscreen )
	{
		mWidth = 800;
		mHeight = 600;
	}
	else
	{
		mWidth = GetSystemMetrics( SM_CXSCREEN );
		mHeight = GetSystemMetrics( SM_CYSCREEN );
	}
	
	mWindow = CreateWindow(ENGINE_NAME, ENGINE_NAME,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, mWidth, mHeight,
		nullptr, nullptr, mInstance, nullptr);
	if ( !mWindow )
		throw std::exception( "Couldn't create window" );
		
	UpdateWindow( mWindow );
	ShowWindow( mWindow, SW_SHOWNORMAL );
	SetFocus( mWindow );

	mFullscreenViewport.Width = ( FLOAT ) mWidth;
	mFullscreenViewport.Height = ( FLOAT ) mHeight;
	mFullscreenViewport.TopLeftX = 0;
	mFullscreenViewport.TopLeftY = 0;
	mFullscreenViewport.MinDepth = 0.0f;
	mFullscreenViewport.MaxDepth = 1.0f;
	mOrthoMatrix = DirectX::XMMatrixOrthographicLH( ( float ) mWidth, ( float ) mHeight, DX::CamNear, DX::CamFar );
}

void Scene::InitD3D( bool bFullscreen )
{
	IDXGIFactory * Factory;
	ThrowIfFailed( CreateDXGIFactory( __uuidof( IDXGIFactory ),
		reinterpret_cast< void** >( &Factory ) ) );
	IDXGIAdapter * Adapter;
	ThrowIfFailed( Factory->EnumAdapters( 0, &Adapter ) );
	IDXGIOutput * Output;
	ThrowIfFailed( Adapter->EnumOutputs( 0, &Output ) );
	UINT NumModes;
	ThrowIfFailed( Output->GetDisplayModeList( DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &NumModes, nullptr ) );
	DXGI_MODE_DESC * Modes = new DXGI_MODE_DESC[ NumModes ];
	ThrowIfFailed( Output->GetDisplayModeList( DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &NumModes, Modes ) );
	DXGI_MODE_DESC FinalMode;
	bool bFound = false;
	for ( size_t i = 0; i < NumModes; ++i )
	{
		if ( Modes[ i ].Width == mWidth && Modes[ i ].Height == mHeight )
		{
			FinalMode = DXGI_MODE_DESC( Modes[ i ] );
			bFound = true;
			break;
		}
	}
	if ( !bFound )
	{
		FinalMode.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		FinalMode.Width = mWidth;
		FinalMode.Height = mHeight;
		FinalMode.RefreshRate.Denominator = 0;
		FinalMode.RefreshRate.Numerator = 60;
		FinalMode.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
		FinalMode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	}
	delete[ ] Modes;
	DXGI_ADAPTER_DESC GPU;
	Adapter->GetDesc( &GPU );
	mGPUDescription = GPU.Description;
	ZeroMemoryAndDeclare( DXGI_SWAP_CHAIN_DESC, swapDesc );
	swapDesc.BufferCount = 1;
	swapDesc.BufferDesc = FinalMode;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.OutputWindow = mWindow;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Windowed = !bFullscreen;

	// MSAA
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	UINT flags = 0;
#if DEBUG || _DEBUG
	flags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driver =
#if defined NO_GPU
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_WARP
#else
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE
#endif
		;

	ThrowIfFailed(
		D3D11CreateDeviceAndSwapChain( NULL, driver, NULL, flags,
			NULL, NULL, D3D11_SDK_VERSION, &swapDesc, &mSwapChain, &mDevice, NULL, &mImmediateContext )
	);

	ID3D11Texture2D * backBufferResource;
	ThrowIfFailed( mSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ),
		reinterpret_cast< void** >( &backBufferResource ) ) );
	ThrowIfFailed(
		mDevice->CreateRenderTargetView(
			backBufferResource, nullptr, &mBackbuffer
		)
	);

	backBufferResource->Release( );
	Factory->Release( );
	Adapter->Release( );
	Output->Release( );
	mInput = std::make_shared<CInput>( );
	mInput->Initialize( mInstance, mWindow );
	DX::InitializeStates( mDevice.Get( ) );
	mImmediateContext->RSSetState( DX::NoCulling.Get( ) );
	mImmediateContext->RSSetViewports( 1, &mFullscreenViewport );
}

void Scene::InitShaders( )
{
	m2DShader = std::make_shared<C2DShader>( mDevice.Get( ), mImmediateContext.Get( ) );
}

void Scene::Init2D( )
{
	m32OpenSans = std::make_shared<CFont>( mDevice.Get( ), mImmediateContext.Get( ),
		( LPWSTR ) L"Data/32OpenSans.fnt" );
	
	mChrissy = std::make_unique<CTexture>(
		(LPWSTR)L"Data/Stock/Chrissy.jpg",
		mDevice.Get(), mImmediateContext.Get(),
		true
		);

	mAfter = std::make_unique<Square>( mDevice.Get( ), mImmediateContext.Get( ),
		m2DShader, mWidth, mHeight, mWidth, mHeight );
	mAfter->TranslateTo( 0, 0 );

#if DEBUG || _DEBUG
	mFPSText = std::make_unique<CText>(mDevice.Get(), mImmediateContext.Get(),
		m2DShader, m32OpenSans, mWidth, mHeight);
	mDebugText = std::make_unique<CText>( mDevice.Get( ), mImmediateContext.Get( ),
		m2DShader, m32OpenSans, mWidth, mHeight );
#endif
}

void Scene::InitFilters( )
{
	mGrayScaleFilter = std::make_unique<GrayScale>(mDevice, mImmediateContext, mChrissy->GetTextureSRV());
	mDitheringFilter = std::make_unique<Dithering>(mDevice, mImmediateContext, mChrissy->GetTextureSRV());
	Dithering::SPaletteInfo palette;
	palette.Colors[0] = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	palette.Colors[1] = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	palette.Colors[2] = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	palette.Colors[3] = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	palette.Colors[4] = DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	palette.Colors[5] = DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	palette.Colors[6] = DirectX::XMFLOAT4(0.71f, 0.62f, 0.57f, 1.0f);
	palette.numColors = 7;
	mDitheringFilter->SetPaletteInfo(palette);
	
	mBrush = std::make_unique<Brush>(mDevice, mImmediateContext, mChrissy->GetTextureSRV());
	mBrush->SetPattern((LPWSTR)L"Data/Stock/Pattern.png");
	Brush::SWindowInfo wi;
	wi.width = mWidth;
	wi.height = mHeight;
	mBrush->SetWindowInfo(wi);

	mAfter->SetTexture(mBrush->GetTextureSRV());
}

LRESULT Scene::WndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	switch ( Message )
	{
	case WM_MOUSEMOVE:
		if (mSceneInstance)
		{
			UINT X = LOWORD(lParam);
			UINT Y = HIWORD(lParam);

			mSceneInstance->mInput->SetCursorPosition(X, Y);
		}
		break;
	case WM_SIZE:
		if (mSceneInstance)
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);
			mSceneInstance->mWidth = width;
			mSceneInstance->mHeight = height;
			Brush::SWindowInfo wi;
			wi.width = width;
			wi.height = height;
			mSceneInstance->mBrush->SetWindowInfo(wi);
		}
		break;
	case WM_QUIT:
		DestroyWindow( hWnd );
		break;
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	default:
		break;
	}
	return DefWindowProc( hWnd, Message, wParam, lParam );
}

void Scene::Run( )
{
	MSG Message;
	mTimer.Start( );
	while ( true )
	{
		if ( PeekMessage( &Message, nullptr, 0, 0, PM_REMOVE ) )
		{
			if ( Message.message == WM_QUIT )
				break;
			TranslateMessage( &Message );
			DispatchMessage( &Message );
		}
		else
		{
			if ( mInput->isKeyPressed( DIK_ESCAPE ) )
				break;
			if ( mTimer.GetTimeSinceLastStart( ) > 1.0f )
				mTimer.Start( );
			Update( );
			Render( );
		}
	}
}

void Scene::EnableBackbuffer( )
{
	mImmediateContext->RSSetViewports( 1, &mFullscreenViewport );
	mImmediateContext->OMSetRenderTargets( 1, mBackbuffer.GetAddressOf( ), nullptr );
}

void Scene::Update( )
{
	static Brush::SCursorInfo cursorInfo;
	static float brushRadius = 16;

	mTimer.Frame();
	mInput->Frame();
	float frameTime = mTimer.GetFrameTime();

	if (mInput->isKeyPressed(DIK_ADD))
	{
		brushRadius++;
		if (brushRadius >= 72.f)
			brushRadius = 72.f;
	}
	if (mInput->isKeyPressed(DIK_SUBTRACT))
	{
		brushRadius--;
		if (brushRadius <= 1)
			brushRadius = 1;
	}

	cursorInfo.thickness = .5f;
	cursorInfo.radius = brushRadius;
	cursorInfo.cursorX = mInput->GetCursorX();
	cursorInfo.cursorY = mInput->GetCursorY();

	mBrush->SetCursorInfo(cursorInfo);

	if (mInput->isLeftKeyPressed())
	{
		mBrush->Action(mChrissy->GetTextureSRV());
	}
	else
	{
		mBrush->Hover(mChrissy->GetTextureSRV());
	}

 #if DEBUG || _DEBUG
	frameTime = 1.f / 60.f;
#endif

#if DEBUG || _DEBUG
	wchar_t buffer[ 500 ];
	swprintf_s( buffer, L"%ws: %d, %.2f", ENGINE_NAME, mTimer.GetFPS( ), frameTime );
	SetWindowText( mWindow, buffer );
#endif
	
}

void Scene::Render( )
{
	using namespace DirectX;
	static FLOAT BackColor[ 4 ] = { 0,0,0,0 };
	EnableBackbuffer( );
	mImmediateContext->ClearRenderTargetView( mBackbuffer.Get( ), BackColor );

	mAfter->Render( mOrthoMatrix );


#if DEBUG || _DEBUG
	char buffer[ 256 ];
	sprintf_s(buffer, "FPS: %d", mTimer.GetFPS());
	mFPSText->Render(mOrthoMatrix, buffer, 0, 0,
		DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
	sprintf_s( buffer, "DEBUG MODE" );
	mDebugText->Render( mOrthoMatrix, buffer, 0, 35,
		DirectX::XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) );
#endif

	mSwapChain->Present( 1, 0 );
}