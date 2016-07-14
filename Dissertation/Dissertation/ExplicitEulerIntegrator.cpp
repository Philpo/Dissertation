#include "ExplicitEulerIntegrator.h"
#include "Particle.h"
#include "Cloth.h"

std::unique_ptr<IIntegrator> ExplicitEulerIntegrator::instance = nullptr;

ExplicitEulerIntegrator::ExplicitEulerIntegrator() : timeStep(0.0), timeSinceLastIntegration(0.0), timeSpentIntegrating(0.0), timeAtStart(0.0), timeAtEquilibrium(0.0), equilibrium(false) {}

IIntegrator* const ExplicitEulerIntegrator::getInstance() {
  if (!instance) {
    instance.reset(new ExplicitEulerIntegrator);
  }

  return instance.get();
}

void ExplicitEulerIntegrator::integrate(Particle& particle, double deltaT) {
  timeSinceLastIntegration += deltaT;

  //if (timeSinceLastIntegration >= timeStep) {
    XMVECTOR acceleration = XMVectorScale(particle.totalForce, 1 / particle.mass);
    particle.velocity = XMVectorAdd(particle.velocity, XMVectorScale(acceleration, timeStepInSeconds));
    ////velocity = XMVectorAdd(velocity, XMVectorScale(velocity, -dampingCoefficient));
    ////position = XMVectorAdd(position, XMVectorAdd(XMVectorScale(velocity, timeInSeconds), XMVectorScale(XMVectorScale(acceleration, timeInSeconds * timeInSeconds), 0.5f)));
    particle.temp = particle.previousPosition;
    particle.previousPosition = particle.position;
    particle.position = XMVectorAdd(particle.position, XMVectorScale(particle.velocity, timeStepInSeconds));
    timeSinceLastIntegration = 0.0;
    particle.totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  //}
}

void ExplicitEulerIntegrator::integrate(Cloth& cloth) {
  if (timeAtStart == 0.0) {
    timeAtStart = getCounter();
  }

  cloth.calcForces();

  float dampFactor = 0.995f;

  XMVECTOR acceleration;
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

        acceleration = XMVectorScale(particle.totalForce, timeStepInSeconds / particle.mass);
        particle.velocity = XMVectorAdd(particle.velocity, acceleration);//XMVectorScale(acceleration, timeStepInSeconds));
        ////velocity = XMVectorAdd(velocity, XMVectorScale(velocity, -dampingCoefficient));
        ////position = XMVectorAdd(position, XMVectorAdd(XMVectorScale(velocity, timeInSeconds), XMVectorScale(XMVectorScale(acceleration, timeInSeconds * timeInSeconds), 0.5f)));
        particle.temp = particle.previousPosition;
        particle.previousPosition = particle.position;
        particle.position = XMVectorAdd(particle.position, XMVectorScale(particle.velocity, timeStepInSeconds));
        particle.totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

        if (!particle.equilibrium) {
          notAtEquilibrium = true;
        }
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