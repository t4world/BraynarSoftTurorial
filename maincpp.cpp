#pragma  comment(lib,"d3dx10.lib")
#pragma  comment(lib,"D3D10_1.lib")
#pragma  comment(lib,"DXGI.lib")
#pragma  comment(lib,"D2D1.lib")
#pragma  comment(lib,"dwrite.lib")
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <D3DX10Math.h>
#include <xnamath.h>
//Simple Font
#include <d3dx10.h>
#include <d3d10_1.h>
#include <d2d1.h>
#include <dxgi.h>
#include <sstream>
#include <dwrite.h>
//Simple Font

LPCTSTR wndClassName = L"FirstWindows";
HWND hwnd = NULL;

const int width = 800;
const int height = 600;

bool InitWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed);
bool messageLoop();

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//DirectX
IDXGISwapChain *swapChain;
ID3D11Device *d3d11Device;
ID3D11DeviceContext *d3d11DevicContext;
ID3D11RenderTargetView *d3d11RenderTargetView;
D3D_FEATURE_LEVEL g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11DepthStencilView *g_depthStencilView;
ID3D11Texture2D *g_depthStencilBuffer;
float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
int colormodr = 1;
int colormodg = 1;
int colormodb = 1;

struct Vertex
{
	Vertex(){

	}
	Vertex(float x, float y, float z,float u,float v) :pos(x, y, z),uv(u,v){}
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

D3D11_INPUT_ELEMENT_DESC m_gLayout[] = {
	{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA, 0},
	//{ "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
};

ID3D11Buffer *g_vertexBuffer;
ID3D11Buffer *g_indexBuffer;
ID3D11Buffer *g_indexBuffer2;
ID3D11VertexShader *g_vertexShader;
ID3D11PixelShader *g_pixelShader;
ID3D10Blob *g_vsBuffer;
ID3D10Blob *g_pxBuffer;
ID3D11InputLayout *g_verInputLayout;
ID3D11Buffer *g_constantBuffer;

ID3D11ShaderResourceView *g_cubeTexture;
ID3D11SamplerState *g_cubeTextureSimpleState;



struct CBuffer {
	XMMATRIX wvp;
};
XMMATRIX viewMatrix;
XMMATRIX projectMatrix;
XMMATRIX WVP;
CBuffer cbBuffer;
XMMATRIX cube1World;
XMMATRIX cube2World;
XMMATRIX cubeTranslate;
XMMATRIX cubeRotation;
XMMATRIX cubeScale;
float rotate = 0.0f;
XMVECTOR eyePos;

//Render state;
ID3D11RasterizerState *g_wireFrame;
ID3D11RasterizerState *g_normalFrame;
ID3D11BlendState *g_Transparency;
ID3D11RasterizerState *g_CCWcullMode;
ID3D11RasterizerState *g_CWcullMode;
ID3D11RasterizerState *g_NoCull;

//SimpleFont
ID3D10Device1 *m_d3d101Device;
IDXGIKeyedMutex *m_keyedMutex11;
IDXGIKeyedMutex *m_keyedMutex10;
ID2D1RenderTarget *m_d2dRenderTarget;
ID2D1SolidColorBrush *m_Brush;
//ID3D11Texture2D *m_BackBuffer11;
ID3D11Texture2D *m_sharedTex11;
ID3D11Buffer *m_d2dVertexBuffer;
ID3D11Buffer *m_d2dIndexBuffer;
ID3D11ShaderResourceView *m_d2dTexture;
IDWriteFactory *m_DWriteFactory;
IDWriteTextFormat *m_TextFormat;
std::wstring m_printText;
//SimpleFont


bool InitDirect3d11App(HINSTANCE hInstance);
void ReleaseObject();
bool InitScene();
void UpdateScene();
void DrawScene();

//SimpleFont;
bool InitD2D_D3D101_DWrite(IDXGIAdapter1 *adapter);
void InitD2DScreenTexture();
void RenderText(std::wstring text);
//SimpleFont

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, LPSTR lpCmdLine, int nShonwCmd)
{
	if (!InitWindow(hInstance, nShonwCmd, width, height, true))
	{
		MessageBox(0, L"Windows Inited Failed!", L"Error", MB_OK);
		return 0;
	}
	if (!InitDirect3d11App(hInstance))
	{
		MessageBox(0, L"Init Direct3d11 Failed", L"Error", MB_OK);
		return false;
	}
	if (!InitScene())
	{
		MessageBox(0, L"Init Scene Failed", L"Error", MB_OK);
		return false;
	}
	messageLoop();
	ReleaseObject();
	return 0;
}

