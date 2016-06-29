#include "Application.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  PAINTSTRUCT ps;
  HDC hdc;

  switch (message) {
    case WM_PAINT:
      hdc = BeginPaint(hWnd, &ps);
      EndPaint(hWnd, &ps);
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }

  return 0;
}

bool Application::handleKeyboard(MSG msg) {
  switch (msg.wParam) {
    case 0x57: // W
    case VK_UP:
      camera->move(cameraSpeed * 0.2f);
      return true;
    case 0x53: // S
    case VK_DOWN:
      camera->move(cameraSpeed * -0.2f);
      return true;
    case 0x44: // D
    case VK_RIGHT:
      camera->strafe(cameraSpeed * 0.2f);
      return true;
    case 0x41: // A
    case VK_LEFT:
      camera->strafe(cameraSpeed * -0.2f);
      return true;
    case VK_SHIFT:
      return true;
    case VK_CONTROL:
      return true;
    case VK_END:
      return true;
    case VK_SPACE:
      camera->moveY(cameraSpeed * 0.2f);
      return true;
    case VK_TAB:
      return true;
  }

  return false;
}

void Application::handleMouseClick(WPARAM buttonStates, int x, int y) {
  if ((buttonStates & MK_LBUTTON) != 0) {
    lastMousePosX = x;
    lastMousePosY = y;
  }
}

void Application::handleMouseMovement(WPARAM buttonStates, int x, int y) {
  if ((buttonStates & MK_LBUTTON) != 0) {
    float xRotation = XMConvertToRadians(x - lastMousePosX);
    float yRotation = XMConvertToRadians(y - lastMousePosY);

    camera->rotatePitch(yRotation);
    camera->rotateYaw(xRotation);

    lastMousePosX = x;
    lastMousePosY = y;
  }
}

HRESULT Application::loadTest(xml_node<>* testNode, Scenario scenario) {
  if (cloth) {
    double timeSpentOnInternalForce = cloth->getTimeSpentCalculatingInternalForce();
    double timeSpentIntegrating = cloth->getTimeSpentIntegrating();

    switch (scenario) {
      case SHEET:
        // now loading a sheet simulation, therefore the previous sim was a flag, so save to the flag file
        flagDataFile << testId << ", " << (timeSpentOnInternalForce / (double) frameCount) << ", " << (timeSpentIntegrating / (double) frameCount);
        flagDataFile << ", " << (averageUpdateTime / (double) frameCount) << ", " << (averageRenderTime / (double) frameCount) << ", " << (averageFPS / (double) numTimeFPSCalculated) << endl;
        break;
      case FLAG:
        // now loading a flag simulation, therefore the previous sim was a sheet, so save to the sheet file
        sheetDataFile << testId << ", " << (timeSpentOnInternalForce / (double) frameCount) << ", " << (timeSpentIntegrating / (double) frameCount);
        sheetDataFile << ", " << (averageUpdateTime / (double) frameCount) << ", " << (averageRenderTime / (double) frameCount) << ", " << (averageFPS / (double) numTimeFPSCalculated) << endl;
        break;
    }
  }

  delete cloth;
  delete[] vertices;
  vertices = nullptr;
  if (vertexBuffer) vertexBuffer->Release();
  if (indexBuffer) indexBuffer->Release();
  frameCount = 0;

  testId = convertStringToNumber<int>(testNode->first_attribute("id")->value());

  cloth = new Cloth(testNode->first_node("cloth_params"));

  HRESULT hr = initVertexBuffer();

  if (FAILED(hr)) {
    return hr;
  }

  hr = initIndexBuffer();

  if (FAILED(hr)) {
    return hr;
  }

  switch (scenario) {
    case SHEET:
      cloth->setPinned(0, 0, true);
      cloth->setPinned(0, cloth->getNumColumns() - 1, true);
      break;
    case FLAG:
      for (int i = 0; i < cloth->getNumColumns(); i++) {
        cloth->setPinned(i, 0, true);
      }
      break;
  }

  xml_node<>* integratorNode = testNode->first_node("integrator");
  IIntegrator* integrator = nullptr;

  switch (convertStringToNumber<int>(integratorNode->first_attribute("type")->value())) {
    case EXPLICIT_EULER:
      integrator = ExplicitEulerIntegrator::getInstance();
      break;
    case VERLET:
      integrator = VerletIntegrator::getInstance();
      break;
    case FOURTH_ORDER_RUNGE_KUTTA:
      break;
    case IMPLICIT_EULER:
      break;
    default:
      return -1;
  }

  integrator->setTimeStep(convertStringToNumber<double>(integratorNode->first_attribute("time_step")->value()));
  cloth->setIntegrator(integrator);

  return S_OK;
}

