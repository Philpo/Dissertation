#include "FourthOrderRungeKutta.h"
#include "Particle.h"
#include "Cloth.h"

double FourthOrderRungeKutta::timeStep = 0.0;
double FourthOrderRungeKutta::timeSinceLastIntegration = 0.0;
double FourthOrderRungeKutta::timeSpentIntegrating = 0.0;
double FourthOrderRungeKutta::timeAtStart = 0.0;
double FourthOrderRungeKutta::timeAtEquilibrium = 0.0;
float FourthOrderRungeKutta::timeStepInSeconds = 0.0f;
float FourthOrderRungeKutta::timeStepBy6 = 0.0f;
bool FourthOrderRungeKutta::equilibrium = false;
XMVECTOR* FourthOrderRungeKutta::originalPosition = nullptr;
XMVECTOR* FourthOrderRungeKutta::originalVelocity = nullptr;
XMVECTOR* FourthOrderRungeKutta::k1Force = nullptr;
XMVECTOR* FourthOrderRungeKutta::k2Force = nullptr;
XMVECTOR* FourthOrderRungeKutta::k3Force = nullptr;
XMVECTOR* FourthOrderRungeKutta::k4Force = nullptr;

void FourthOrderRungeKutta::resetData() {
  timeSpentIntegrating = timeAtStart = timeAtEquilibrium = 0.0;
  equilibrium = false;

  delete[] originalPosition;
  originalPosition = nullptr;
  delete[] originalVelocity;
  originalVelocity = nullptr;
  delete[] k1Force;
  k1Force = nullptr;
  delete[] k2Force;
  k2Force = nullptr;
  delete[] k3Force;
  k3Force = nullptr;
  delete[] k4Force;
  k4Force = nullptr;
}

void FourthOrderRungeKutta::integrate(Cloth& cloth) {
  if (timeAtStart == 0.0) {
    timeAtStart = getCounter();

    originalPosition = new XMVECTOR[cloth.rows * cloth.columns];
    originalVelocity = new XMVECTOR[cloth.rows * cloth.columns];
    k1Force = new XMVECTOR[cloth.rows * cloth.columns];
    k2Force = new XMVECTOR[cloth.rows * cloth.columns];
    k3Force = new XMVECTOR[cloth.rows * cloth.columns];
    k4Force = new XMVECTOR[cloth.rows * cloth.columns];
  }

  for (int i = 0; i < cloth.rows; i++) {
    for (int j = 0; j < cloth.columns; j++) {
      originalPosition[(i * cloth.columns) + j] = cloth.particles[(i * cloth.columns) + j].position;
      originalVelocity[(i * cloth.columns) + j] = cloth.particles[(i * cloth.columns) + j].velocity;
    }
  }

  double currentTime = getCounter();

  cloth.calcForces();
  intermediateIntegration(cloth, k1Force, true);
  cloth.calcForces();
  intermediateIntegration(cloth, k2Force, true);
  cloth.calcForces();
  intermediateIntegration(cloth, k3Force, true);
  cloth.calcForces();
  intermediateIntegration(cloth, k4Force, true);

  bool notAtEquilibrium = false;
  XMVECTOR k1PlusK2, k3PlusK4;

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

        k2Force[(i * cloth.columns) + j] = XMVectorScale(k2Force[(i * cloth.columns) + j], 2.0f);
        k3Force[(i * cloth.columns) + j] = XMVectorScale(k3Force[(i * cloth.columns) + j], 2.0f);

        k1PlusK2 = XMVectorAdd(k1Force[(i * cloth.columns) + j], k2Force[(i * cloth.columns) + j]);
        k3PlusK4 = XMVectorAdd(k3Force[(i * cloth.columns) + j], k4Force[(i * cloth.columns) + j]);
        particle.velocity = XMVectorAdd(originalVelocity[(i * cloth.columns) + j], XMVectorScale(XMVectorAdd(k1PlusK2, k3PlusK4), timeStepBy6 / particle.mass));

        particle.previousPosition = originalPosition[(i * cloth.columns) + j];
        particle.position = XMVectorAdd(originalPosition[(i * cloth.columns) + j], XMVectorScale(particle.velocity, timeStepInSeconds));

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

void FourthOrderRungeKutta::intermediateIntegration(Cloth& cloth, XMVECTOR*& forceArray, bool halfTimeStep) {
  XMVECTOR acceleration;
  float timeStep = halfTimeStep ? timeStepInSeconds * 0.5f : timeStepInSeconds;

  for (int i = 0; i < cloth.rows; i++) {
    for (int j = 0; j < cloth.columns; j++) {
      Particle& particle = cloth.particles[(i * cloth.columns) + j];

      if (!particle.pinned) {
        acceleration = XMVectorScale(particle.totalForce, timeStep / particle.mass);
        particle.velocity = XMVectorAdd(particle.velocity, acceleration);
        forceArray[(i * cloth.columns) + j] = particle.totalForce;
        ////velocity = XMVectorAdd(velocity, XMVectorScale(velocity, -dampingCoefficient));
        ////position = XMVectorAdd(position, XMVectorAdd(XMVectorScale(velocity, timeInSeconds), XMVectorScale(XMVectorScale(acceleration, timeInSeconds * timeInSeconds), 0.5f)));
        //particle.temp = particle.previousPosition;
        //particle.previousPosition = particle.position;
        particle.position = XMVectorAdd(particle.position, XMVectorScale(particle.velocity, timeStep));
        particle.totalForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
      }
    }
  }
}