#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Particle;

enum Integrators {
  EXPLICIT_EULER = 0
, VERLET = 1
, FOURTH_ORDER_RUNGE_KUTTA = 2
, IMPLICIT_EULER = 3
};

class IIntegrator {
public:
  virtual ~IIntegrator() {}

  virtual void setTimeStep(double timeStep) = 0;
  virtual void integrate(Particle& particle, double deltaT) = 0;
};