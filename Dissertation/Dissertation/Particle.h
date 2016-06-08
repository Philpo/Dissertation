#pragma once
#include <directxmath.h>

using namespace DirectX;

class Particle {
public:
  Particle();
  Particle(float mass, float damingCoefficient, FXMVECTOR position, FXMVECTOR normal);
  ~Particle() {}

  const float getMass() const { return mass; }
  const bool isPinned() const { return pinned; }
  const XMFLOAT3& getPosition() const { return position; }
  const XMFLOAT3& getNormal() const { return normal; }
  const XMFLOAT3& getAcceleration() const { return acceleration; }
  const XMFLOAT3& getVelocity() const { return velocity; }
  const XMVECTOR& getPos() const { return pos; }
  const XMVECTOR& getNorm() const { return norm; }
  const XMVECTOR& getAccel() const { return accel; }
  const XMVECTOR& getVel() const { return vel; }

  void setPinned(const bool pinned) { this->pinned = pinned; }
  void setPosition(const FXMVECTOR& position) { XMStoreFloat3(&this->position, position); pos = position; }
  void setAcceleration(const FXMVECTOR& acceleration) { XMStoreFloat3(&this->acceleration, acceleration); accel = acceleration; }
  void setVelocity(const FXMVECTOR& velocity) { XMStoreFloat3(&this->velocity, velocity); vel = velocity; }

  void addForce(const FXMVECTOR& force) { totalForce = XMVectorAdd(totalForce, force); }
  void update(double deltaT);
private:
  float mass, dampingCoefficient;
  bool pinned;
  XMFLOAT3 position, normal, acceleration, velocity;
  XMVECTOR pos, norm, accel, vel, totalForce;

  void integrate(double deltaT);
  bool closeToZero();
};