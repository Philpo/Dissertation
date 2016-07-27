#pragma once
#include "IIntegrator.h"
#include <memory>

class VerletIntegrator : public IIntegrator {
public:
  ~VerletIntegrator() {}

  static IIntegrator* const getInstance();
  const double getTimeSpentIntegrating() const override { return timeSpentIntegrating; }

  void resetData() override { timeSpentIntegrating = timeAtStart = 0.0; }
  void setTimeStep(double timeStep) override { this->timeStep = timeStep; timeStepInSeconds = timeStep / 1000.0f; }
  void integrate(Cloth& cloth) override;
private:
  static std::unique_ptr<IIntegrator> instance;
  double timeStep, timeSpentIntegrating, timeAtStart;
  float timeStepInSeconds;

  VerletIntegrator();
};