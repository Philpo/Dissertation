#include "Particle.h"

Particle::Particle() : mass(0.0f), pinned(false), position(0.0f, 0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), acceleration(0.0f, 0.0f, 0.0f), velocity(0.0f, 0.0f, 0.0f) {
  pos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  norm = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  accel = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  vel = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}

Particle::Particle(float mass, float damingCoefficient, FXMVECTOR position, FXMVECTOR normal) :
  mass(mass), dampingCoefficient(damingCoefficient), pinned(false), position(0.0f, 0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), acceleration(0.0f, 0.0f, 0.0f), velocity(0.0f, 0.0f, 0.0f) {
  XMStoreFloat3(&this->position, position);
  XMStoreFloat3(&this->normal, normal);
  pos = position;
  norm = normal;
  accel = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  vel = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}

void Particle::update(double deltaT) {
  if (!pinned) {
    integrate(deltaT);
    totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  }
}

void Particle::integrate(double deltaT) {
  float timeInSeconds = deltaT / 1000.0f;
  if (closeToZero()) {
    totalForce.m128_f32[0] = 0.0f;
    totalForce.m128_f32[1] = 0.0f;
    totalForce.m128_f32[2] = 0.0f;
  }
  accel = XMVectorScale(totalForce, 1 / mass);
  vel = XMVectorAdd(vel, XMVectorScale(accel, timeInSeconds));
  vel = XMVectorAdd(vel, XMVectorScale(vel, -dampingCoefficient));
  pos = XMVectorAdd(pos, XMVectorAdd(XMVectorScale(vel, timeInSeconds), XMVectorScale(XMVectorScale(accel, timeInSeconds * timeInSeconds), 0.5f)));
  
  XMStoreFloat3(&acceleration, accel);
  XMStoreFloat3(&velocity, vel);
  XMStoreFloat3(&position, pos);
}

bool Particle::closeToZero() {
  return (abs(totalForce.m128_f32[0]) > 0.0f && abs(totalForce.m128_f32[0]) < 0.0001f) || (abs(totalForce.m128_f32[1]) > 0.0f && abs(totalForce.m128_f32[1]) < 0.0001f) || (abs(totalForce.m128_f32[2]) > 0.0f && abs(totalForce.m128_f32[2]) < 0.0001f);
}