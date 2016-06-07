#pragma once
#include <directxmath.h>

using namespace DirectX;

struct SimpleVertex {
  XMFLOAT3 posL;
};

struct ConstantBuffer {
  XMMATRIX view;
  XMMATRIX projection;
};