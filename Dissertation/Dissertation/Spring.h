#pragma once
#include "Particle.h"

class Spring {
public:
  Spring();
  Spring(float stiffness, float damping, Particle* const p1, Particle* const p2);
  ~Spring() {}

  const XMVECTOR calcSpringForce();
private:
  float restLength, stiffness, damping;
  Particle *p1, *p2;
};