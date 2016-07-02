#include "VerletIntegrator.h"
#include "Particle.h"

std::unique_ptr<IIntegrator> VerletIntegrator::instance = nullptr;

VerletIntegrator::VerletIntegrator() : timeStep(0.0), timeSinceLastIntegration(0.0) {}

IIntegrator* const VerletIntegrator::getInstance() {
  if (!instance) {
    instance.reset(new VerletIntegrator);
  }

  return instance.get();
}

void VerletIntegrator::integrate(Particle& particle, double deltaT) {

  //if (timeSinceLastIntegration >= timeStep) {
  float dampFactor = 0.995f;

    XMVECTOR acceleration = XMVectorScale(particle.totalForce, 1 / particle.mass);
    particle.velocity = XMVectorSubtract(particle.position, particle.previousPosition);
    acceleration = XMVectorScale(acceleration, timeStepInSeconds * timeStepInSeconds);
    particle.previousPosition = particle.position;
    particle.position = XMVectorAdd(particle.position, XMVectorAdd(XMVectorScale(particle.velocity, dampFactor), acceleration));
    particle.totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  //}
}