bool InitWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = wndClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Error Register Class", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	hwnd = CreateWindowEx(NULL, wndClassName, L"Window Title", WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, NULL);
	if (!hwnd)
	{
		MessageBox(NULL, L"Error Create Window", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}
	ShowWindow(hwnd, ShowWnd);
	UpdateWindow(hwnd);
	return true;
}

bool messageLoop()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			UpdateScene();
			DrawScene();
		}
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case  WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			if (MessageBox(0, L"Are you sure you want to exit?", L"Realy?", MB_YESNO | MB_ICONQUESTION))
			{
				DestroyWindow(hWnd);
			}
		}
		return 0;
	case  WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool InitDirect3d11App(HINSTANCE hInstance)
{
	HRESULT hr;
	//Init Apater;
	IDXGIFactory1 *DXGIFactory;
	hr = CreateDXGIFactory1(_uuidof(IDXGIFactory1), (void**)&DXGIFactory);
	IDXGIAdapter1 *adapter;
	hr = DXGIFactory->EnumAdapters1(0, &adapter);
	DXGIFactory->Release();

	UINT creatDeviceFlag = 0;
#ifdef _DEBUG
	creatDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_DRIVER_TYPE driveTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT driveNum = ARRAYSIZE(driveTypes);
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	creatDeviceFlag |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	UINT featureLevelNum = ARRAYSIZE(featureLevels);

	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));
	bufferDesc.Width = width;
	bufferDesc.Height = height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;//DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapDesc.BufferDesc = bufferDesc;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 2;
	swapDesc.OutputWindow = hwnd;
	swapDesc.Windowed = TRUE;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//swapDesc.Flags = 
	//Create Swap Chain;
	for (UINT i = 0; i < driveNum;i++)
	{
		D3D_DRIVER_TYPE driveType = driveTypes[i];
		//hr = D3D11CreateDeviceAndSwapChain(adapter, driveType, NULL, creatDeviceFlag, featureLevels, featureLevelNum, D3D11_SDK_VERSION, &swapDesc, &swapChain,
		hr = D3D11CreateDeviceAndSwapChain(adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, creatDeviceFlag, featureLevels, featureLevelNum, D3D11_SDK_VERSION, &swapDesc, &swapChain,
			&d3d11Device, &g_featureLevel, &d3d11DevicContext);
		if (SUCCEEDED(hr))
		{
			break;;
		}
	}
	if (FAILED(hr))
	{
		MessageBox(0, L"Create Swap Chain Failed", L"Error", MB_OK);
		return false;
	}
	InitD2D_D3D101_DWrite(adapter);
	adapter->Release();
	//Create backBuffer;
	ID3D11Texture2D *backBuffer;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)(&backBuffer));
	//Create Render targetView;
	hr = d3d11Device->CreateRenderTargetView(backBuffer, NULL, &d3d11RenderTargetView);
	backBuffer->Release();
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &g_depthStencilBuffer);
	d3d11Device->CreateDepthStencilView(g_depthStencilBuffer, NULL, &g_depthStencilView);
	//Set render target;
	d3d11DevicContext->OMSetRenderTargets(1, &d3d11RenderTargetView, g_depthStencilView);
	return true;
}

void ReleaseObject()
{
	d3d11RenderTargetView->Release();
	swapChain->Release();
	d3d11DevicContext->Release();
	d3d11Device->Release();
	g_vertexBuffer->Release();
	g_indexBuffer->Release();
	g_indexBuffer2->Release();
	g_verInputLayout->Release();
	g_vertexShader->Release();
	g_pixelShader->Release();
	g_vsBuffer->Release();
	g_pxBuffer->Release();
	g_depthStencilView->Release();
	g_depthStencilBuffer->Release();
	g_constantBuffer->Release();
	g_wireFrame->Release();
	g_normalFrame->Release();
	g_cubeTexture->Release();
	g_cubeTextureSimpleState->Release();
	g_Transparency->Release();
	g_CCWcullMode->Release();
	g_CWcullMode->Release();

	m_d3d101Device->Release();
	m_keyedMutex11->Release();
	m_keyedMutex10->Release();
	m_d2dRenderTarget->Release();
	m_Brush->Release();
	//m_BackBuffer11->Release();
	m_sharedTex11->Release();
	m_d2dVertexBuffer->Release();
	m_d2dIndexBuffer->Release();
	m_d2dTexture->Release();
	m_DWriteFactory->Release();
	m_TextFormat->Release();
}

