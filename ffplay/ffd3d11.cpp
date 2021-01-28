#include "stdafx.h"
#include "ffd3d11.h"
#include "globaldef.h"

#pragma  comment(lib,"d3d11")


class DirectSystem
{
	//私有字段
private:
	//基础接口
	ID3D11Device *m_pDevice;                    //DX设备接口，用于生成各种各样的对象
	ID3D11DeviceContext *m_pDeviceContext;      //DX设备上下文,用于生成设备渲染指令

	//视图区域
	ID3D11RenderTargetView *m_pRenderTargetView;//DX渲染目标视图，字面意思
	ID3D11DepthStencilView *m_pDepthStencilView;//DX深度模板缓存视图
	ID3D11Texture2D        *m_pDepthStencil;    //深度模板缓存

	//图形接口
	IDXGISwapChain *m_pSwapChain;               //DX图形接口，交换链



public:
	DirectSystem()
		:m_pDevice(NULL),
		m_pDeviceContext(NULL),
		m_pRenderTargetView(NULL),
		m_pDepthStencilView(NULL),
		m_pDepthStencil(NULL),
		m_pSwapChain(NULL)
	{

	}

	HRESULT Direct3D_Init(HWND hwnd, int WINDOW_WIDTH, int WINDOW_HEIGHT) {
		//描述交换链
		DXGI_SWAP_CHAIN_DESC scDesc;
		::ZeroMemory(&scDesc, sizeof(scDesc));
		scDesc.OutputWindow = hwnd;     //描述输出窗口
		scDesc.BufferCount = 1;//缓存数量
		scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//描述后台缓存格式
		scDesc.BufferDesc.Width = WINDOW_WIDTH; //描述后台缓存分辨率宽度
		scDesc.BufferDesc.Height = WINDOW_HEIGHT;//描述后台缓存分辨率高度
		scDesc.BufferDesc.RefreshRate.Denominator = 1;//刷新频率的分母
		scDesc.BufferDesc.RefreshRate.Numerator = 60;//刷新频率的分子，这两项表明每秒刷新6次
		scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.Flags = 0;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scDesc.SampleDesc.Count = 1;    //这里不使用多重采样，因此数量设为1
		scDesc.SampleDesc.Quality = 0;  //不使用多重采样，设为0
		scDesc.Windowed = true;         //设置为窗口模式

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,     //D3D11.0 所支持的功能特征
			D3D_FEATURE_LEVEL_10_1,     //D3D10.1 所支持的功能特征
			D3D_FEATURE_LEVEL_10_0      //D3D10.0 所支持的功能特征
		};
		UINT numFeature = ARRAYSIZE(featureLevels);

		HRESULT te;
		//创建设备，设备上下文，交换链
		if (FAILED(te = D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
			featureLevels, numFeature,
			D3D11_SDK_VERSION, &scDesc,
			&m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext)))
		{
			return E_FAIL;
		}

		//获取后台缓冲区
		ID3D11Texture2D *pBack = NULL;
		if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBack))) {
			return E_FAIL;
		}

		//创建渲染目标视图
		HRESULT hr;
		hr = m_pDevice->CreateRenderTargetView(pBack, NULL, &m_pRenderTargetView);

		SAFE_RELEASE(pBack);
		if (FAILED(hr))
		{
			return E_FAIL;
		}

		//描述深度模板缓存
		D3D11_TEXTURE2D_DESC dsDesc;
		::ZeroMemory(&dsDesc, sizeof(dsDesc));
		dsDesc.ArraySize = 1;
		dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dsDesc.CPUAccessFlags = 0;
		dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsDesc.Height = WINDOW_HEIGHT;
		dsDesc.Width = WINDOW_WIDTH;
		dsDesc.MipLevels = 1;
		dsDesc.MiscFlags = 0;
		dsDesc.SampleDesc.Count = 1;
		dsDesc.SampleDesc.Quality = 0;
		dsDesc.Usage = D3D11_USAGE_DEFAULT;

		if (FAILED(m_pDevice->CreateTexture2D(&dsDesc, NULL, &m_pDepthStencil)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pDevice->CreateDepthStencilView(m_pDepthStencil, 0, &m_pDepthStencilView)))
		{
			return E_FAIL;
		}

		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		//第四步，设置视口大小，D3D11默认不会设置视口，此步骤必须手动设置  
		D3D11_VIEWPORT vp;    //创建一个视口的对象
		vp.TopLeftX = 0;      //视口左上角的横坐标
		vp.TopLeftY = 0;      //视口左上角的总坐标
		vp.Width = WINDOW_WIDTH;     //视口的宽
		vp.Height = WINDOW_HEIGHT;   //视口的高
		vp.MinDepth = 0.0f;   //深度值的下限，**由于深度值是[0, 1]所以下限值是0
		vp.MaxDepth = 1.0f;   //深度值的上限，上限值是1

		m_pDeviceContext->RSSetViewports(1, &vp);

		return S_OK;

	}

	void Direct3D_Render()
	{
		if (m_pDevice)
		{
			FLOAT color[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
			m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
			m_pSwapChain->Present(NULL, NULL);
		}

	}

	bool SetUp()
	{
		return true;
	}

	~DirectSystem()
	{
		SAFE_RELEASE(m_pDevice);
		SAFE_RELEASE(m_pDeviceContext);
		SAFE_RELEASE(m_pRenderTargetView);
		SAFE_RELEASE(m_pDepthStencilView);
		SAFE_RELEASE(m_pDepthStencil);
		SAFE_RELEASE(m_pSwapChain);

	}
};