Application::Application() : lastMousePosX(0.0f), lastMousePosY(0.0f), cloth(nullptr) {
  hInst = nullptr;
  hWnd = nullptr;
  driverType = D3D_DRIVER_TYPE_NULL;
  featureLevel = D3D_FEATURE_LEVEL_11_0;
  d3dDevice = nullptr;
  immediateContext = nullptr;
  swapChain = nullptr;
  renderTargetView = nullptr;
  vertexShader = nullptr;
  vertexLayout = nullptr;
  vertexBuffer = nullptr;
  indexBuffer = nullptr;
  constantBuffer = nullptr;
}

Application::~Application() {
  cleanup();

  double timeSpentOnInternalForce = cloth->getTimeSpentCalculatingInternalForce();
  double timeSpentIntegrating = cloth->getTimeSpentIntegrating();
  flagDataFile << testId << ", " << (timeSpentOnInternalForce / (double) frameCount) << ", " << (timeSpentIntegrating / (double) frameCount);
  flagDataFile << ", " << (averageUpdateTime / (double) frameCount) << ", " << (averageRenderTime / (double) frameCount) << ", " << (averageFPS / (double) numTimeFPSCalculated) << endl;

  delete cloth;
}

HRESULT Application::initialise(HINSTANCE hInstance, int nCmdShow) {
  if (FAILED(initWindow(hInstance, nCmdShow))) {
    return E_FAIL;
  }

  AllocConsole();
  HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);
  FILE *COutputHandle = _fdopen(SystemOutput, "w");
  *stdout = *COutputHandle;
  setvbuf(stdout, NULL, _IONBF, 0);

  RECT rc;
  GetClientRect(hWnd, &rc);
  windowWidth = rc.right - rc.left;
  windowHeight = rc.bottom - rc.top;

  //cloth = new Cloth(XMVectorSet(-10.0f, 10.0f, 10.0f, 0.0f), 10.0f, 10.0f, 50, 50, 100.0f, 200.0f, 200.0f, 200.0f, 200.0f, 1.0f, 1.0f, 0.01f);
  //cloth->setPinned(0, 0, true);
  //cloth->setPinned(0, 49, true);
  //for (int i = 0; i < 50; i++) {
  //  cloth->setPinned(i, 0, true);
  //}

  a = Particle(.04f, 0.0f, XMVectorSet(-10.0f, 10.0f, 10.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 01.0f, 0.0f));
  b = Particle(.04f, 0.0f, XMVectorSet(-9.0f, 10.0f, 10.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 01.0f, 0.0f));
  a.setPinned(true);
  spring = Spring(20.0f, 2.0f, &a, &b);

  if (FAILED(initDevice())) {
    cleanup();

    return E_FAIL;
  }

  // Setup Camera
  XMFLOAT3 eye = XMFLOAT3(0.0f, 2.0f, -10.0f);
  XMFLOAT3 at = XMFLOAT3(0.0f, 0.0f, 1.0f);
  XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

  camera = new Camera(true, true, eye, at, up, (float) renderWidth, (float) renderHeight, 1.0f, 1000.0f);

  return S_OK;
}

HRESULT Application::initShadersAndInputLayout() {
  HRESULT hr;

  // Compile the vertex shader
  ID3DBlob* pVSBlob = nullptr;
  hr = compileShaderFromFile(L"Shaders.fx", "VS", "vs_4_0", &pVSBlob);

  if (FAILED(hr)) {
    MessageBox(nullptr,
      "The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", "Error", MB_OK);
    return hr;
  }

  // Create the vertex shader
  hr = d3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &vertexShader);

  if (FAILED(hr)) {
    pVSBlob->Release();
    return hr;
  }

  // Compile the pixel shader
  ID3DBlob* pPSBlob = nullptr;
  hr = compileShaderFromFile(L"Shaders.fx", "PS", "ps_4_0", &pPSBlob);

  if (FAILED(hr)) {
    MessageBox(nullptr,
      "The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", "Error", MB_OK);
    return hr;
  }

  // Create the pixel shader
  hr = d3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pixelShader);
  pPSBlob->Release();

  if (FAILED(hr)) {
    return hr;
  }

  // Define the input layout
  D3D11_INPUT_ELEMENT_DESC layout[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
  };

  UINT numElements = ARRAYSIZE(layout);

  // Create the input layout
  hr = d3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &vertexLayout);
  pVSBlob->Release();

  if (FAILED(hr)) {
    return hr;
  }

  return hr;
}

