#pragma once
#include <directxmath.h>

using namespace DirectX;

class Cloth;

class FourthOrderRungeKutta {
public:
  static const double getTimeSpentIntegrating() { return timeSpentIntegrating; }
  static const double getTimeTakenToReachEquilibrium() { return timeAtEquilibrium - timeAtStart; }
  static const bool reachedEquilibrium() { return equilibrium; }

  static void resetData();
  static void setTimeStep(double ts) { timeStep = ts; timeStepInSeconds = timeStep / 1000.0f; timeStepBy6 = timeStepInSeconds / 6.0f; }
  static void integrate(Cloth& cloth);
private:
  static double timeStep, timeSinceLastIntegration, timeSpentIntegrating, timeAtStart, timeAtEquilibrium;
  static float timeStepInSeconds, timeStepBy6;
  static bool equilibrium;
  static XMVECTOR *originalPosition, *originalVelocity, *k1Force, *k2Force, *k3Force, *k4Force;

  static void intermediateIntegration(Cloth& cloth, XMVECTOR*& forceArray, bool halfTimeStep);
};