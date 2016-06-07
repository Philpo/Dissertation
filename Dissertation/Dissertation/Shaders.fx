cbuffer ConstantBuffer : register(b0) {
  matrix view;
  matrix projection;
}

struct VS_INPUT {
  float4 posL : POSITION;
};

struct VS_OUTPUT {
  float4 posH : SV_POSITION;
};

VS_OUTPUT VS(VS_INPUT input) {
  VS_OUTPUT output = (VS_OUTPUT) 0;

  float4 posW = input.posL;
  output.posH = mul(posW, view);
  output.posH = mul(output.posH, projection);

  return output;
}

float4 PS(VS_OUTPUT input) : SV_Target {
  return float4(1.0f, 0.0f, 0.0f, 1.0f);
}