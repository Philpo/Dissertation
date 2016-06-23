#include "Particle.h"

Particle::Particle() : timeSpentIntegrating(0.0), mass(0.0f), pinned(false) {
  position = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  normal = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  acceleration = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}

Particle::Particle(float mass, float damingCoefficient, FXMVECTOR position, FXMVECTOR normal) :
  timeSpentIntegrating(0.0), mass(mass), dampingCoefficient(damingCoefficient), pinned(false) {
  this->position = position;
  this->normal = normal;
  acceleration = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}

void Particle::update(double deltaT) {
  if (!pinned) {
    integrate(deltaT);
    totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  }
}

void Particle::integrate(double deltaT) {
  double currentTime = getCounter();

  float timeInSeconds = deltaT / 1000.0f;
  if (closeToZero()) {
    return;
  }
  acceleration = XMVectorScale(totalForce, 1 / mass);
  velocity = XMVectorAdd(velocity, XMVectorScale(acceleration, timeInSeconds));
  //velocity = XMVectorAdd(velocity, XMVectorScale(velocity, -dampingCoefficient));
  //position = XMVectorAdd(position, XMVectorAdd(XMVectorScale(velocity, timeInSeconds), XMVectorScale(XMVectorScale(acceleration, timeInSeconds * timeInSeconds), 0.5f)));
  position = XMVectorAdd(position, XMVectorScale(velocity, timeInSeconds));

  timeSpentIntegrating = getCounter() - currentTime;
  int a = 1;
}

bool Particle::closeToZero() {
  return (abs(totalForce.m128_f32[0]) > 0.0f && abs(totalForce.m128_f32[0]) < 0.0001f) || (abs(totalForce.m128_f32[1]) > 0.0f && abs(totalForce.m128_f32[1]) < 0.0001f) || (abs(totalForce.m128_f32[2]) > 0.0f && abs(totalForce.m128_f32[2]) < 0.0001f);
}