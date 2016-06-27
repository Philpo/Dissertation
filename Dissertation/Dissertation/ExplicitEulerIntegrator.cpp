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
    float timeInSeconds = deltaT / 1000.0f;

    XMVECTOR acceleration = XMVectorScale(particle.getTotalForce(), 1 / particle.getMass());
    XMVECTOR velocity = XMVectorAdd(particle.getVelocity(), XMVectorScale(acceleration, timeInSeconds));
    particle.setVelocity(velocity);
    ////velocity = XMVectorAdd(velocity, XMVectorScale(velocity, -dampingCoefficient));
    ////position = XMVectorAdd(position, XMVectorAdd(XMVectorScale(velocity, timeInSeconds), XMVectorScale(XMVectorScale(acceleration, timeInSeconds * timeInSeconds), 0.5f)));
    particle.setPosition(XMVectorAdd(particle.getPosition(), XMVectorScale(velocity, timeInSeconds)));
    timeSinceLastIntegration = 0.0;
  }
}