HRESULT Application::initVertexBuffer() {
  HRESULT hr;

  if (cloth) {
    // Create vertex buffer
    vertices = new SimpleVertex[cloth->getNumRows() * cloth->getNumColumns()];
    const Particle* const particles = cloth->getParticles();

    for (int i = 0; i < cloth->getNumRows(); i++) {
      for (int j = 0; j < cloth->getNumColumns(); j++) {
        XMFLOAT3& posL = vertices[(i * cloth->getNumColumns()) + j].posL;
        XMVECTOR pos = particles[(i * cloth->getNumColumns()) + j].getPosition();
        posL.x = pos.m128_f32[0];
        posL.y = pos.m128_f32[1];
        posL.z = pos.m128_f32[2];
      }
    }

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(SimpleVertex) * cloth->getNumRows() * cloth->getNumColumns();
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    //vertices = new SimpleVertex[2];
    //vertices[0].posL.x = a.getPosition().m128_f32[0];
    //vertices[0].posL.y = a.getPosition().m128_f32[1];
    //vertices[0].posL.z = a.getPosition().m128_f32[2];
    //vertices[1].posL.x = b.getPosition().m128_f32[0];
    //vertices[1].posL.y = b.getPosition().m128_f32[1];
    //vertices[1].posL.z = b.getPosition().m128_f32[2];

    //D3D11_BUFFER_DESC bd;
    //ZeroMemory(&bd, sizeof(bd));
    //bd.Usage = D3D11_USAGE_DYNAMIC;
    //bd.ByteWidth = sizeof(SimpleVertex) * 2;
    //bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    //bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;

    hr = d3dDevice->CreateBuffer(&bd, &InitData, &vertexBuffer);

    if (FAILED(hr)) {
      return hr;
    }

    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
  }

  return S_OK;
}

HRESULT Application::initIndexBuffer() {
  HRESULT hr;

  if (cloth) {
    // Create index buffer
    UINT* indices = new UINT[(cloth->getNumStructuralSprings() + cloth->getNumShearSprings()) * 2];
    int currentIndex = 0;
    for (int i = 0; i < cloth->getNumRows(); i++) {
      for (int j = 0; j < cloth->getNumColumns(); j++) {
        if (i < cloth->getNumRows() - 1 && j < cloth->getNumColumns() - 1) {
          indices[currentIndex++] = (i * cloth->getNumColumns()) + j;
          indices[currentIndex++] = (i * cloth->getNumColumns()) + j + 1;

          indices[currentIndex++] = (i * cloth->getNumColumns()) + j;
          indices[currentIndex++] = ((i + 1) * cloth->getNumColumns()) + j;

          indices[currentIndex++] = (i * cloth->getNumColumns()) + j;
          indices[currentIndex++] = ((i + 1) * cloth->getNumColumns()) + j + 1;

          indices[currentIndex++] = (i * cloth->getNumColumns()) + j + 1;
          indices[currentIndex++] = ((i + 1) * cloth->getNumColumns()) + j;
        }
        else if (i == cloth->getNumRows() - 1 && j < cloth->getNumColumns() - 1) {
          indices[currentIndex++] = (i * cloth->getNumColumns()) + j;
          indices[currentIndex++] = (i * cloth->getNumColumns()) + j + 1;
        }
        else if (j == cloth->getNumColumns() - 1 && i < cloth->getNumRows() - 1) {
          indices[currentIndex++] = (i * cloth->getNumColumns()) + j;
          indices[currentIndex++] = ((i + 1) * cloth->getNumColumns()) + j;
        }
      }
    }

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(UINT) * (cloth->getNumStructuralSprings() + cloth->getNumShearSprings()) * 2;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    //UINT* indices = new UINT[2];
    //indices[0] = 0;
    //indices[1] = 1;

    //D3D11_BUFFER_DESC bd;
    //ZeroMemory(&bd, sizeof(bd));

    //bd.Usage = D3D11_USAGE_DEFAULT;
    //bd.ByteWidth = sizeof(UINT) * (cloth->getNumStructuralSprings() + cloth->getNumShearSprings()) * 2;
    //bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    //bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;
    hr = d3dDevice->CreateBuffer(&bd, &InitData, &indexBuffer);

    delete[] indices;

    if (FAILED(hr)) {
      return hr;
    }

    immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
  }

  return S_OK;
}

