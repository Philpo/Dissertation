#include "Particle.h"
#include <iostream>

Particle::Particle() : mass(0.0f), pinned(false) {
  position = previousPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  acceleration = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}

Particle::Particle(float mass, FXMVECTOR position) :
  mass(mass), pinned(false) {
  this->position = previousPosition = position;
  acceleration = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}