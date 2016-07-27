#pragma once
#include "Utils.h"
#include <directxmath.h>

using namespace DirectX;

class VerletIntegrator;
class ExplicitEulerIntegrator;
class FourthOrderRungeKuttaIntegrator;
class MidpointIntegrator;

class Particle {
  friend class VerletIntegrator;
  friend class ExplicitEulerIntegrator;
  friend class FourthOrderRungeKuttaIntegrator;
  friend class MidpointIntegrator;
public:
  Particle();
  Particle(float mass, FXMVECTOR position);
  ~Particle() {}

  const float getMass() const { return mass; }
  const bool isPinned() const { return pinned; }
  const XMVECTOR& getPosition() const { return position; }
  const XMVECTOR& getPreviousPosition() const { return previousPosition; }
  const XMVECTOR& getAcceleration() const { return acceleration; }
  const XMVECTOR& getVelocity() const { return velocity; }
  const XMVECTOR& getTotalForce() const { return totalForce; }

  void setPinned(const bool pinned) { this->pinned = pinned; }
  void setPosition(const FXMVECTOR& position) { previousPosition = this->position; this->position = position; }
  void setAcceleration(const FXMVECTOR& acceleration) { this->acceleration = acceleration; }
  void setVelocity(const FXMVECTOR& velocity) { this->velocity = velocity; }

  void addForce(const FXMVECTOR& force) { totalForce = XMVectorAdd(totalForce, force); }
  void zeroForce() { totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f); }
private:
  float mass;
  bool pinned;
  XMVECTOR position, previousPosition, acceleration, velocity, totalForce;
};