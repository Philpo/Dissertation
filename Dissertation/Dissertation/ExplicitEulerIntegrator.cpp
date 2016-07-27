#include "ExplicitEulerIntegrator.h"

std::unique_ptr<IIntegrator> ExplicitEulerIntegrator::instance = nullptr;

ExplicitEulerIntegrator::ExplicitEulerIntegrator() : timeStep(0.0), timeSpentIntegrating(0.0), timeAtStart(0.0) {}

IIntegrator* const ExplicitEulerIntegrator::getInstance() {
  if (!instance) {
    instance.reset(new ExplicitEulerIntegrator);
  }

  return instance.get();
}

void ExplicitEulerIntegrator::integrate(Cloth& cloth) {
  if (timeAtStart == 0.0) {
    timeAtStart = getCounter();
  }

  cloth.calcForces();

  XMVECTOR acceleration;
  double currentTime = getCounter();

  for (int i = 0; i < cloth.rows; i++) {
    for (int j = 0; j < cloth.columns; j++) {
      Particle& particle = cloth.particles[(i * cloth.columns) + j];

      if (!particle.pinned) {
        acceleration = XMVectorScale(particle.totalForce, timeStepInSeconds / particle.mass);
        particle.velocity = XMVectorAdd(particle.velocity, acceleration);
        particle.previousPosition = particle.position;
        particle.position = XMVectorAdd(particle.position, XMVectorScale(particle.velocity, timeStepInSeconds));
      }

      if (!unitTests) {
        particle.totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
      }
    }
  }

  timeSpentIntegrating += getCounter() - currentTime;
}