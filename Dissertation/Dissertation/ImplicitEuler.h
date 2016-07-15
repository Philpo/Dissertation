#pragma once
#include "IIntegrator.h"
#include <fstream>

using namespace std;

class ImplicitEuler : public IIntegrator {
public:
  ~ImplicitEuler() {}

  const double getTimeSpentIntegrating() const override { return 0.0; }
  const double getTimeTakenToReachEquilibrium() const override { return 0.0; }
  const bool reachedEquilibrium() const override { return false; }
  void resetData() override {}
  void setTimeStep(double timeStep) override {}
  void integrate(Particle& particle, double deltaT) override {}
  void integrate(Cloth& cloth) override {}

  void preCalculateHessian(int columns, int n, float structuralStiffness, float shearStiffness, float flexionStiffness);
  void printHessian();
private:
  int n;
  float** hessian;
};