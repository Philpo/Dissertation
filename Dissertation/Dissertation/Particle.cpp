#include "Particle.h"

Particle::Particle() : mass(0.0f), pinned(false), position(0.0f, 0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), acceleration(0.0f, 0.0f, 0.0f), velocity(0.0f, 0.0f, 0.0f), totalForce(0.0f, 0.0f, 0.0f) {}

Particle::Particle(float mass, FXMVECTOR position, FXMVECTOR normal) : 
  mass(mass), pinned(false), position(0.0f, 0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), acceleration(0.0f, 0.0f, 0.0f), velocity(0.0f, 0.0f, 0.0f), totalForce(0.0f, 0.0f, 0.0f) {
  XMStoreFloat3(&this->position, position);
  XMStoreFloat3(&this->normal, normal);
}

void Particle::update(double deltaT) {
  integrate(deltaT);
  totalForce.x = totalForce.y = totalForce.z = 0.0f;
}

void Particle::integrate(double deltaT) {

}