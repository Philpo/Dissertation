#include "VerletIntegrator.h"
#include "Particle.h"
#include "Cloth.h"

std::unique_ptr<IIntegrator> VerletIntegrator::instance = nullptr;

VerletIntegrator::VerletIntegrator() : timeStep(0.0), timeSinceLastIntegration(0.0), timeSpentIntegrating(0.0), timeAtStart(0.0), timeAtEquilibrium(0.0), equilibrium(false) {}

IIntegrator* const VerletIntegrator::getInstance() {
  if (!instance) {
    instance.reset(new VerletIntegrator);
  }

  return instance.get();
}

void VerletIntegrator::integrate(Particle& particle, double deltaT) {

  //if (timeSinceLastIntegration >= timeStep) {
  float dampFactor = 0.995f;

    XMVECTOR acceleration = XMVectorScale(particle.totalForce, 1 / particle.mass);
    XMVECTOR velocity = XMVectorSubtract(particle.position, particle.previousPosition);
    acceleration = XMVectorScale(acceleration, timeStepInSeconds * timeStepInSeconds);
    particle.previousPosition = particle.position;
    particle.position = XMVectorAdd(particle.position, XMVectorAdd(XMVectorScale(velocity, dampFactor), acceleration));
    particle.totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  //}
}

void VerletIntegrator::integrate(Cloth& cloth) {
  if (timeAtStart == 0.0) {
    timeAtStart = getCounter();
  }

  cloth.calcForces();

  float dampFactor = 0.995f;

  XMVECTOR acceleration, velocity;
  bool notAtEquilibrium = false;
  double currentTime = getCounter();

  for (int i = 0; i < cloth.rows; i++) {
    for (int j = 0; j < cloth.columns; j++) {
      Particle& particle = cloth.particles[(i * cloth.columns) + j];

      if (!particle.pinned) {
        if (currentScenario == SHEET) {
          bool zeroDisplacement = particle.closeToZero();
          if (updateCount > 500 && zeroDisplacement) {
            if (particle.timeAtEquilibrium == 0.0) {
              particle.timeAtEquilibrium = getCounter();
            }

            particle.equilibrium = getCounter() - particle.timeAtEquilibrium >= particle.TIME_FOR_EQUILIBIRUM;
            if (particle.equilibrium) {
              continue;
            }
          }

          if (!zeroDisplacement) {
            particle.timeAtEquilibrium = 0.0;
          }
        }

        acceleration = XMVectorScale(particle.totalForce, 1 / particle.mass);
        velocity = XMVectorSubtract(particle.position, particle.previousPosition);
        acceleration = XMVectorScale(acceleration, timeStepInSeconds * timeStepInSeconds);
        particle.previousPosition = particle.position;
        particle.position = XMVectorAdd(particle.position, XMVectorAdd(XMVectorScale(velocity, dampFactor), acceleration));

        if (!particle.equilibrium) {
          notAtEquilibrium = true;
        }
      }

      if (!unitTests) {
        particle.totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
      }
    }
  }

  timeSpentIntegrating += getCounter() - currentTime;

  equilibrium = !notAtEquilibrium;

  if (equilibrium) {
    timeAtEquilibrium = getCounter();
  }
  //}
}