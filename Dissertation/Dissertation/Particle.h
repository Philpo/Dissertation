#pragma once
#include "Utils.h"
#include <directxmath.h>

using namespace DirectX;

class Particle {
public:
  Particle();
  Particle(float mass, float dampingCoefficient, FXMVECTOR position, FXMVECTOR normal);
  ~Particle() {}

  const double getTimeSpentIntegrating() const { return timeSpentIntegrating; }
  const float getMass() const { return mass; }
  const bool isPinned() const { return pinned; }
  const bool reachedEquilibrium() const { return equilibrium; }
  const XMVECTOR& getPosition() const { return position; }
  const XMVECTOR& getNormal() const { return normal; }
  const XMVECTOR& getAcceleration() const { return acceleration; }
  const XMVECTOR& getVelocity() const { return velocity; }

  void setPinned(const bool pinned) { this->pinned = pinned; }
  void setPosition(const FXMVECTOR& position) { this->position = position; }
  void setAcceleration(const FXMVECTOR& acceleration) { this->acceleration = acceleration; }
  void setVelocity(const FXMVECTOR& velocity) { this->velocity = velocity; }

  void addForce(const FXMVECTOR& force) { totalForce = XMVectorAdd(totalForce, force); }
  void update(double deltaT);
private:
  double timeSpentIntegrating;
  float mass, dampingCoefficient;
  bool pinned, equilibrium;
  int numFramesAtEquilibrium;
  XMVECTOR position, normal, acceleration, velocity, totalForce;
  static const int NUM_FRAMES_FOR_EQUILIBIRUM;

  void integrate(double deltaT);
  bool closeToZero();
};