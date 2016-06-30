#include "ExplicitEulerIntegrator.h"
#include "Particle.h"

std::unique_ptr<IIntegrator> ExplicitEulerIntegrator::instance = nullptr;

ExplicitEulerIntegrator::ExplicitEulerIntegrator() : timeStep(0.0), timeSinceLastIntegration(0.0){}

IIntegrator* const ExplicitEulerIntegrator::getInstance() {
  if (!instance) {
    instance.reset(new ExplicitEulerIntegrator);
  }

  return instance.get();
}

void ExplicitEulerIntegrator::integrate(Particle& particle, double deltaT) {
  timeSinceLastIntegration += deltaT;

  if (timeSinceLastIntegration >= timeStep) {
    XMVECTOR acceleration = XMVectorScale(particle.totalForce, 1 / particle.mass);
    particle.velocity = XMVectorAdd(particle.velocity, XMVectorScale(acceleration, timeStepInSeconds));
    ////velocity = XMVectorAdd(velocity, XMVectorScale(velocity, -dampingCoefficient));
    ////position = XMVectorAdd(position, XMVectorAdd(XMVectorScale(velocity, timeInSeconds), XMVectorScale(XMVectorScale(acceleration, timeInSeconds * timeInSeconds), 0.5f)));
    particle.position = XMVectorAdd(particle.position, XMVectorScale(particle.velocity, timeStepInSeconds));
    timeSinceLastIntegration = 0.0;
  }
}