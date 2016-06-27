#include "ExplicitEuler.h"

double ExplicitEuler::timeStep = 0.0;
double ExplicitEuler::timeSinceLastIntegration = 0.0;

void ExplicitEuler::integrate(Particle& particle, double deltaT) {
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