bool InitScene()
{
	InitD2DScreenTexture();
	HRESULT hr;
	hr = D3DX11CompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_5_0", 0, 0, 0, &g_vsBuffer, 0, 0);//CompileShaderFromFile( L"Effects.fx", "VS", "vs_4_0", &g_vsBuffer );
	if (FAILED(hr))
	{
		MessageBox(0, L"Effects.fx vs shader compiler Error!", L"Error", MB_OK);
		return false;
	}
	hr = d3d11Device->CreateVertexShader(g_vsBuffer->GetBufferPointer(), g_vsBuffer->GetBufferSize(), NULL, &g_vertexShader);
	hr = D3DX11CompileFromFile(L"Effects.fx", 0, 0, "PS", "ps_5_0", 0, 0, 0, &g_pxBuffer, 0, 0);
	if (FAILED(hr))
	{
		MessageBox(0, L"Effects.fx ps shader compiler Error!", L"Error", MB_OK);
		return false;
	}
	hr = d3d11Device->CreatePixelShader(g_pxBuffer->GetBufferPointer(), g_pxBuffer->GetBufferSize(), NULL, &g_pixelShader);
	d3d11DevicContext->VSSetShader(g_vertexShader, NULL, 0);
	d3d11DevicContext->PSSetShader(g_pixelShader, NULL, 0);

	Vertex vertices[] = {
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

		// Back Face
		Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f),
		Vertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
		Vertex(1.0f,  1.0f, 1.0f, 0.0f, 0.0f),
		Vertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f),

		// Top Face
		Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f),
		Vertex(1.0f, 1.0f,  1.0f, 1.0f, 0.0f),
		Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f),

		// Bottom Face
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
		Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(1.0f, -1.0f,  1.0f, 0.0f, 0.0f),
		Vertex(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f),

		// Left Face
		Vertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f),
		Vertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

		// Right Face
		Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		Vertex(1.0f,  1.0f,  1.0f, 1.0f, 0.0f),
		Vertex(1.0f, -1.0f,  1.0f, 1.0f, 1.0f),
	};

	DWORD indexArr[] = {
		0, 1, 2,
		0, 2, 3
	};
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	UINT indexNum = ARRAYSIZE(indexArr);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.ByteWidth = sizeof(DWORD) * indexNum;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	vertexBufferData.pSysMem = indexArr;
	hr = d3d11Device->CreateBuffer(&bufferDesc, &vertexBufferData, &g_indexBuffer);
	d3d11DevicContext->IASetIndexBuffer(g_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	DWORD indexArr2[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,

		// Back Face
		4,  5,  6,
		4,  6,  7,

		// Top Face
		8,  9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};
	UINT indexNum2 = ARRAYSIZE(indexArr2);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.ByteWidth = sizeof(DWORD) * indexNum2;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	vertexBufferData.pSysMem = indexArr2;
	hr = d3d11Device->CreateBuffer(&bufferDesc, &vertexBufferData, &g_indexBuffer2);
	d3d11DevicContext->IASetIndexBuffer(g_indexBuffer2, DXGI_FORMAT_R32_UINT, 0);

	UINT vertexNum = ARRAYSIZE(vertices);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = sizeof(Vertex) * vertexNum;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	vertexBufferData.pSysMem = vertices;
	hr = d3d11Device->CreateBuffer(&bufferDesc, &vertexBufferData, &g_vertexBuffer);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	d3d11DevicContext->IASetVertexBuffers(0, 1, &g_vertexBuffer, &stride, &offset);
	
	UINT layoutNum = ARRAYSIZE(m_gLayout);
	hr = d3d11Device->CreateInputLayout(m_gLayout, layoutNum, g_vsBuffer->GetBufferPointer(), g_vsBuffer->GetBufferSize(), &g_verInputLayout);
	d3d11DevicContext->IASetInputLayout(g_verInputLayout);

	d3d11DevicContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_VIEWPORT viewPort;
	ZeroMemory(&viewPort, sizeof(D3D11_VIEWPORT));
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = width;
	viewPort.Height = height;
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1.0f;
	d3d11DevicContext->RSSetViewports(1,&viewPort);

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	hr = d3d11Device->CreateBuffer(&bufferDesc, NULL, &g_constantBuffer);
	eyePos = XMVectorSet(0.0f, 3.0f, -8.0f, 0.0);
	XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	//XMMATRIX worldMatrix = XMMatrixIdentity();
	viewMatrix = XMMatrixLookAtLH(eyePos, lookAt, up);
	projectMatrix = XMMatrixPerspectiveFovLH(0.4f * 3.14f, (float)width / height, 0.01f, 1000.0f);
	//WVP = worldMatrix * viewMatrix * projectMatrix;
	D3D11_RASTERIZER_DESC raDesc;
	ZeroMemory(&raDesc, sizeof(D3D11_RASTERIZER_DESC));
	raDesc.FillMode = D3D11_FILL_WIREFRAME;
	raDesc.CullMode = D3D11_CULL_NONE;
	hr = d3d11Device->CreateRasterizerState(&raDesc, &g_wireFrame);
	raDesc.FillMode = D3D11_FILL_SOLID;
	raDesc.CullMode = D3D11_CULL_BACK;
	hr = d3d11Device->CreateRasterizerState(&raDesc, &g_normalFrame);

	//hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"braynzar.jpg", NULL, NULL, &g_cubeTexture, NULL);
	hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"Cage.png", NULL, NULL, &g_cubeTexture, NULL);

	D3D11_SAMPLER_DESC sampleDesc;
	ZeroMemory(&sampleDesc, sizeof(D3D11_SAMPLER_DESC));
	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
	sampleDesc.MipLODBias = 0;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampleDesc.BorderColor[0] = 1.0f;
	sampleDesc.BorderColor[1] = 0.0f;
	sampleDesc.BorderColor[2] = 0.0f;
	sampleDesc.BorderColor[3] = 1.0f;

	hr = d3d11Device->CreateSamplerState(&sampleDesc, &g_cubeTextureSimpleState);
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	D3D11_RENDER_TARGET_BLEND_DESC targetDesc;
	ZeroMemory(&targetDesc, sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
	targetDesc.BlendEnable = true;
	targetDesc.SrcBlend = D3D11_BLEND_SRC_COLOR;
	targetDesc.DestBlend = D3D11_BLEND_INV_SRC1_ALPHA;//D3D11_BLEND_BLEND_FACTOR;
	targetDesc.BlendOp = D3D11_BLEND_OP_ADD;
	targetDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	targetDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	targetDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	targetDesc.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.RenderTarget[0] = targetDesc;
	d3d11Device->CreateBlendState(&blendDesc, &g_Transparency);

	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;

	rsDesc.FrontCounterClockwise = true;
	hr = d3d11Device->CreateRasterizerState(&rsDesc, &g_CCWcullMode);

	rsDesc.FrontCounterClockwise = FALSE;
	hr = d3d11Device->CreateRasterizerState(&rsDesc, &g_CWcullMode);

	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_NONE;
	d3d11Device->CreateRasterizerState(&rsDesc, &g_NoCull);
	return true;
}