int d3d11Create(FFD3D11 *d3d, HWND hWnd, int width, int height)
{
	if (d3d->_initD3D)
	{
		if (d3d->_imageHeight == height && d3d->_imageWidth == width)
		{
			return 0;
		}

		d3d11Release(d3d);
	}


	d3d->driverType = D3D_DRIVER_TYPE_NULL;               
	d3d->featureLevel = D3D_FEATURE_LEVEL_11_0;           

	HRESULT hResult = S_OK;

	d3d->m_hWnd = hWnd;

	d3d->_imageWidth = width;
	d3d->_imageHeight  = height;

	RECT rect;
	GetWindowRect(d3d->m_hWnd, &rect);

	
	d3d->rcDisplay.left = d3d->rcDisplay.top = 0;
	d3d->rcDisplay.right = rect.right - rect.left;
	d3d->rcDisplay.bottom = rect.bottom - rect.top;

	int rcWidth = d3d->rcDisplay.right - d3d->rcDisplay.left;
	int rcHeight = d3d->rcDisplay.bottom - d3d->rcDisplay.top;


	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	d3d->m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));//填充
	sd.BufferCount = 1;                              //我们只创建一个后缓冲（双缓冲）因此为1
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = d3d->m_Format;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = d3d->m_hWnd;
	sd.SampleDesc.Count = 1;                      //1重采样
	sd.SampleDesc.Quality = 0;                      //采样等级
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;      //常用参数
	sd.Windowed = TRUE;                              //是否全屏

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
	{
		d3d->driverType = driverTypes[driverTypeIndex];
		hResult = D3D11CreateDeviceAndSwapChain(
			NULL,                                //默认图形适配器
			d3d->driverType,                        //驱动类型
			NULL,                                //实现软件渲染设备的动态库句柄，如果使用的驱动设备类型是软件设备则不能为NULL
			createDeviceFlags,                    //创建标志，0用于游戏发布，一般D3D11_CREATE_DEVICE_DEBUG允许我们创建可供调试的设备，在开发中比较有用
			featureLevels,                        //特征等级
			numFeatureLevels,                    //特征等级数量
			D3D11_SDK_VERSION,                    //sdk版本号
			&sd,
			&d3d->pSwapChain,
			&d3d->pd3dDevice,
			&d3d->featureLevel,
			&d3d->pImmediateContext
			);
		if (SUCCEEDED(hResult))
			break;
	}
	if (FAILED(hResult))
		return -2;

	ID3D11Texture2D *pBackBuffer = NULL;
	
	hResult = d3d->pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hResult))
		return -3;

	hResult = d3d->pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &d3d->pRenderTargetView);
	SAFE_RELEASE(pBackBuffer);
	if (FAILED(hResult))
		return -4;


	//描述深度模板缓存
	D3D11_TEXTURE2D_DESC dsDesc;
	::ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.ArraySize = 1;
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsDesc.CPUAccessFlags = 0;
	dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsDesc.Height = height;
	dsDesc.Width = width;
	dsDesc.MipLevels = 1;
	dsDesc.MiscFlags = 0;
	dsDesc.SampleDesc.Count = 1;
	dsDesc.SampleDesc.Quality = 0;
	dsDesc.Usage = D3D11_USAGE_DEFAULT;

	if (FAILED(d3d->pd3dDevice->CreateTexture2D(&dsDesc, NULL, &d3d->m_pDepthStencil)))
	{
		return E_FAIL;
	}

	if (FAILED(d3d->pd3dDevice->CreateDepthStencilView(d3d->m_pDepthStencil, 0, &d3d->m_pDepthStencilView)))
	{
		return E_FAIL;
	}


	d3d->pImmediateContext->OMSetRenderTargets(1, &d3d->pRenderTargetView, d3d->m_pDepthStencilView);

	D3D11_VIEWPORT vp;
	vp.Height = (FLOAT)rcHeight;
	vp.Width = (FLOAT)rcWidth;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	d3d->pImmediateContext->RSSetViewports(1, &vp);

	d3d->pixel = (unsigned char*)malloc(d3d->_imageWidth*d3d->_imageHeight * 4);


	d3d->_initD3D = TRUE;

	return S_OK;
}


