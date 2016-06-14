#pragma once
#include "Spring.h"
#include <vector>
#include <d3d11_1.h>

using namespace std;
using namespace DirectX;

class Cloth {
public:
  Cloth(FXMVECTOR topLeftPostition, float height, float width, int numRows, int numColumns, float totalMass, float structuralStiffness, float structuralDamping, float shearStiffness, float shearDamping, float flexionStiffness, float flexionDamping, float linearDamping);
  ~Cloth();

  const int getNumRows() const { return rows; }
  const int getNumColumns() const { return columns; }
  const int getNumStructuralSprings() const { return numStructural; }
  const int getNumShearSprings() const { return numShear; }
  const Particle* const getParticles() const { return particles; }

  void setPinned(int row, int column, bool pinned);

  void update(double deltaT);
  void draw(ID3D11DeviceContext* const immediateContext) const;
private:
  int rows, columns, numStructural, numShear, numFlexion;
  Particle* particles;
  Spring *structuralSprings, *shearSprings, *flexionSprings;
  static XMVECTOR GRAVITY;

  void createParticles(FXMVECTOR topLeftPostition, float height, float width, float totalMass, float linearDamping);
  void createStructuralLinks(float structuralStiffness, float structuralDamping);
  void createShearLinks(float shearStiffness, float shearDamping);
  void createFlexionLinks(float flexionStiffness, float flexionDamping);
};