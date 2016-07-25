#include "Spring.h"

Spring::Spring() : restLength(0.0f), stiffness(0.0f), damping(0.0f), p1(nullptr), p2(nullptr) {}

Spring::Spring(float stiffness, float damping, Particle* const p1, Particle* const p2) : stiffness(stiffness), damping(damping), p1(p1), p2(p2) {
  XMFLOAT3 length;
  XMStoreFloat3(&length, XMVector3Length(XMVectorSubtract(p1->getPosition(), p2->getPosition())));
  restLength = length.x;
}

void Spring::calcSpringForce() {
  XMVECTOR length = XMVectorSubtract(p1->getPosition(), p2->getPosition());
  XMVECTOR currentLength = XMVector3Length(length);

  float springForce = stiffness * (currentLength.m128_f32[0] - restLength);

  XMVECTOR deltaV = XMVectorSubtract(p1->getVelocity(), p2->getVelocity());
  XMVECTOR normLength = XMVectorDivide(length, currentLength);

  XMVECTOR dotProduct, dampenedDotProduct, normalizedDotProduct;
  dotProduct = XMVector3Dot(deltaV, length);
  dampenedDotProduct = XMVectorScale(dotProduct, damping);
  normalizedDotProduct = XMVectorScale(dampenedDotProduct, 1 / currentLength.m128_f32[0]);
  
  if (abs(normalizedDotProduct.m128_f32[0]) > springForce) {
    normalizedDotProduct.m128_f32[0] = springForce;
  }

  XMVECTOR force = XMVectorScale(normLength, -(springForce + normalizedDotProduct.m128_f32[0]));

  p1->addForce(force);
  p2->addForce(XMVectorNegate(force));
}