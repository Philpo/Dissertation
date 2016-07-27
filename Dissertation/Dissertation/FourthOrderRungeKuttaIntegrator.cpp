#include "FourthOrderRungeKuttaIntegrator.h"

std::unique_ptr<IIntegrator> FourthOrderRungeKuttaIntegrator::instance = nullptr;

FourthOrderRungeKuttaIntegrator::FourthOrderRungeKuttaIntegrator() : 
timeStep(0.0), timeSpentIntegrating(0.0), timeAtStart(0.0), originalPosition(nullptr), originalVelocity(nullptr), k1Force(nullptr), k2Force(nullptr), k3Force(nullptr), k4Force(nullptr) {}

IIntegrator* const FourthOrderRungeKuttaIntegrator::getInstance() {
  if (!instance) {
    instance.reset(new FourthOrderRungeKuttaIntegrator);
  }

  return instance.get();
}

void FourthOrderRungeKuttaIntegrator::resetData() {
  timeSpentIntegrating = timeAtStart = 0.0;

  delete[] originalPosition;
  originalPosition = nullptr;
  delete[] originalVelocity;
  originalVelocity = nullptr;
  delete[] k1Force;
  k1Force = nullptr;
  delete[] k2Force;
  k2Force = nullptr;
  delete[] k3Force;
  k3Force = nullptr;
  delete[] k4Force;
  k4Force = nullptr;
}

void FourthOrderRungeKuttaIntegrator::integrate(Cloth& cloth) {
  if (timeAtStart == 0.0) {
    timeAtStart = getCounter();

    originalPosition = new XMVECTOR[cloth.rows * cloth.columns];
    originalVelocity = new XMVECTOR[cloth.rows * cloth.columns];
    k1Force = new XMVECTOR[cloth.rows * cloth.columns];
    k2Force = new XMVECTOR[cloth.rows * cloth.columns];
    k3Force = new XMVECTOR[cloth.rows * cloth.columns];
    k4Force = new XMVECTOR[cloth.rows * cloth.columns];
  }

  for (int i = 0; i < cloth.rows; i++) {
    for (int j = 0; j < cloth.columns; j++) {
      originalPosition[(i * cloth.columns) + j] = cloth.particles[(i * cloth.columns) + j].position;
      originalVelocity[(i * cloth.columns) + j] = cloth.particles[(i * cloth.columns) + j].velocity;
    }
  }

  double currentTime = getCounter();

  cloth.calcForces();
  intermediateIntegration(cloth, k1Force);
  cloth.calcForces();
  intermediateIntegration(cloth, k2Force);
  cloth.calcForces();
  intermediateIntegration(cloth, k3Force);
  cloth.calcForces();
  intermediateIntegration(cloth, k4Force);

  XMVECTOR k1PlusK2, k3PlusK4;
  
  for (int i = 0; i < cloth.rows; i++) {
    for (int j = 0; j < cloth.columns; j++) {
      Particle& particle = cloth.particles[(i * cloth.columns) + j];

      if (!particle.pinned) {
        k2Force[(i * cloth.columns) + j] = XMVectorScale(k2Force[(i * cloth.columns) + j], 2.0f);
        k3Force[(i * cloth.columns) + j] = XMVectorScale(k3Force[(i * cloth.columns) + j], 2.0f);

        k1PlusK2 = XMVectorAdd(k1Force[(i * cloth.columns) + j], k2Force[(i * cloth.columns) + j]);
        k3PlusK4 = XMVectorAdd(k3Force[(i * cloth.columns) + j], k4Force[(i * cloth.columns) + j]);

        particle.velocity = XMVectorAdd(originalVelocity[(i * cloth.columns) + j], XMVectorScale(XMVectorAdd(k1PlusK2, k3PlusK4), timeStepBy6 / particle.mass));
        particle.previousPosition = originalPosition[(i * cloth.columns) + j];
        particle.position = XMVectorAdd(originalPosition[(i * cloth.columns) + j], XMVectorScale(particle.velocity, timeStepInSeconds));
      }
    }
  }

  timeSpentIntegrating += getCounter() - currentTime;
}

void FourthOrderRungeKuttaIntegrator::intermediateIntegration(Cloth& cloth, XMVECTOR*& forceArray) {
  XMVECTOR acceleration;

  for (int i = 0; i < cloth.rows; i++) {
    for (int j = 0; j < cloth.columns; j++) {
      Particle& particle = cloth.particles[(i * cloth.columns) + j];

      if (!particle.pinned) {
        acceleration = XMVectorScale(particle.totalForce, halfTimeStep / particle.mass);
        particle.velocity = XMVectorAdd(particle.velocity, acceleration);
        forceArray[(i * cloth.columns) + j] = particle.totalForce;
        particle.position = XMVectorAdd(particle.position, XMVectorScale(particle.velocity, halfTimeStep));
      }

      particle.totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    }
  }
}