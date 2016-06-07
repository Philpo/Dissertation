#pragma once
#include <directxmath.h>

using namespace DirectX;

class Particle {
public:
  Particle();
  Particle(float mass, FXMVECTOR position, FXMVECTOR normal);
  ~Particle() {}

  const float getMass() const { return mass; }
  const bool isPinned() const { return pinned; }
  const XMFLOAT3 getPosition() const { return position; }
  const XMFLOAT3 getNormal() const { return normal; }
  const XMFLOAT3 getAcceleration() const { return acceleration; }
  const XMFLOAT3 getVelocity() const { return velocity; }

  void setPinned(const bool pinned) { this->pinned = pinned; }
  void setPosition(const FXMVECTOR position) { XMStoreFloat3(&this->position, position); }
  void setAcceleration(const FXMVECTOR acceleration) { XMStoreFloat3(&this->acceleration, acceleration); }
  void setVelocity(const FXMVECTOR velocity) { XMStoreFloat3(&this->velocity, velocity); }

  void addForce(const FXMVECTOR force) { XMStoreFloat3(&totalForce, (XMLoadFloat3(&totalForce) + force)); }
  void update(double deltaT);
private:
  float mass;
  bool pinned;
  XMFLOAT3 position, normal, acceleration, velocity, totalForce;

  void integrate(double deltaT);
};