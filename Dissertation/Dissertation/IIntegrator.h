#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Particle;

enum Integrator {
  EXPLICIT_EULER = 0
, VERLET = 1
, FOURTH_ORDER_RUNGE_KUTTA = 2
, IMPLICIT_EULER = 3
};

// sadly the simplest solution to the problem of converting an enum to a string for output to the CSV files
// other, more generic solutions involve messy preprocessor declarations
// this solution, whilst not ideal, is suitable enough for this project
static const char* INTEGRATOR_NAMES[] = { "Explicit Euler", "Verlet", "Fourth Order Runge-Kutta", "Implicit Euler" };

class IIntegrator {
public:
  virtual ~IIntegrator() {}

  virtual void setTimeStep(double timeStep) = 0;
  virtual void integrate(Particle& particle, double deltaT) = 0;
};