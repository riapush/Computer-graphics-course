#pragma once

#include <d3d11.h>
#include <dxgi.h>

struct Vertex {
	float x, y, z;
	COLORREF color;
};


class Renderer {
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
	IDXGISwapChain* m_pSwapChain = nullptr;
	ID3D11RenderTargetView* m_pBackBufferRTV = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	UINT m_width = 0;
	UINT m_height = 0;

	HRESULT setupBackBuffer();
	HRESULT initScene();

public:
	bool deviceInit(HWND hWnd);
	bool render();
	void deviceCleanup();
	bool winResize(UINT width, UINT height);
};