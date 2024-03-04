#include <assert.h>
#include <d3dcompiler.h>

#include "renderer.h"

HRESULT Renderer::setupBackBuffer() {
  ID3D11Texture2D* pBackBuffer = NULL;
  HRESULT hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
  assert(SUCCEEDED(hr));
  if (SUCCEEDED(hr)) {
    hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pBackBufferRTV);
    assert(SUCCEEDED(hr));

    SAFE_RELEASE(pBackBuffer);
  }
  return hr;
}

HRESULT Renderer::initScene() {
  HRESULT hr = S_OK;
  
  static const Vertex Vertices[] = {
         {-0.5, -0.5,  0.5, 0, 1},
        { 0.5, -0.5,  0.5, 1, 1},
        { 0.5, -0.5, -0.5, 1, 0},
        {-0.5, -0.5, -0.5, 0, 0},

        {-0.5,  0.5, -0.5, 0, 1},
        { 0.5,  0.5, -0.5, 1, 1},
        { 0.5,  0.5,  0.5, 1, 0},
        {-0.5,  0.5,  0.5, 0, 0},

        { 0.5, -0.5, -0.5, 0, 1},
        { 0.5, -0.5,  0.5, 1, 1},
        { 0.5,  0.5,  0.5, 1, 0},
        { 0.5,  0.5, -0.5, 0, 0},

        {-0.5, -0.5,  0.5, 0, 1},
        {-0.5, -0.5, -0.5, 1, 1},
        {-0.5,  0.5, -0.5, 1, 0},
        {-0.5,  0.5,  0.5, 0, 0},

        { 0.5, -0.5,  0.5, 0, 1},
        {-0.5, -0.5,  0.5, 1, 1},
        {-0.5,  0.5,  0.5, 1, 0},
        { 0.5,  0.5,  0.5, 0, 0},

        {-0.5, -0.5, -0.5, 0, 1},
        { 0.5, -0.5, -0.5, 1, 1},
        { 0.5,  0.5, -0.5, 1, 0},
        {-0.5,  0.5, -0.5, 0, 0}
  };
  
  static const USHORT Indices[] = {
        0, 2, 1, 0, 3, 2,
        4, 6, 5, 4, 7, 6,
        8, 10, 9, 8, 11, 10,
        12, 14, 13, 12, 15, 14,
        16, 18, 17, 16, 19, 18,
        20, 22, 21, 20, 23, 22
  };

  static const D3D11_INPUT_ELEMENT_DESC InputDesc[] = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0} };


  if (SUCCEEDED(hr)) {
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(Vertices);
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &Vertices;
    data.SysMemPitch = sizeof(Vertices);
    data.SysMemSlicePitch = 0;

    hr = m_pDevice->CreateBuffer(&desc, &data, &m_pVertexBuffer);
    assert(SUCCEEDED(hr));
  }

  if (SUCCEEDED(hr)) {
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(Indices);
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &Indices;
    data.SysMemPitch = sizeof(Indices);
    data.SysMemSlicePitch = 0;

    hr = m_pDevice->CreateBuffer(&desc, &data, &m_pIndexBuffer);
    assert(SUCCEEDED(hr));
  }

  ID3D10Blob* vShaderBuffer = nullptr;
  ID3D10Blob* pShaderBuffer = nullptr;
  
  int flags = 0;
  #ifdef _DEBUG
    flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
  #endif
  
  if (SUCCEEDED(hr)) {
    hr = D3DCompileFromFile(L"vShader.hlsl", NULL, NULL, "main", "vs_5_0", flags, 0, &vShaderBuffer, NULL);
    hr = m_pDevice->CreateVertexShader(vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader);
  }
  if (SUCCEEDED(hr)) {
    hr = D3DCompileFromFile(L"pShader.hlsl", NULL, NULL, "main", "ps_5_0", flags, 0, &pShaderBuffer, NULL);
    hr = m_pDevice->CreatePixelShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader);
  }
  if (SUCCEEDED(hr)) {
    int numElements = sizeof(InputDesc) / sizeof(InputDesc[0]);
    hr = m_pDevice->CreateInputLayout(InputDesc, numElements, vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), &m_pInputLayout);
  }

  SAFE_RELEASE(vShaderBuffer);
  SAFE_RELEASE(pShaderBuffer);

  if (SUCCEEDED(hr)) {
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(WorldMatrixBuffer);
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    WorldMatrixBuffer worldMatrixBuffer;
    worldMatrixBuffer.mWorldMatrix = DirectX::XMMatrixIdentity();

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &worldMatrixBuffer;
    data.SysMemPitch = sizeof(worldMatrixBuffer);
    data.SysMemSlicePitch = 0;

    hr = m_pDevice->CreateBuffer(&desc, &data, &m_pWorldMatrixBuffer);
    assert(SUCCEEDED(hr));
  }
  if (SUCCEEDED(hr)) {
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(SceneMatrixBuffer);
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    hr = m_pDevice->CreateBuffer(&desc, nullptr, &m_pSceneMatrixBuffer);
    assert(SUCCEEDED(hr));
  }

  if (SUCCEEDED(hr)) {
    D3D11_RASTERIZER_DESC desc = {};
    desc.AntialiasedLineEnable = false;
    desc.FillMode = D3D11_FILL_SOLID;
    desc.CullMode = D3D11_CULL_BACK;
    desc.DepthBias = 0;
    desc.DepthBiasClamp = 0.0f;
    desc.FrontCounterClockwise = false;
    desc.DepthClipEnable = true;
    desc.ScissorEnable = false;
    desc.MultisampleEnable = false;
    desc.SlopeScaledDepthBias = 0.0f;

    hr = m_pDevice->CreateRasterizerState(&desc, &m_pRasterizerState);
    assert(SUCCEEDED(hr));
  }

  try {
    m_textureArray.emplace_back(m_pDevice, m_pDeviceContext, L"heart.dds");
  }
  catch (...) {
    return E_FAIL;
  }

  if (SUCCEEDED(hr)) {
    D3D11_SAMPLER_DESC desc = {};

    desc.Filter = D3D11_FILTER_ANISOTROPIC;
    desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.MinLOD = -D3D11_FLOAT32_MAX;
    desc.MaxLOD = D3D11_FLOAT32_MAX;
    desc.MipLODBias = 0.0f;
    desc.MaxAnisotropy = 16;
    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 1.0f;

    hr = m_pDevice->CreateSamplerState(&desc, &m_pSampler);
    assert(SUCCEEDED(hr));
  }

  return hr;
}

