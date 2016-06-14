#include "Spring.h"

Spring::Spring() : restLength(0.0f), stiffness(0.0f), damping(0.0f), p1(nullptr), p2(nullptr) {}

Spring::Spring(float stiffness, float damping, Particle* const p1, Particle* const p2) : stiffness(stiffness), damping(damping), p1(p1), p2(p2) {
  XMFLOAT3 length;
  XMStoreFloat3(&length, XMVector3Length(XMVectorSubtract(p1->getPosition(), p2->getPosition())));
  restLength = length.x;
}

void Spring::calcSpringForce() {
  XMVECTOR length = XMVectorSubtract(p1->getPosition(), p2->getPosition());
  XMFLOAT3 currentLength;
  XMStoreFloat3(&currentLength, XMVector3Length(length));

  float springForce = stiffness * (currentLength.x - restLength);

  XMVECTOR deltaV = XMVectorSubtract(p1->getVelocity(), p2->getVelocity());
  XMVECTOR normLength = XMVectorDivide(length, XMLoadFloat3(&currentLength));

  XMFLOAT3 dampingForce;
  XMVECTOR a, b, c, d;
  a = XMVector3Dot(deltaV, length);
  b = XMVectorScale(a, damping);
  c = XMVectorScale(b, 1 / currentLength.x);
  XMStoreFloat3(&dampingForce, c);
  
  if (abs(dampingForce.x) > springForce) {
    dampingForce.x = springForce;
  }

  XMVECTOR force = XMVectorScale(normLength, -(springForce + dampingForce.x));

  p1->addForce(force);
  p2->addForce(-force);
}