HRESULT Application::initWindow(HINSTANCE hInstance, int nCmdShow) {
  // Register class
  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, (LPCTSTR) IDI_TUTORIAL1);
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
  wcex.lpszMenuName = nullptr;
  wcex.lpszClassName = "TutorialWindowClass";
  wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR) IDI_TUTORIAL1);
  if (!RegisterClassEx(&wcex))
    return E_FAIL;

  // Create window
  hInst = hInstance;
  RECT rc = { 0, 0, 960, 540 };
  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
  hWnd = CreateWindow("TutorialWindowClass", "FGGC Semester 2 Framework", WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
    nullptr);
  if (!hWnd) {
    return E_FAIL;
  }

  ShowWindow(hWnd, SW_MAXIMIZE);

  return S_OK;
}

HRESULT Application::compileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut) {
  HRESULT hr = S_OK;

  DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
  // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
  // Setting this flag improves the shader debugging experience, but still allows 
  // the shaders to be optimized and to run exactly the way they will run in 
  // the release configuration of this program.
  dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

  ID3DBlob* pErrorBlob;
  hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
    dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

  if (FAILED(hr)) {
    if (pErrorBlob != nullptr)
      OutputDebugStringA((char*) pErrorBlob->GetBufferPointer());

    if (pErrorBlob) pErrorBlob->Release();

    return hr;
  }

  if (pErrorBlob) pErrorBlob->Release();

  return S_OK;
}

HRESULT Application::initDevice() {
  HRESULT hr = S_OK;

  UINT createDeviceFlags = 0;

#ifdef _DEBUG
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  D3D_DRIVER_TYPE driverTypes[] = {
    D3D_DRIVER_TYPE_HARDWARE,
    D3D_DRIVER_TYPE_WARP,
    D3D_DRIVER_TYPE_REFERENCE,
  };

  UINT numDriverTypes = ARRAYSIZE(driverTypes);

  D3D_FEATURE_LEVEL featureLevels[] = {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
  };

  UINT numFeatureLevels = ARRAYSIZE(featureLevels);

  UINT sampleCount = 1;

  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 1;
  sd.BufferDesc.Width = renderWidth;
  sd.BufferDesc.Height = renderHeight;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hWnd;
  sd.SampleDesc.Count = sampleCount;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;

  for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {
    driverType = driverTypes[driverTypeIndex];
    hr = D3D11CreateDeviceAndSwapChain(nullptr, driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
      D3D11_SDK_VERSION, &sd, &swapChain, &d3dDevice, &featureLevel, &immediateContext);
    if (SUCCEEDED(hr)) {
      break;
    }
  }

  if (FAILED(hr)) {
    return hr;
  }

  // Create a render target view
  ID3D11Texture2D* pBackBuffer = nullptr;
  hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &pBackBuffer);

  if (FAILED(hr)) {
    return hr;
  }

  hr = d3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &renderTargetView);
  pBackBuffer->Release();

  if (FAILED(hr)) {
    return hr;
  }

  // Setup the viewport
  vp.Width = (FLOAT) renderWidth;
  vp.Height = (FLOAT) renderHeight;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  immediateContext->RSSetViewports(1, &vp);

  initShadersAndInputLayout();

  initVertexBuffer();
  initIndexBuffer();

  // Set primitive topology
  immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

  // Create the constant buffer
  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(bd));
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.ByteWidth = sizeof(ConstantBuffer);
  bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd.CPUAccessFlags = 0;
  hr = d3dDevice->CreateBuffer(&bd, nullptr, &constantBuffer);

  if (FAILED(hr)) {
    return hr;
  }

  D3D11_TEXTURE2D_DESC depthStencilDesc;

  depthStencilDesc.Width = renderWidth;
  depthStencilDesc.Height = renderHeight;
  depthStencilDesc.MipLevels = 1;
  depthStencilDesc.ArraySize = 1;
  depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilDesc.SampleDesc.Count = sampleCount;
  depthStencilDesc.SampleDesc.Quality = 0;
  depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
  depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthStencilDesc.CPUAccessFlags = 0;
  depthStencilDesc.MiscFlags = 0;

  d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);
  d3dDevice->CreateDepthStencilView(depthStencilBuffer, nullptr, &depthStencilView);

  immediateContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

  // Rasterizer
  D3D11_RASTERIZER_DESC cmdesc;

  ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
  cmdesc.FillMode = D3D11_FILL_SOLID;
  cmdesc.CullMode = D3D11_CULL_BACK;
  cmdesc.FrontCounterClockwise = false;
  hr = d3dDevice->CreateRasterizerState(&cmdesc, &cwCullMode);

  if (FAILED(hr)) {
    return hr;
  }

  return S_OK;
}

