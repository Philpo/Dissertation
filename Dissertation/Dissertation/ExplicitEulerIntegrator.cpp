#include "ExplicitEulerIntegrator.h"

std::unique_ptr<IIntegrator> ExplicitEulerIntegrator::instance = nullptr;

ExplicitEulerIntegrator::ExplicitEulerIntegrator() : timeStep(0.0), timeSinceLastIntegration(0.0), timeSpentIntegrating(0.0), timeAtStart(0.0), timeAtEquilibrium(0.0), equilibrium(false) {}

IIntegrator* const ExplicitEulerIntegrator::getInstance() {
  if (!instance) {
    instance.reset(new ExplicitEulerIntegrator);
  }

  return instance.get();
}

void ExplicitEulerIntegrator::integrate(Cloth& cloth) {
  if (timeAtStart == 0.0) {
    timeAtStart = getCounter();
  }

  cloth.calcForces();

  XMVECTOR acceleration;
  bool notAtEquilibrium = false;
  double currentTime = getCounter();

  for (int i = 0; i < cloth.rows; i++) {
    for (int j = 0; j < cloth.columns; j++) {
      Particle& particle = cloth.particles[(i * cloth.columns) + j];

      if (!particle.pinned) {
        // only check for equilibrium for the sheet scenario, since the flag scenario will never evolve to an equilibrium
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

        acceleration = XMVectorScale(particle.totalForce, timeStepInSeconds / particle.mass);
        particle.velocity = XMVectorAdd(particle.velocity, acceleration);
        particle.previousPosition = particle.position;
        particle.position = XMVectorAdd(particle.position, XMVectorScale(particle.velocity, timeStepInSeconds));

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
}