int d3d11LockSurface(FFD3D11 *d3d, void **data, int *pitch)
{
	if (NULL != d3d->pixel)
	{
		*data = d3d->pixel;
		pitch[0] = d3d->_imageWidth * 4;
	}
	else
	{
		return -1;
	}
	
	return 0;
}

void d3d11UnlockSurface(FFD3D11 *d3d)
{
	return;
}


void d3d11RenderInternal(FFD3D11 *d3d)
{
	if (!d3d->_initD3D)
	{
		return;
	}

	RECT rcDisplay;
	GetWindowRect(d3d->m_hWnd, &rcDisplay);


	float ClearColor[4] = { 0.5f, 0.1f, 0.2f, 1.0f };
	d3d->pImmediateContext->ClearRenderTargetView(d3d->pRenderTargetView, ClearColor);

	ID3D11Texture2D* pBackBuffer;
	d3d->pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	d3d->pImmediateContext->UpdateSubresource(pBackBuffer, 0, NULL, d3d->pixel, d3d->_imageWidth * 4, 0);

	d3d->pSwapChain->Present(0, 0);
}

void d3d11Render(FFD3D11 *d3d, int width, int height, HWND hWnd)
{
	if (hWnd == NULL || !IsWindowVisible(hWnd))
	{
		return;
	}

	if (!d3d->_initD3D)
	{
		d3d->_initD3D = d3d11Create(d3d, hWnd, width, height);
		if (!d3d->_initD3D)
		{
			return;
		}
	}


	if (d3d->_imageWidth != width || d3d->_imageHeight != height)
	{
		d3d->_initD3D = FALSE;
		d3d->_initD3D = d3d11Create(d3d, hWnd, width, height);
		if (!d3d->_initD3D)
		{
			return;
		}
	}


	RECT rcDisplay;
	GetWindowRect(d3d->m_hWnd, &rcDisplay);


	if (d3d->rcDisplay.right - d3d->rcDisplay.left != rcDisplay.right - rcDisplay.left
		|| d3d->rcDisplay.bottom - d3d->rcDisplay.top != rcDisplay.bottom - rcDisplay.top)
	{
		//d3d->pSwapChain->ResizeBuffers(0, d3d->rcDisplay.right - d3d->rcDisplay.left, d3d->rcDisplay.bottom - d3d->rcDisplay.top, DXGI_FORMAT_UNKNOWN, 0);

		d3d->_initD3D = FALSE;
		return ;
	}


	float ClearColor[4] = { 0.5f, 0.1f, 0.2f, 1.0f }; 
	d3d->pImmediateContext->ClearRenderTargetView(d3d->pRenderTargetView, ClearColor);

	d3d->pImmediateContext->ClearDepthStencilView(d3d->m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);



	ID3D11Texture2D* pBackBuffer;
	d3d->pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),(LPVOID*)&pBackBuffer);

	d3d->pImmediateContext->UpdateSubresource(pBackBuffer, 0, NULL, d3d->pixel, d3d->_imageWidth * 4, 0);

	d3d->pSwapChain->Present(0, 0);
}

void d3d11Release(FFD3D11 *d3d)
{
	if (d3d->pImmediateContext)
		d3d->pImmediateContext->ClearState();

	SAFE_RELEASE(d3d->pSwapChain);

	SAFE_RELEASE(d3d->pRenderTargetView);

	SAFE_RELEASE(d3d->pImmediateContext);

	SAFE_RELEASE(d3d->pd3dDevice);

	if (NULL != d3d->pixel)
	{
		free(d3d->pixel);
		d3d->pixel = NULL;
	}
	
	d3d->_initD3D = FALSE;
}