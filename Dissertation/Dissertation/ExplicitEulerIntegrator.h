#pragma once
#include "IIntegrator.h"
#include <memory>

class ExplicitEulerIntegrator : public IIntegrator {
public:
  ~ExplicitEulerIntegrator() {}

  static IIntegrator* const getInstance();
  void setTimeStep(double timeStep) override { this->timeStep = timeStep; timeStepInSeconds = timeStep / 1000.0f; }
  void integrate(Particle& particle, double deltaT);
private:
  static std::unique_ptr<IIntegrator> instance;
  double timeStep, timeSinceLastIntegration;
  float timeStepInSeconds;

  ExplicitEulerIntegrator();
};