bool Renderer::deviceInit(HINSTANCE hinst, HWND hWnd, Camera* pCamera, Input* pInput) {
  m_pCamera = pCamera;
  m_pInput = pInput;

  HRESULT hr;

  IDXGIFactory* pFactory = nullptr;
  hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);

  //select adapter
  IDXGIAdapter* pSelectedAdapter = NULL;
  if (SUCCEEDED(hr)) {
    IDXGIAdapter* pAdapter = NULL;
    UINT adapterIndex = 0;
    while (SUCCEEDED(pFactory->EnumAdapters(adapterIndex, &pAdapter))) {
      DXGI_ADAPTER_DESC desc;
      pAdapter->GetDesc(&desc);
      if (wcscmp(desc.Description, L"Microsoft Basic Render Driver") != 0) {
        pSelectedAdapter = pAdapter;
        break;
      }
      pAdapter->Release();
      adapterIndex++;
    }
  }
  assert(pSelectedAdapter != NULL);

  //create DirectX 11 device
  D3D_FEATURE_LEVEL level;
  D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
  if (SUCCEEDED(hr)) {
    UINT flags = 0;
#ifdef _DEBUG
      flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    hr = D3D11CreateDevice(pSelectedAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL,
      flags, levels, 1, D3D11_SDK_VERSION, &m_pDevice, &level, &m_pDeviceContext);
    assert(level == D3D_FEATURE_LEVEL_11_0);
    assert(SUCCEEDED(hr));
  }
  //create swapchain
  if (SUCCEEDED(hr)) {
    RECT rc;
    GetClientRect(hWnd, &rc);
    m_width = rc.right - rc.left;
    m_height = rc.bottom - rc.top;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferDesc.Width = m_width;
    swapChainDesc.BufferDesc.Height = m_height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = true;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    hr = pFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
    assert(SUCCEEDED(hr));
  }

  if (SUCCEEDED(hr)) {
    hr = setupBackBuffer();
  }
  
  if (SUCCEEDED(hr)) {
    hr = initScene();
  }

  SAFE_RELEASE(pSelectedAdapter);
  SAFE_RELEASE(pFactory);
  
  if (SUCCEEDED(hr)) {
    if(!m_pCamera)
      hr = S_FALSE;
  }

  if (SUCCEEDED(hr)) {
    if(!m_pInput)
      hr = S_FALSE;
  }
  if (SUCCEEDED(hr)) {
    if(!m_pCubeMap)
      hr = S_FALSE;
  }

  if (FAILED(hr)) {
    deviceCleanup();
  }
  m_pCubeMap = new CubeMap(m_pDevice, m_pDeviceContext, m_width, m_height, 10, 10);
  return SUCCEEDED(hr);
}

void Renderer::inputMovement() {
  XMFLOAT3 mouseMove = m_pInput->getMouseState();
  m_pCamera->getMouseState(mouseMove.x, mouseMove.y, mouseMove.z);
}

