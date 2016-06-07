#include "Spring.h"

Spring::Spring() : restLength(0.0f), stiffness(0.0f), damping(0.0f), p1(nullptr), p2(nullptr) {}

Spring::Spring(float stiffness, float damping, Particle* const p1, Particle* const p2) : stiffness(stiffness), damping(damping), p1(p1), p2(p2) {
  XMFLOAT3 length;
  XMStoreFloat3(&length, XMVector3Length(XMLoadFloat3(&p1->getPosition()) - XMLoadFloat3(&p2->getPosition())));
  restLength = length.x;
}

const XMVECTOR Spring::calcSpringForce() {
  XMVECTOR springForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

  return springForce;
}