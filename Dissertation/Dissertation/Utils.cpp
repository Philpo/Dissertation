#include "Utils.h"

double freq;
__int64 counterStart;
ofstream sheetDataFile, flagDataFile;
int testId, frameCount, averageFPS, numTimeFPSCalculated, updateCount;
double averageUpdateTime, averageRenderTime, timeStep;
Integrator currentIntegrator;
Scenario currentScenario;
int** connections;

/*
* Taken from https://stackoverflow.com/questions/1739259/how-to-use-queryperformancecounter
*/
HRESULT startCounter() {
  LARGE_INTEGER li;
  if (!QueryPerformanceFrequency(&li)) {
    return -1;
  }

  freq = double(li.QuadPart) / 1000.0;

  QueryPerformanceCounter(&li);
  counterStart = li.QuadPart;

  return S_OK;
}

/*
* Taken from https://stackoverflow.com/questions/1739259/how-to-use-queryperformancecounter
*/
double getCounter() {
  LARGE_INTEGER li;
  QueryPerformanceCounter(&li);
  return double(li.QuadPart - counterStart) / freq;
}