#pragma once
#include <fstream>
#include <windows.h>
#include <string>
#include <sstream>
#include <map>
#include <directxmath.h>

using namespace std;
using namespace DirectX;

class Particle;
typedef void (*IntegrationFunction)(Particle&, double);
enum Integrator;
enum Scenario {
  SHEET = 0
, FLAG = 1
};

struct UnitTestData {
  XMVECTOR totalForce, velocity, position;
};

extern ofstream sheetDataFile, flagDataFile;
extern int testId, frameCount, averageFPS, numTimeFPSCalculated, updateCount;
extern double averageUpdateTime, averageRenderTime, timeStep;
extern Integrator currentIntegrator;
extern Scenario currentScenario;
extern int** connections;
extern bool unitTests;
extern map<int, UnitTestData> unitTestData;

/*
* Taken from https://stackoverflow.com/questions/1739259/how-to-use-queryperformancecounter
*/
HRESULT startCounter();

/*
* Taken from https://stackoverflow.com/questions/1739259/how-to-use-queryperformancecounter
*/
double getCounter();

void loadUnitTestData(string filePath);

template <class T>
T convertStringToNumber(const string& toConvert) {
  T r;
  stringstream(toConvert) >> r;
  return r;
}
