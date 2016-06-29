#pragma once
#include <fstream>
#include <windows.h>
#include <string>
#include <sstream>

using namespace std;

class Particle;
typedef void (*IntegrationFunction)(Particle&, double);

extern ofstream sheetDataFile, flagDataFile;
extern int testId, frameCount, averageFPS, numTimeFPSCalculated;
extern double averageUpdateTime, averageRenderTime;

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