#include "VerletIntegrator.h"

std::unique_ptr<IIntegrator> VerletIntegrator::instance = nullptr;

VerletIntegrator::VerletIntegrator() : timeStep(0.0), timeSpentIntegrating(0.0), timeAtStart(0.0) {}

IIntegrator* const VerletIntegrator::getInstance() {
  if (!instance) {
    instance.reset(new VerletIntegrator);
  }

  return instance.get();
}

void VerletIntegrator::integrate(Cloth& cloth) {
  if (timeAtStart == 0.0) {
    timeAtStart = getCounter();
  }

  cloth.calcForces();

  float dampFactor = 0.995f;

  XMVECTOR acceleration, velocity;
  double currentTime = getCounter();

  for (int i = 0; i < cloth.rows; i++) {
    for (int j = 0; j < cloth.columns; j++) {
      Particle& particle = cloth.particles[(i * cloth.columns) + j];

      if (!particle.pinned) {
        acceleration = XMVectorScale(particle.totalForce, 1 / particle.mass);
        velocity = XMVectorSubtract(particle.position, particle.previousPosition);
        acceleration = XMVectorScale(acceleration, timeStepInSeconds * timeStepInSeconds);
        particle.previousPosition = particle.position;
        particle.position = XMVectorAdd(particle.position, XMVectorAdd(XMVectorScale(velocity, dampFactor), acceleration));
      }

      if (!unitTests) {
        particle.totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
      }
    }
  }

  timeSpentIntegrating += getCounter() - currentTime;
}