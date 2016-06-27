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
  timeSinceLastIntegration += deltaT;

  if (timeSinceLastIntegration >= timeStep) {
    float timeInSeconds = deltaT / 1000.0;
    timeSinceLastIntegration = 0.0;

    XMVECTOR acceleration = XMVectorScale(particle.getTotalForce(), 1 / particle.getMass());
    XMVECTOR currentPosition = particle.getPosition();
    XMVECTOR velocity = XMVectorSubtract(currentPosition, particle.getPreviousPosition());
    acceleration = XMVectorScale(acceleration, timeInSeconds * timeInSeconds);
    particle.setPosition(XMVectorAdd(currentPosition, XMVectorAdd(velocity, acceleration)));
  }
}