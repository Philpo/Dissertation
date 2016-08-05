#include "MidpointIntegrator.h"

std::unique_ptr<IIntegrator> MidpointIntegrator::instance = nullptr;

MidpointIntegrator::MidpointIntegrator() : timeStep(0.0), timeSpentIntegrating(0.0), timeAtStart(0.0) {}

IIntegrator* const MidpointIntegrator::getInstance() {
  if (!instance) {
    instance.reset(new MidpointIntegrator);
  }

  return instance.get();
}

void MidpointIntegrator::integrate(Cloth& cloth) {
  if (timeAtStart == 0.0) {
    timeAtStart = getCounter();
  }

  double currentTime = getCounter();

  cloth.calcForces();
  intermediateIntegration(cloth);
  cloth.calcForces();
  intermediateIntegration(cloth, !unitTests);

  timeSpentIntegrating += getCounter() - currentTime;
}

void MidpointIntegrator::intermediateIntegration(Cloth& cloth, bool resetForce) {
  XMVECTOR acceleration;

  for (int i = 0; i < cloth.rows; i++) {
    for (int j = 0; j < cloth.columns; j++) {
      Particle& particle = cloth.particles[(i * cloth.columns) + j];

      if (!particle.pinned) {
        acceleration = XMVectorScale(particle.totalForce, halfTimeStepInSeconds / particle.mass);
        particle.velocity = XMVectorAdd(particle.velocity, acceleration);
        particle.previousPosition = particle.position;
        particle.position = XMVectorAdd(particle.position, XMVectorScale(particle.velocity, halfTimeStepInSeconds));
      }

      if (resetForce) {
        particle.totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
      }
    }
  }
}