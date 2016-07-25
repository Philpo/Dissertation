#pragma once
#include <fstream>
#include <windows.h>
#include <string>
#include <sstream>

using namespace std;

enum Integrator;
enum Scenario {
  SHEET = 0
, FLAG = 1
};

extern ofstream sheetDataFile, flagDataFile;
extern int testId, frameCount, averageFPS, numTimeFPSCalculated, updateCount;
extern double averageUpdateTime, averageRenderTime, timeStep;
extern Integrator currentIntegrator;
extern Scenario currentScenario;
extern bool unitTests;

/*
* Taken from https://stackoverflow.com/questions/1739259/how-to-use-queryperformancecounter
*/
HRESULT startCounter();

/*
* Taken from https://stackoverflow.com/questions/1739259/how-to-use-queryperformancecounter
*/
double getCounter();

template <class T>
T convertStringToNumber(const string& toConvert) {
  T r;
  stringstream(toConvert) >> r;
  return r;
}

// globally overload new and delete to ensure heap allocated memory is 16-byte alligned
// this is necessary to prevent crashes in release builds where SSE instructions will be used
// see https://stackoverflow.com/questions/20104815/warning-c4316-object-allocated-on-the-heap-may-not-be-aligned-16 for more information. Code taken from the same link
void* operator new(size_t i);
void* operator new[](size_t i);
void operator delete(void* p);
void operator delete[](void* p);