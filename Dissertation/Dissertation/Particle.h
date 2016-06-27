#pragma once
#include "Utils.h"
#include "IIntegrator.h"
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
  const XMVECTOR& getPreviousPosition() const { return previousPosition; }
  const XMVECTOR& getNormal() const { return normal; }
  const XMVECTOR& getAcceleration() const { return acceleration; }
  const XMVECTOR& getVelocity() const { return velocity; }
  const XMVECTOR& getTotalForce() const { return totalForce; }

  void setPinned(const bool pinned) { this->pinned = pinned; }
  void setIntegrator(IIntegrator* const integrator) { integratorInterface = integrator; }
  void setIntegrator(IntegrationFunction integrator) { integratorFPointer = integrator; }
  void setPosition(const FXMVECTOR& position) { previousPosition = this->position; this->position = position; }
  void setAcceleration(const FXMVECTOR& acceleration) { this->acceleration = acceleration; }
  void setVelocity(const FXMVECTOR& velocity) { this->velocity = velocity; }

  void addForce(const FXMVECTOR& force) { totalForce = XMVectorAdd(totalForce, force); }
  void update(double deltaT);
private:
  double timeSpentIntegrating;
  float mass, dampingCoefficient;
  bool pinned, equilibrium;
  int numFramesAtEquilibrium;
  IIntegrator* integratorInterface;
  IntegrationFunction integratorFPointer;
  XMVECTOR position, previousPosition, normal, acceleration, velocity, totalForce;
  static const int NUM_FRAMES_FOR_EQUILIBIRUM;

  void integrate(double deltaT);
  bool closeToZero();
};