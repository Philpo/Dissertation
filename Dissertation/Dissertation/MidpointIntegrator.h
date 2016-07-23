#pragma once
#include "IIntegrator.h"
#include <memory>

class MidpointIntegrator : public IIntegrator {
public:
  ~MidpointIntegrator() {}
  
  static IIntegrator* const getInstance();
  const double getTimeSpentIntegrating() const { return timeSpentIntegrating; }
  const double getTimeTakenToReachEquilibrium() const { return timeAtEquilibrium - timeAtStart; }
  const bool reachedEquilibrium() const { return equilibrium; }

  void resetData() override { timeSpentIntegrating = timeAtStart = timeAtEquilibrium = 0.0; equilibrium = false; }
  void setTimeStep(double timeStep) override { this->timeStep = timeStep; halfTimeStepInSeconds = (timeStep / 1000.0f) / 2.0f; }
  void integrate(Particle& particle, double deltaT) override;
  void integrate(Cloth& cloth) override;
private:
  static std::unique_ptr<IIntegrator> instance;
  double timeStep, timeSinceLastIntegration, timeSpentIntegrating, timeAtStart, timeAtEquilibrium;
  float halfTimeStepInSeconds;
  bool equilibrium;

  MidpointIntegrator();
  void intermediateIntegration(Cloth& cloth, bool resetForce = true);
};
