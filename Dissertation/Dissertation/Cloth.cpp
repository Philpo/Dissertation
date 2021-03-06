#include "Cloth.h"
#include <iostream>

XMVECTOR Cloth::GRAVITY = XMVectorSet(0.0f, -.981, 0.0f, 0.0f);

Cloth::Cloth(xml_node<>* clothParams) : timeSpentCalculatingInternalForce(0.0), timeSpentCalculatingExternalForce(0.0) {
  float x, y, z, height, width, mass, stiffness, damping;

  height = convertStringToNumber<float>(clothParams->first_attribute("height")->value());
  width = convertStringToNumber<float>(clothParams->first_attribute("width")->value());
  mass = convertStringToNumber<float>(clothParams->first_attribute("mass")->value());
  windConstant = convertStringToNumber<float>(clothParams->first_attribute("wind_constant")->value());

  xml_node<>* currentNode = clothParams->first_node("top_left_position");
  x = convertStringToNumber<float>(currentNode->first_attribute("x")->value());
  y = convertStringToNumber<float>(currentNode->first_attribute("y")->value());
  z = convertStringToNumber<float>(currentNode->first_attribute("z")->value());

  currentNode = clothParams->first_node("mesh_size");
  rows = convertStringToNumber<int>(currentNode->first_attribute("rows")->value());
  columns = convertStringToNumber<int>(currentNode->first_attribute("columns")->value());

  XMVECTOR topLeft = XMVectorSet(x, y, z, 0.0f);
  createParticles(topLeft, height, width, mass);

  currentNode = clothParams->first_node("structural");
  stiffness = convertStringToNumber<float>(currentNode->first_attribute("spring_coefficient")->value());
  damping = convertStringToNumber<float>(currentNode->first_attribute("damping_coefficient")->value());
  createStructuralLinks(stiffness, damping);

  currentNode = clothParams->first_node("shear");
  stiffness = convertStringToNumber<float>(currentNode->first_attribute("spring_coefficient")->value());
  damping = convertStringToNumber<float>(currentNode->first_attribute("damping_coefficient")->value());
  createShearLinks(stiffness, damping);

  currentNode = clothParams->first_node("flexion");
  stiffness = convertStringToNumber<float>(currentNode->first_attribute("spring_coefficient")->value());
  damping = convertStringToNumber<float>(currentNode->first_attribute("damping_coefficient")->value());
  createFlexionLinks(stiffness, damping);

  currentNode = clothParams->first_node("wind_direction");
  x = convertStringToNumber<float>(currentNode->first_attribute("x")->value());
  y = convertStringToNumber<float>(currentNode->first_attribute("y")->value());
  z = convertStringToNumber<float>(currentNode->first_attribute("z")->value());
  windDirection = XMVectorSet(x, y, z, 0.0f);
}

Cloth::Cloth(FXMVECTOR topLeftPostition, float height, float width, int numRows, int numColumns, float totalMass, float structuralStiffness, float structuralDamping, float shearStiffness, float shearDamping, float flexionStiffness, float flexionDamping) :
 timeSpentCalculatingInternalForce(0.0), timeSpentCalculatingExternalForce(0.0) {
  createParticles(topLeftPostition, height, width, totalMass);
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

void Cloth::calcForces() {
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

  currentTime = getCounter();

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      particles[(i * columns) + j].addForce(XMVectorScale(GRAVITY, particles[(i * columns) + j].getMass()));

      if (currentScenario == FLAG) {
        if (i < rows - 1 && j < columns - 1) {
          addWindForce(particles[(i * columns) + j], particles[(i * columns) + j + 1], particles[((i + 1) * columns) + j]);
          addWindForce(particles[((i + 1) * columns) + j], particles[(i * columns) + j + 1], particles[((i + 1) * columns) + j + 1]);
        }
      }
    }
  }

  timeSpentCalculatingExternalForce += getCounter() - currentTime;
}

void Cloth::draw(ID3D11DeviceContext* const immediateContext) const {
  immediateContext->DrawIndexed((numStructural + numShear) * 2, 0, 0);
}

void Cloth::createParticles(FXMVECTOR topLeftPostition, float height, float width, float totalMass) {
  particles = new Particle[rows * columns];

  XMFLOAT3 topLeft;
  XMStoreFloat3(&topLeft, topLeftPostition);

  float particleMass = totalMass / (float) (rows * columns);

  float x, y;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      x = topLeft.x + (j * (width / (float) columns));
      y = topLeft.y - (i * (height / (float) rows));
      particles[(i * columns) + j] = Particle(particleMass, XMVectorSet(x, y, topLeft.z, 1.0f));
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
        // horizontal spring
        structuralSprings[index++] = Spring(structuralStiffness, structuralDamping, &particles[(i * columns) + j], &particles[(i * columns) + j + 1]);
        // vertical spring
        structuralSprings[index++] = Spring(structuralStiffness, structuralDamping, &particles[(i * columns) + j], &particles[((i + 1) * columns) + j]);
      }
      else if (i == rows - 1 && j < columns - 1) {
        // only create horizontal springs for the bottom edge of the mesh
        structuralSprings[index++] = Spring(structuralStiffness, structuralDamping, &particles[(i * columns) + j], &particles[(i * columns) + j + 1]);
      }
      else if (j == columns - 1 && i < rows - 1) {
        // only create vertical springs for the right edge of the mesh
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
        // horizontal spring
        flexionSprings[index++] = Spring(flexionStiffness, flexionDamping, &particles[(i * columns) + j], &particles[(i * columns) + j + 2]);
        // vertical spring
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

/**
 * Code available for download at http://cg.alexandra.dk/?p=147 used as reference for addWindForce
 */
void Cloth::addWindForce(Particle& p1, Particle& p2, Particle& p3) {
  XMVECTOR pos1, pos2, pos3, temp1, temp2, normal, force;

  pos1 = p1.getPosition();
  pos2 = p2.getPosition();
  pos3 = p3.getPosition();

  temp1 = XMVectorSubtract(pos2, pos1);
  temp2 = XMVectorSubtract(pos3, pos1);

  normal = XMVector3Cross(temp1, temp2);
  temp1 = XMVector3Normalize(normal);

  force = XMVectorScale(normal, XMVectorScale(XMVector3Dot(temp1, windDirection), windConstant).m128_f32[0]);

  p1.addForce(force);
  p2.addForce(force);
  p3.addForce(force);
}