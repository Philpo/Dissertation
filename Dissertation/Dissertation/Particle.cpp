#include "Particle.h"

const double Particle::TIME_FOR_EQUILIBIRUM = 500.0;

Particle::Particle() : timeSpentIntegrating(0.0), mass(0.0f), pinned(false), equilibrium(false), timeAtEquilibrium(0.0), integratorInterface(nullptr), integratorFPointer(nullptr) {
  position = previousPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  normal = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  acceleration = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}

Particle::Particle(float mass, float damingCoefficient, FXMVECTOR position, FXMVECTOR normal) :
  timeSpentIntegrating(0.0), mass(mass), dampingCoefficient(damingCoefficient), pinned(false), equilibrium(false), timeAtEquilibrium(0.0), integratorInterface(nullptr), integratorFPointer(nullptr) {
  this->position = previousPosition = position;
  this->normal = normal;
  acceleration = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}

void Particle::update(double deltaT) {
  if (!pinned) {
    integrate(deltaT);
  }
}

void Particle::integrate(double deltaT) {
  double currentTime = getCounter();

  //float timeInSeconds = deltaT / 1000.0f;
  bool zeroDisplacement = closeToZero();
  if (updateCount > 500 && zeroDisplacement) {
    equilibrium = timeAtEquilibrium >= TIME_FOR_EQUILIBIRUM;
    if (equilibrium) {
      return;
    }
    timeAtEquilibrium += deltaT;
  }

  if (!zeroDisplacement) {
    timeAtEquilibrium = 0.0;
  }
  //acceleration = XMVectorScale(totalForce, 1 / mass);
  //velocity = XMVectorAdd(velocity, XMVectorScale(acceleration, timeInSeconds));
  ////velocity = XMVectorAdd(velocity, XMVectorScale(velocity, -dampingCoefficient));
  ////position = XMVectorAdd(position, XMVectorAdd(XMVectorScale(velocity, timeInSeconds), XMVectorScale(XMVectorScale(acceleration, timeInSeconds * timeInSeconds), 0.5f)));
  //position = XMVectorAdd(position, XMVectorScale(velocity, timeInSeconds));

  if (integratorInterface) {
    integratorInterface->integrate(*this, deltaT);
  }
  else if (integratorFPointer) {
    integratorFPointer(*this, deltaT);
  }

  timeSpentIntegrating = getCounter() - currentTime;
}

bool Particle::closeToZero() {
  XMVECTOR displacementLastFrame = XMVectorSubtract(position, previousPosition);
  float absX = abs(displacementLastFrame.m128_f32[0]);
  float absY = abs(displacementLastFrame.m128_f32[1]);
  float absZ = abs(displacementLastFrame.m128_f32[2]);

  if ((absX > 0.0f && absX < 0.001f) || (absY > 0.0f && absY < 0.001f) || (absZ > 0.0f && absZ < 0.001f)) {
    return true;
  }
  else if (absX == 0.0f && absY == 0.0f && absZ == 0.0f) {
    return true;
  }
  else {
    return false;
  }

  //return ( > 0.0f && abs(totalForce.m128_f32[0]) < 0.0001f) || (abs(totalForce.m128_f32[1]) > 0.0f && abs(totalForce.m128_f32[1]) < 0.0001f) || (abs(totalForce.m128_f32[2]) > 0.0f && abs(totalForce.m128_f32[2]) < 0.0001f);
}