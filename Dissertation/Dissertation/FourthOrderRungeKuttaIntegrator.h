#pragma once
#include "IIntegrator.h"
#include <memory>
#include <directxmath.h>

using namespace DirectX;

class FourthOrderRungeKuttaIntegrator : public IIntegrator {
public:
  ~FourthOrderRungeKuttaIntegrator() {}

  static IIntegrator* const getInstance();
  const double getTimeSpentIntegrating() const { return timeSpentIntegrating; }
  const double getTimeTakenToReachEquilibrium() const { return timeAtEquilibrium - timeAtStart; }
  const bool reachedEquilibrium() const { return equilibrium; }

  void resetData() override;
  void setTimeStep(double timeStep) override { this->timeStep = timeStep; timeStepInSeconds = timeStep / 1000.0f; timeStepBy6 = timeStepInSeconds / 6.0f; }
  void integrate(Particle& particle, double deltaT) override;
  void integrate(Cloth& cloth) override;
private:
  static std::unique_ptr<IIntegrator> instance;
  double timeStep, timeSinceLastIntegration, timeSpentIntegrating, timeAtStart, timeAtEquilibrium;
  float timeStepInSeconds, timeStepBy6;
  bool equilibrium;
  XMVECTOR *originalPosition, *originalVelocity, *k1Force, *k2Force, *k3Force, *k4Force;

  FourthOrderRungeKuttaIntegrator();
  void intermediateIntegration(Cloth& cloth, XMVECTOR*& forceArray, bool halfTimeStep);
};