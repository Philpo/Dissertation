#include "Cloth.h"
#include <iostream>

XMVECTOR Cloth::GRAVITY = XMVectorSet(0.0f, -.981, 0.0f, 0.0f);

Cloth::Cloth(FXMVECTOR topLeftPostition, float height, float width, int numRows, int numColumns, float totalMass, float structuralStiffness, float structuralDamping, float shearStiffness, float shearDamping, float flexionStiffness, float flexionDamping, float linearDamping) :
  timeSpentCalculatingInternalForce(0.0), timeSpentIntegrating(0.0), rows(numRows), columns(numColumns) {
  createParticles(topLeftPostition, height, width, totalMass, linearDamping);
  createStructuralLinks(structuralStiffness, structuralDamping);
  createShearLinks(shearStiffness, shearDamping);
  createFlexionLinks(flexionStiffness, flexionDamping);
}

Cloth::~Cloth() {
  delete[] particles;
  delete[] structuralSprings;
  delete[] shearSprings;
  delete[] flexionSprings;
}

void Cloth::setPinned(int row, int column, bool pinned) {
  if (row < rows && column < columns && row >= 0 && column >= 0) {
    particles[(row * columns) + column].setPinned(pinned);
  }
}

void Cloth::update(double deltaT) {
  double currentTime = getCounter();

  for (int i = 0; i < numStructural; i++) {
    structuralSprings[i].calcSpringForce();
  }
  for (int i = 0; i < numShear; i++) {
    shearSprings[i].calcSpringForce();
  }
  for (int i = 0; i < numFlexion; i++) {
    flexionSprings[i].calcSpringForce();
  }

  timeSpentCalculatingInternalForce += getCounter() - currentTime;

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      particles[(i * columns) + j].addForce(XMVectorScale(GRAVITY, particles[(i * columns) + j].getMass()));
      particles[(i * columns) + j].update(deltaT);
      timeSpentIntegrating += particles[(i * columns) + j].getTimeSpentIntegrating();
    }
  }
}

void Cloth::draw(ID3D11DeviceContext* const immediateContext) const {
  immediateContext->DrawIndexed((numStructural + numShear) * 2, 0, 0);
}

void Cloth::createParticles(FXMVECTOR topLeftPostition, float height, float width, float totalMass, float linearDamping) {
  particles = new Particle[rows * columns];

  XMFLOAT3 topLeft;
  XMStoreFloat3(&topLeft, topLeftPostition);

  float particleMass = totalMass / (float) (rows * columns);

  float x, y, z;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      x = topLeft.x + (j * (width / (float) columns));
      y = topLeft.y - (i * (height / (float) rows));
      particles[(i * columns) + j] = Particle(particleMass, linearDamping, XMVectorSet(x, y, topLeft.z, 1.0f), XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
    }
  }
}

void Cloth::createStructuralLinks(float structuralStiffness, float structuralDamping) {
  numStructural = ((rows - 1) * (columns - 1) * 2) + (rows - 1) + (columns - 1);
  structuralSprings = new Spring[numStructural];
  int index = 0;

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (i < rows - 1 && j < columns - 1) {
        structuralSprings[index++] = Spring(structuralStiffness, structuralDamping, &particles[(i * columns) + j], &particles[(i * columns) + j + 1]);
        structuralSprings[index++] = Spring(structuralStiffness, structuralDamping, &particles[(i * columns) + j], &particles[((i + 1) * columns) + j]);
      }
      else if (i == rows - 1 && j < columns - 1) {
        structuralSprings[index++] = Spring(structuralStiffness, structuralDamping, &particles[(i * columns) + j], &particles[(i * columns) + j + 1]);
      }
      else if (j == columns - 1 && i < rows - 1) {
        structuralSprings[index++] = Spring(structuralStiffness, structuralDamping, &particles[(i * columns) + j], &particles[((i + 1) * columns) + j]);
      }
    }
  }
}

void Cloth::createShearLinks(float shearStiffness, float shearDamping) {
  numShear = (rows - 1) * (columns - 1) * 2;
  shearSprings = new Spring[numShear];
  int index = 0;

  for (int i = 0; i < rows - 1; i++) {
    for (int j = 0; j < columns - 1; j++) {
      shearSprings[index++] = Spring(shearStiffness, shearDamping, &particles[(i * columns) + j], &particles[((i + 1) * columns) + j + 1]);
      shearSprings[index++] = Spring(shearStiffness, shearDamping, &particles[(i * columns) + j + 1], &particles[((i + 1) * columns) + j]);
    }
  }
}

void Cloth::createFlexionLinks(float flexionStiffness, float flexionDamping) {
  numFlexion = ((rows - 2) * (columns - 2) * 2) + ((rows - 2) * 2) + ((columns - 2) * 2);
  flexionSprings = new Spring[numFlexion];
  int index = 0;

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (i < rows - 2 && j < columns - 2) {
        flexionSprings[index++] = Spring(flexionStiffness, flexionDamping, &particles[(i * columns) + j], &particles[(i * columns) + j + 2]);
        flexionSprings[index++] = Spring(flexionStiffness, flexionDamping, &particles[(i * columns) + j], &particles[((i + 2) * columns) + j]);
      }
      else if (i < rows && j < columns - 2) {
        flexionSprings[index++] = Spring(flexionStiffness, flexionDamping, &particles[(i * columns) + j], &particles[(i * columns) + j + 2]);
      }
      else if (j < columns && i < rows - 2) {
        flexionSprings[index++] = Spring(flexionStiffness, flexionDamping, &particles[(i * columns) + j], &particles[((i + 2) * columns) + j]);
      }
    }
  }
}