void Application::cleanup() {
  if (immediateContext) immediateContext->ClearState();
  if (constantBuffer) constantBuffer->Release();
  if (vertexBuffer) vertexBuffer->Release();
  if (indexBuffer) indexBuffer->Release();
  if (vertexLayout) vertexLayout->Release();
  if (instanceLayout) instanceLayout->Release();
  if (vertexShader) vertexShader->Release();
  if (pixelShader) pixelShader->Release();
  if (renderTargetView) renderTargetView->Release();
  if (swapChain) swapChain->Release();
  if (immediateContext) immediateContext->Release();
  if (d3dDevice) d3dDevice->Release();
  if (depthStencilView) depthStencilView->Release();
  if (depthStencilBuffer) depthStencilBuffer->Release();
  if (cwCullMode) cwCullMode->Release();
  delete camera;
  delete[] vertices;
}

void Application::update(double deltaT) {
  // Update camera
  camera->update();
  cloth->update(deltaT);
  a.addForce(XMVectorScale(XMVectorSet(0.0f, -.981, 0.0f, 0.0f), a.getMass()));
  b.addForce(XMVectorScale(XMVectorSet(0.0f, -.981, 0.0f, 0.0f), b.getMass()));
  spring.calcSpringForce();
  a.update(deltaT);
  b.update(deltaT);

  D3D11_MAPPED_SUBRESOURCE mappedData;
  immediateContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
  const Particle* const particles = cloth->getParticles();
  UINT* data = reinterpret_cast<UINT*>(mappedData.pData);
  for (int i = 0; i < cloth->getNumRows(); i++) {
    for (int j = 0; j < cloth->getNumColumns(); j++) {
      XMFLOAT3& posL = vertices[(i * cloth->getNumColumns()) + j].posL;
      XMVECTOR pos = particles[(i * cloth->getNumColumns()) + j].getPosition();
      posL.x = pos.m128_f32[0];
      posL.y = pos.m128_f32[1];
      posL.z = pos.m128_f32[2];
    }
  }
  //vertices[0].posL.x = a.getPosition().m128_f32[0];
  //vertices[0].posL.y = a.getPosition().m128_f32[1];
  //vertices[0].posL.z = a.getPosition().m128_f32[2];
  //vertices[1].posL.x = b.getPosition().m128_f32[0];
  //vertices[1].posL.y = b.getPosition().m128_f32[1];
  //vertices[1].posL.z = b.getPosition().m128_f32[2];
  //memcpy(mappedData.pData, &vertices[0], sizeof(SimpleVertex) * 2);
  memcpy(mappedData.pData, &vertices[0], sizeof(SimpleVertex) * cloth->getNumColumns() * cloth->getNumRows());
  immediateContext->Unmap(vertexBuffer, 0);
}

void Application::draw() {
  ConstantBuffer cb;

  immediateContext->RSSetState(cwCullMode);
  immediateContext->VSSetShader(vertexShader, nullptr, 0);
  immediateContext->PSSetShader(pixelShader, nullptr, 0);
  immediateContext->VSSetConstantBuffers(0, 1, &constantBuffer);
  immediateContext->PSSetConstantBuffers(0, 1, &constantBuffer);
  immediateContext->IASetInputLayout(vertexLayout);

  immediateContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
  immediateContext->RSSetViewports(1, &vp);
  float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
  immediateContext->ClearRenderTargetView(renderTargetView, ClearColor);
  immediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  XMFLOAT4X4 viewAsFloats = camera->getView();
  XMFLOAT4X4 projectionAsFloats = camera->getProjection();

  XMMATRIX view = XMLoadFloat4x4(&viewAsFloats);
  XMMATRIX projection = XMLoadFloat4x4(&projectionAsFloats);

  cb.view = XMMatrixTranspose(view);
  cb.projection = XMMatrixTranspose(projection);

  immediateContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);
  //immediateContext->DrawIndexed(2, 0, 0);
  cloth->draw(immediateContext);

  swapChain->Present(0, 0);
}