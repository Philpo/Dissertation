#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "Camera.h"
#include <vector>
#include <time.h>
#include <iostream>
#include <sstream>
#include "TypeDefs.h"
#include "resource.h"
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include "Cloth.h"
#include "ExplicitEuler.h"
#include "VerletIntegrator.h"
#include "ExplicitEulerIntegrator.h"

using namespace DirectX;

enum Scenario {
  SHEET = 0
, FLAG = 1
};

class Application {
public:
  Application();
  ~Application();

  HRESULT initialise(HINSTANCE hInstance, int nCmdShow);

  bool handleKeyboard(MSG msg);
  void handleMouseMovement(WPARAM buttonStates, int x, int y);
  void handleMouseClick(WPARAM buttonStates, int x, int y);
  bool reachedEquilibrium() const { return cloth ? cloth->reachedEquilibrium() : false; }

  void update(double deltaT);
  void draw();
  inline void setWindowCaption(const std::stringstream& caption) const { SetWindowText(hWnd, caption.str().c_str()); }
  HRESULT loadTest(xml_node<>* testNode, Scenario scenario);
private:
  HINSTANCE hInst;
  HWND hWnd;
  D3D_DRIVER_TYPE driverType;
  D3D_FEATURE_LEVEL featureLevel;
  ID3D11Device* d3dDevice;
  ID3D11DeviceContext* immediateContext;
  IDXGISwapChain* swapChain;
  ID3D11RenderTargetView* renderTargetView;
  ID3D11VertexShader* vertexShader;
  ID3D11PixelShader* pixelShader;
  ID3D11InputLayout* vertexLayout;
  ID3D11InputLayout* instanceLayout;

  ID3D11Buffer* vertexBuffer;
  ID3D11Buffer* indexBuffer;

  ID3D11Buffer* constantBuffer;

  ID3D11DepthStencilView* depthStencilView = nullptr;
  ID3D11Texture2D* depthStencilBuffer = nullptr;

  D3D11_VIEWPORT vp;
  SimpleVertex* vertices;

  Camera* camera;
  Cloth* cloth;
  Particle a, b;
  Spring spring;
  float cameraSpeed = 2.0f;
  double timeSinceLastUpdate = 0.0f;

  UINT windowHeight;
  UINT windowWidth;

  // Render dimensions - Change here to alter screen resolution
  UINT renderHeight = 1080;
  UINT renderWidth = 1920;

  ID3D11RasterizerState* cwCullMode;
  int lastMousePosX, lastMousePosY;

  HRESULT initWindow(HINSTANCE hInstance, int nCmdShow);
  HRESULT initDevice();
  void cleanup();
  HRESULT compileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
  HRESULT initShadersAndInputLayout();
  HRESULT initVertexBuffer();
  HRESULT initIndexBuffer();
};