void UpdateScene()
{
	//Update the colors of our scene
	red += colormodr * 0.00005f;
	green += colormodg * 0.00002f;
	blue += colormodb * 0.00001f;

	if (red >= 1.0f || red <= 0.0f)
		colormodr *= -1;
	if (green >= 1.0f || green <= 0.0f)
		colormodg *= -1;
	if (blue >= 1.0f || blue <= 0.0f)
		colormodb *= -1;

	rotate += 0.00005;
	if (rotate > 6.24f)
	{
		rotate -= 6.24f;
	}
	if (rotate > 3.14f)
	{
		//d3d11DevicContext->RSSetState(g_wireFrame);
	}
	else
	{
		//d3d11DevicContext->RSSetState(g_normalFrame);
	}
	cube1World = XMMatrixIdentity();
	XMVECTOR rotateAxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	cubeRotation = XMMatrixRotationAxis(rotateAxis, rotate);
	cubeTranslate = XMMatrixTranslation(0.0f, 0.0f, 4.0f);
	cube1World = cubeTranslate * cubeRotation;

	cube2World = XMMatrixIdentity();
	cubeRotation = XMMatrixRotationAxis(rotateAxis, -rotate);
	cubeScale = XMMatrixScaling(1.3f, 1.3f, 1.3f);
	cube2World = cubeRotation * cubeScale;


	
}

