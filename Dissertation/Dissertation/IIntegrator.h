#pragma once
#include <DirectXMath.h>
#include "Cloth.h"

enum Integrator {
  EXPLICIT_EULER = 0
, VERLET = 1
, FOURTH_ORDER_RUNGE_KUTTA = 2
, MIDPOINT = 3
};

// sadly the simplest solution to the problem of converting an enum to a string for output to the CSV files
// other, more generic solutions involve messy preprocessor declarations
// this solution, whilst not ideal, is suitable enough for this project
static const char* INTEGRATOR_NAMES[] = { "Explicit Euler", "Verlet", "Fourth Order Runge-Kutta", "Midpoint" };

class IIntegrator {
public:
  virtual ~IIntegrator() {}

  virtual const double getTimeSpentIntegrating() const = 0;
  virtual const double getTimeTakenToReachEquilibrium() const = 0;
  virtual const bool reachedEquilibrium() const = 0;

  virtual void setTimeStep(double timeStep) = 0;
  virtual void resetData() = 0;
  virtual void integrate(Cloth& cloth) = 0;
};