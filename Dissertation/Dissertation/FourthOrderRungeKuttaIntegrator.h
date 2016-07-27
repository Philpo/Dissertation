#pragma once
#include "IIntegrator.h"
#include <memory>
#include <directxmath.h>

using namespace DirectX;

class FourthOrderRungeKuttaIntegrator : public IIntegrator {
public:
  ~FourthOrderRungeKuttaIntegrator() {}

  static IIntegrator* const getInstance();
  const double getTimeSpentIntegrating() const override { return timeSpentIntegrating; }

  void resetData() override;
  void setTimeStep(double timeStep) override { this->timeStep = timeStep; timeStepInSeconds = timeStep / 1000.0f; halfTimeStep = timeStepInSeconds / 2.0f; timeStepBy6 = timeStepInSeconds / 6.0f; }
  void integrate(Cloth& cloth) override;
private:
  static std::unique_ptr<IIntegrator> instance;
  double timeStep, timeSpentIntegrating, timeAtStart;
  float timeStepInSeconds, halfTimeStep, timeStepBy6;
  XMVECTOR *originalPosition, *originalVelocity, *k1Force, *k2Force, *k3Force, *k4Force;

  FourthOrderRungeKuttaIntegrator();
  void intermediateIntegration(Cloth& cloth, XMVECTOR*& forceArray);
};