void DrawScene()
{
	D3DXCOLOR bgColor(red, green, blue, 1.0f);
	d3d11DevicContext->ClearRenderTargetView(d3d11RenderTargetView, bgColor);
	d3d11DevicContext->ClearDepthStencilView(g_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	float blendFacor[] = { 0.75f,0.75f,0.75,1.0f };
	//d3d11DevicContext->OMSetBlendState(0, 0, 0xFFFFFFFF);
	//Render the Opaque objects
	

	//Set the blend state for transparent objects;
	//d3d11DevicContext->OMSetBlendState(g_Transparency, blendFacor, 0xFFFFFFFF);
	XMVECTOR cubePos = XMVectorZero();
	cubePos = XMVector3TransformCoord(cubePos, cube1World);
	float distant1_X = XMVectorGetX(cubePos) - XMVectorGetX(eyePos);
	float distant1_Y = XMVectorGetY(cubePos) - XMVectorGetY(eyePos);
	float distant1_Z = XMVectorGetZ(cubePos) - XMVectorGetZ(eyePos);
	float distance1 = distant1_X *distant1_X + distant1_Y * distant1_Y + distant1_Z * distant1_Z;

	cubePos = XMVectorZero();
	cubePos = XMVector3TransformCoord(cubePos, cube2World);
	float distant2_X = XMVectorGetX(cubePos) - XMVectorGetX(eyePos);
	float distant2_Y = XMVectorGetY(cubePos) - XMVectorGetY(eyePos);
	float distant2_Z = XMVectorGetZ(cubePos) - XMVectorGetZ(eyePos);
	float distance2 = distant2_X *distant2_X + distant2_Y * distant2_Y + distant2_Z * distant2_Z;
	if (distance1 < distance2)
	{
		XMMATRIX temp = cube1World;
		cube1World = cube2World;
		cube2World = temp;
	}

	d3d11DevicContext->RSSetState(g_NoCull);
	//d3d11DevicContext->Draw(3, 0);
	WVP = cube1World * viewMatrix * projectMatrix;
	cbBuffer.wvp = XMMatrixTranspose(WVP);
	d3d11DevicContext->UpdateSubresource(g_constantBuffer, 0, NULL, &cbBuffer, 0, 0);
	d3d11DevicContext->VSSetConstantBuffers(0, 1, &g_constantBuffer);
	d3d11DevicContext->PSSetShaderResources(0, 1, &g_cubeTexture);
	d3d11DevicContext->PSSetSamplers(0, 1, &g_cubeTextureSimpleState);
	//d3d11DevicContext->RSSetState(g_CCWcullMode);
	//d3d11DevicContext->DrawIndexed(36, 0, 0);
	//d3d11DevicContext->RSSetState(g_CWcullMode);
	d3d11DevicContext->DrawIndexed(36, 0, 0);

	WVP = cube2World * viewMatrix * projectMatrix;
	cbBuffer.wvp = XMMatrixTranspose(WVP);
	d3d11DevicContext->UpdateSubresource(g_constantBuffer, 0, NULL, &cbBuffer, 0, 0);
	d3d11DevicContext->VSSetConstantBuffers(0, 1, &g_constantBuffer);
	d3d11DevicContext->PSSetShaderResources(0, 1, &g_cubeTexture);
	d3d11DevicContext->PSSetSamplers(0, 1, &g_cubeTextureSimpleState);
	//d3d11DevicContext->RSSetState(g_CCWcullMode);
	//d3d11DevicContext->DrawIndexed(36, 0, 0);
	//d3d11DevicContext->RSSetState(g_CWcullMode);
	d3d11DevicContext->DrawIndexed(36, 0, 0);
	//
	RenderText(L"Hello World zyz!");
	swapChain->Present(0, 0);
}

bool InitD2D_D3D101_DWrite(IDXGIAdapter1 * adapter)
{
	HRESULT hr = D3D10CreateDevice1(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_DEBUG | D3D10_CREATE_DEVICE_BGRA_SUPPORT, D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &m_d3d101Device);
	D3D11_TEXTURE2D_DESC sharedTexDesc;
	ZeroMemory(&sharedTexDesc, sizeof(sharedTexDesc));
	sharedTexDesc.Width = width;
	sharedTexDesc.Height = height;
	sharedTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sharedTexDesc.MipLevels = 1;
	sharedTexDesc.ArraySize = 1;
	sharedTexDesc.SampleDesc.Count = 1;
	sharedTexDesc.Usage = D3D11_USAGE_DEFAULT;
	sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	sharedTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
	hr = d3d11Device->CreateTexture2D(&sharedTexDesc, NULL, &m_sharedTex11);
	//Get the keyed mutex for the shared texture (for d3d11)
	hr = m_sharedTex11->QueryInterface(__uuidof(IDXGIKeyedMutex), (void **)&m_keyedMutex11);
	IDXGIResource *sharedResource10;
	HANDLE sharedHandle10;
	hr = m_sharedTex11->QueryInterface(__uuidof(IDXGIResource), (void**)&sharedResource10);
	hr = sharedResource10->GetSharedHandle(&sharedHandle10);
	sharedResource10->Release();
	//Open the surface for the shared texture in d3d10.1;
	IDXGISurface1 *sharedSurface10;
	hr = m_d3d101Device->OpenSharedResource(sharedHandle10, __uuidof(IDXGISurface1), (void **)(&sharedSurface10));
	hr = sharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&m_keyedMutex10);
	//Create D2D factory
	ID2D1Factory *d2dFactory;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&d2dFactory);
	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;
	ZeroMemory(&renderTargetProperties, sizeof(renderTargetProperties));
	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);
	hr = d2dFactory->CreateDxgiSurfaceRenderTarget(sharedSurface10, &renderTargetProperties, &m_d2dRenderTarget);
	sharedSurface10->Release();
	d2dFactory->Release();

	//Create a solid color brush to draw something with
	hr = m_d2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f,1.0f,0.0f,1.0f),&m_Brush);

	//DirectWrite
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_DWriteFactory));

	hr = m_DWriteFactory->CreateTextFormat(L"Lucida Handwriting",
		NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 24.0f, L"en-us", &m_TextFormat);
	hr = m_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	hr = m_TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	m_d3d101Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	return true;
}

