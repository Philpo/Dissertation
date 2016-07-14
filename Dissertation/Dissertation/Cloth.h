#pragma once
#include "Spring.h"
#include <rapidxml.hpp>
#include <vector>
#include <d3d11_1.h>

class VerletIntegrator;
class ExplicitEulerIntegrator;
class FourthOrderRungeKuttaIntegrator;
class FourthOrderRungeKutta;

using namespace std;
using namespace DirectX;
using namespace rapidxml;

class Cloth {
  friend class VerletIntegrator;
  friend class ExplicitEulerIntegrator;
  friend class FourthOrderRungeKuttaIntegrator;
  friend class FourthOrderRungeKutta;
public:
  Cloth(xml_node<>* clothParams);
  Cloth(FXMVECTOR topLeftPostition, float height, float width, int numRows, int numColumns, float totalMass, float structuralStiffness, float structuralDamping, float shearStiffness, float shearDamping, float flexionStiffness, float flexionDamping, float linearDamping);
  ~Cloth();

  const double getTimeSpentCalculatingInternalForce() const { return timeSpentCalculatingInternalForce; }
  const double getTimeSpentIntegrating() const { return timeSpentIntegrating; }
  const double getTimeTakenToReachEquilibrium() const { return timeAtEquilibrium - timeAtStart; }
  const bool reachedEquilibrium() const { return equilibrium; }
  const int getNumRows() const { return rows; }
  const int getNumColumns() const { return columns; }
  const int getNumStructuralSprings() const { return numStructural; }
  const int getNumShearSprings() const { return numShear; }
  const Particle* const getParticles() const { return particles; }

  void setPinned(int row, int column, bool pinned);
  void setIntegrator(IIntegrator* const integrator);
  void setIntegrator(IntegrationFunction integrator);

  void calcForces();
  void update(double deltaT);
  void draw(ID3D11DeviceContext* const immediateContext) const;
private:
  double timeSpentCalculatingInternalForce, timeSpentIntegrating, timeAtStart, timeAtEquilibrium;
  int rows, columns, numStructural, numShear, numFlexion;
  float windConstant;
  XMVECTOR windDirection;
  bool equilibrium;
  Particle* particles;
  Spring *structuralSprings, *shearSprings, *flexionSprings;
  static XMVECTOR GRAVITY;

  void createParticles(FXMVECTOR topLeftPostition, float height, float width, float totalMass, float linearDamping);
  void createStructuralLinks(float structuralStiffness, float structuralDamping);
  void createShearLinks(float shearStiffness, float shearDamping);
  void createFlexionLinks(float flexionStiffness, float flexionDamping);
  void addWindForce(Particle& p1, Particle& p2, Particle& p3);
};