#pragma once
#include "IIntegrator.h"
#include <memory>

class VerletIntegrator : public IIntegrator {
public:
  ~VerletIntegrator() {}

  static IIntegrator* const getInstance();
  const double getTimeSpentIntegrating() const { return timeSpentIntegrating; }
  const double getTimeTakenToReachEquilibrium() const { return timeAtEquilibrium - timeAtStart; }
  const bool reachedEquilibrium() const { return equilibrium; }

  void resetData() override { timeSpentIntegrating = timeAtStart = timeAtEquilibrium = 0.0; equilibrium = false; }
  void setTimeStep(double timeStep) override { this->timeStep = timeStep; timeStepInSeconds = timeStep / 1000.0f; }
  void integrate(Cloth& cloth) override;
private:
  static std::unique_ptr<IIntegrator> instance;
  double timeStep, timeSinceLastIntegration, timeSpentIntegrating, timeAtStart, timeAtEquilibrium;
  float timeStepInSeconds;
  bool equilibrium;

  VerletIntegrator();
};