#include "Particle.h"
#include <iostream>

const double Particle::TIME_FOR_EQUILIBIRUM = 10000.0;

Particle::Particle() : mass(0.0f), pinned(false), equilibrium(false), timeAtEquilibrium(0.0) {
  position = previousPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  acceleration = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}

Particle::Particle(float mass, FXMVECTOR position) :
  mass(mass), pinned(false), equilibrium(false), timeAtEquilibrium(0.0) {
  this->position = previousPosition = position;
  acceleration = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
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
}