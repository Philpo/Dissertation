#include "Utils.h"

double freq;
__int64 counterStart;
ofstream sheetDataFile, flagDataFile;
int testId, frameCount, averageFPS, numTimeFPSCalculated, updateCount;
double averageUpdateTime, averageRenderTime, timeStep;
Integrator currentIntegrator;
Scenario currentScenario;
int** connections;
map<int, UnitTestData> unitTestData;

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

void loadUnitTestData(string filePath) {
  ifstream file(filePath);

  int counter, particleNumber = 1;
  string readLine, value;
  stringstream currentLine;

  while (!file.eof()) {
    UnitTestData data;
    counter = 0;
    file >> ws >> readLine >> ws;

    currentLine.clear();
    currentLine << readLine;

    while (currentLine.good()) {
      getline(currentLine, value, ',');

      switch (counter) {
        case 0:
          data.totalForce.m128_f32[0] = convertStringToNumber<float>(value);
          break;
        case 1:
          data.totalForce.m128_f32[1] = convertStringToNumber<float>(value);
          break;
        case 2:
          data.totalForce.m128_f32[2] = convertStringToNumber<float>(value);
          break;
        case 3:
          data.velocity.m128_f32[0] = convertStringToNumber<float>(value);
          break;
        case 4:
          data.velocity.m128_f32[1] = convertStringToNumber<float>(value);
          break;
        case 5:
          data.velocity.m128_f32[2] = convertStringToNumber<float>(value);
          break;
        case 6:
          data.position.m128_f32[0] = convertStringToNumber<float>(value);
          break;
        case 7:
          data.position.m128_f32[1] = convertStringToNumber<float>(value);
          break;
        case 8:
          data.position.m128_f32[2] = convertStringToNumber<float>(value);
          break;
      }

      counter++;
    }

    unitTestData.insert(pair<int, UnitTestData>(particleNumber, data));

    particleNumber = particleNumber == 1 ? 3 : particleNumber + 1;
  }
}