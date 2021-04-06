#include "stdafx.h"
#include "ffd3d11.h"
#include "globaldef.h"

#pragma  comment(lib,"d3d11")

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
	GetClientRect(d3d->m_hWnd, &rect);

	
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
	ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));//���
	sd.BufferCount = 1;                              //����ֻ����һ���󻺳壨˫���壩���Ϊ1
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = d3d->m_Format;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = d3d->m_hWnd;
	sd.SampleDesc.Count = 1;                      //1�ز���
	sd.SampleDesc.Quality = 0;                      //�����ȼ�
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;      //���ò���
	sd.Windowed = TRUE;                              //�Ƿ�ȫ��

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
	{
		d3d->driverType = driverTypes[driverTypeIndex];
		hResult = D3D11CreateDeviceAndSwapChain(
			NULL,                                //Ĭ��ͼ��������
			d3d->driverType,                        //��������
			NULL,                                //ʵ�������Ⱦ�豸�Ķ�̬���������ʹ�õ������豸����������豸����ΪNULL
			createDeviceFlags,                    //������־��0������Ϸ������һ��D3D11_CREATE_DEVICE_DEBUG�������Ǵ����ɹ����Ե��豸���ڿ����бȽ�����
			featureLevels,                        //�����ȼ�
			numFeatureLevels,                    //�����ȼ�����
			D3D11_SDK_VERSION,                    //sdk�汾��
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


	//�������ģ�建��
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
	GetClientRect(d3d->m_hWnd, &rcDisplay);

#if 0


	if (d3d->rcDisplay.right - d3d->rcDisplay.left != rcDisplay.right - rcDisplay.left
		|| d3d->rcDisplay.bottom - d3d->rcDisplay.top != rcDisplay.bottom - rcDisplay.top)
	{
		//d3d->pSwapChain->ResizeBuffers(0, d3d->rcDisplay.right - d3d->rcDisplay.left, d3d->rcDisplay.bottom - d3d->rcDisplay.top, DXGI_FORMAT_UNKNOWN, 0);

		d3d->_initD3D = FALSE;
		d3d->_initD3D = d3d11Create(d3d, hWnd, width, height);
		if (!d3d->_initD3D)
		{
			return;
		}
		return ;
	}
#endif

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