void InitD2DScreenTexture()
{
	Vertex v[] =
	{
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
	};

	DWORD indices[] = {
		0,1,2,
		0,2,3
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * ARRAYSIZE(indices);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = indices;
	d3d11Device->CreateBuffer(&indexBufferDesc, &initData, &m_d2dIndexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(v);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;
	d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_d2dVertexBuffer);

	d3d11Device->CreateShaderResourceView(m_sharedTex11, NULL, &m_d2dTexture);
}

void RenderText(std::wstring text)
{
	m_keyedMutex11->ReleaseSync(0);
	m_keyedMutex10->AcquireSync(0, 5);
	m_d2dRenderTarget->BeginDraw();
	m_d2dRenderTarget->Clear(D2D1::ColorF(0.5f, 0.0f, 0.0f, 0.0f));
	std::wostringstream printString;
	printString << text;
	m_printText = printString.str();
	D2D1_COLOR_F fontColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
	m_Brush->SetColor(fontColor);
	D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, width, height);
	m_d2dRenderTarget->DrawText(m_printText.c_str(), wcslen(m_printText.c_str()), m_TextFormat,layoutRect, m_Brush);
	m_d2dRenderTarget->EndDraw();
	m_keyedMutex10->ReleaseSync(1);
	m_keyedMutex11->AcquireSync(1, 5);
	d3d11DevicContext->OMSetBlendState(g_Transparency, NULL, 0xffffffff);
	//set the d2d index buffer;
	d3d11DevicContext->IASetIndexBuffer(m_d2dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	d3d11DevicContext->IASetVertexBuffers(0, 1, &m_d2dVertexBuffer, &stride, &offset);
	WVP = XMMatrixIdentity();
	cbBuffer.wvp = XMMatrixTranspose(WVP);
	d3d11DevicContext->UpdateSubresource(g_constantBuffer, 0, NULL, &cbBuffer, 0, 0);
	d3d11DevicContext->VSSetConstantBuffers(0, 1, &g_constantBuffer);
	d3d11DevicContext->PSSetShaderResources(0, 1, &m_d2dTexture);
	d3d11DevicContext->PSSetSamplers(0, 1, &g_cubeTextureSimpleState);
	d3d11DevicContext->RSSetState(g_CWcullMode);
	d3d11DevicContext->DrawIndexed(6, 0, 0);
}
