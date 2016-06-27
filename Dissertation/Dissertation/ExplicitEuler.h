#pragma once
#include "Particle.h"

class ExplicitEuler {
public:
  ExplicitEuler() = delete;
  ExplicitEuler(ExplicitEuler& toCopy) = delete;
  ~ExplicitEuler() = delete;

  ExplicitEuler operator=(ExplicitEuler& rhs) = delete;

  static void setTimeStep(double step) { timeStep = step; }
  static void integrate(Particle& particle, double deltaT);
private:
  static double timeStep, timeSinceLastIntegration;
};