bool Renderer::getState() {
  HRESULT hr = S_OK;
  m_pCamera->getState();
  m_pInput->getState();

  inputMovement();

  static float t = 0.0f;
  static ULONGLONG timeStart = 0;
  ULONGLONG timeCur = GetTickCount64();
  if (timeStart == 0) {
    timeStart = timeCur;
  }
  t = (timeCur - timeStart) / 1000.0f;

  WorldMatrixBuffer wmb;
  wmb.mWorldMatrix = XMMatrixRotationY(t);
  m_pDeviceContext->UpdateSubresource(m_pWorldMatrixBuffer, 0, nullptr, &wmb, 0, 0);

  XMMATRIX mView;
  m_pCamera->getView(mView);

  XMMATRIX mProjection = XMMatrixPerspectiveFovLH(
      XM_PIDIV2, 
      m_width / (FLOAT)m_height, 
      0.01f, 100.0f);

  D3D11_MAPPED_SUBRESOURCE subresource;
  hr = m_pDeviceContext->Map(m_pSceneMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
  assert(SUCCEEDED(hr));
  if (SUCCEEDED(hr)) {
    SceneMatrixBuffer& sceneBuffer = *reinterpret_cast<SceneMatrixBuffer*>(subresource.pData);
    sceneBuffer.mViewProjectionMatrix = XMMatrixMultiply(mView, mProjection);
    m_pDeviceContext->Unmap(m_pSceneMatrixBuffer, 0);
  }

  m_pCubeMap->getState(m_pDeviceContext, mView, mProjection, m_pCamera->getPosition());

  return SUCCEEDED(hr);
}

bool Renderer::render() {
  m_pDeviceContext->ClearState();

  ID3D11RenderTargetView* views[] = { m_pBackBufferRTV };
  m_pDeviceContext->OMSetRenderTargets(1, views, nullptr);
  static const FLOAT BackColor[4] = { 0.75f, 0.25f, 0.25f, 1.0f };
  m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, BackColor);

  D3D11_VIEWPORT vp;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  vp.Width = (FLOAT)m_width;
  vp.Height = (FLOAT)m_height;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;

  m_pDeviceContext->RSSetViewports(1, &vp);

  D3D11_RECT rect;
  rect.left = 0;
  rect.right = m_width;
  rect.top = 0;
  rect.bottom = m_height;

  m_pDeviceContext->RSSetScissorRects(1, &rect);

  m_pCubeMap->render(m_pDeviceContext);

  m_pDeviceContext->RSSetState(m_pRasterizerState);

  ID3D11SamplerState* samplers[] = { m_pSampler };
  m_pDeviceContext->PSSetSamplers(0, 1, samplers);

  ID3D11ShaderResourceView* resources[] = { m_textureArray[0].GetTexture() };
  m_pDeviceContext->PSSetShaderResources(0, 1, resources);

  m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

  ID3D11Buffer* vBuffer[] = {m_pVertexBuffer};
  UINT strides[] = {20};
  UINT offsets[] = {0};

  m_pDeviceContext->IASetVertexBuffers(0, 1, vBuffer, strides, offsets);
  m_pDeviceContext->IASetInputLayout(m_pInputLayout);
  m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
  m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pWorldMatrixBuffer);
  m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pSceneMatrixBuffer);
  m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
  m_pDeviceContext->DrawIndexed(36, 0, 0);

  HRESULT hr = m_pSwapChain->Present(1, 0);
  assert(SUCCEEDED(hr));

  return SUCCEEDED(hr);
}

void Renderer::deviceCleanup() {
  SAFE_RELEASE(m_pBackBufferRTV);
  SAFE_RELEASE(m_pSwapChain);
  SAFE_RELEASE(m_pDeviceContext);

  SAFE_RELEASE(m_pIndexBuffer);
  SAFE_RELEASE(m_pVertexBuffer);
  SAFE_RELEASE(m_pVertexShader);
  SAFE_RELEASE(m_pPixelShader);

  SAFE_RELEASE(m_pInputLayout);
  SAFE_RELEASE(m_pDevice);

  SAFE_RELEASE(m_pRasterizerState);
  SAFE_RELEASE(m_pWorldMatrixBuffer);
  SAFE_RELEASE(m_pSceneMatrixBuffer);
  SAFE_RELEASE(m_pSampler);

  for (auto t : m_textureArray) {
    t.Release();
  }

  m_textureArray.clear();
  delete m_pCubeMap;
}

bool Renderer::winResize(UINT width, UINT height) {
  if (width != m_width || height != m_height) {
    SAFE_RELEASE(m_pBackBufferRTV);

    HRESULT hr = m_pSwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    assert(SUCCEEDED(hr));
    if (SUCCEEDED(hr)) {
      m_width = width;
      m_height = height;

      hr = setupBackBuffer();
      m_pInput->resize(width, height);
      m_pCubeMap->resize(width, height);
    }
    return SUCCEEDED(hr);
  }
  return true;
}
