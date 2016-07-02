#include "Application.h"
#include "Utils.h"
#include <windowsx.h>
#include <rapidxml_utils.hpp>
#define NOMINMAX

double timeLastFrame, simulationRunningTime = 0.0;

const double MAX_SIMULATION_TIME = 60000.0;

/*
* Taken from Frank Luna: 3D Game Programming with DirectX 11
*/
void calculateFrameRateStats(Application* const app) {
  static int frameCnt = 0;
  static double timeElapsed = 0.0;

  frameCnt++;

  if (getCounter() - timeElapsed >= 1.0f) {
    float fps = (float) frameCnt;
    float mspf = 1000.0f / fps;

    std::stringstream outs;
    outs.precision(6);
    outs << "Cork    " << "FPS: " << fps << "    " << "Frame Time: " << mspf << " (ms)";
    app->setWindowCaption(outs);

    averageFPS += frameCnt;
    numTimeFPSCalculated++;
    frameCnt = 0;
    timeElapsed += 1000.0;
  }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  if (__argc != 3) {
    return -1;
  }

  file<> file(__argv[2]);
  xml_document<> doc;
  doc.parse<0>(file.data());
  xml_node<>* rootNode = doc.first_node();
  xml_node<>* currentTest = rootNode->first_node();
  Scenario currentScenario = SHEET;

  // timer resolution code taken from https://msdn.microsoft.com/en-us/library/windows/desktop/dd743626%28v=vs.85%29.aspx
  TIMECAPS tc;
  UINT wTimerRes;

  if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) {
    std::cerr << "error getting timer capabilities" << std::endl;
    return -1;
  }

  // set the timer resolution to the lowest possible, so that we can sleep the thread as accurately as possible
  wTimerRes = min(max(tc.wPeriodMin, 1), tc.wPeriodMax);
  timeBeginPeriod(wTimerRes);

  Application* theApp = new Application();
  timeLastFrame = 0.0;
  averageUpdateTime = averageRenderTime = 0.0;
  frameCount = averageFPS = numTimeFPSCalculated = updateCount = 0;

  if (FAILED(theApp->initialise(hInstance, nCmdShow))) {
    return -1;
  }

  if (FAILED(theApp->loadTest(currentTest, currentScenario))) {
    return -1;
  }

  sheetDataFile.open("sheet_test_data.csv");
  flagDataFile.open("flag_test_data.csv");
  sheetDataFile << "Test ID, Integrator, Rows, Columns, Time Step, Average time calcSpringForce (ms), Average time integrate (ms), Average time updating (ms), Average rendering time (ms), Average FPS" << endl;
  flagDataFile << "Test ID, Integrator, Rows, Columns, Time Step, Average time calcSpringForce (ms), Average time integrate (ms), Average time updating (ms), Average rendering time (ms), Average FPS" << endl;

  // Main message loop
  MSG msg = { 0 };

  startCounter();

  while (WM_QUIT != msg.message) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      bool handled = false;

      if (msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST) {
        handled = theApp->handleKeyboard(msg);
      }
      else if (msg.message == WM_LBUTTONDOWN) {
        theApp->handleMouseClick(msg.wParam, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
      }
      else if (msg.message == WM_MOUSEMOVE) {
        theApp->handleMouseMovement(msg.wParam, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
      }
      else if (WM_QUIT == msg.message)
        break;

      if (!handled) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
    else {
      calculateFrameRateStats(theApp);
      double currentTime = getCounter();
      double deltaT = currentTime - timeLastFrame;
      double time = getCounter();
      theApp->update(deltaT);
      averageUpdateTime += getCounter() - time;
      time = getCounter();
      theApp->draw();
      averageRenderTime += getCounter() - time;
      simulationRunningTime += getCounter() - currentTime;
      timeLastFrame = currentTime;
      frameCount++;

      if ((theApp->reachedEquilibrium() && frameCount > 0))  {
        switch (currentScenario) {
          case SHEET:
            currentScenario = FLAG;
            //sheetDataFile << ", " << (averageUpdateTime / (double) frameCount) << ", " << (averageRenderTime / (double) frameCount) << endl;
            break;
          case FLAG:
            currentScenario = SHEET;
            currentTest = currentTest->next_sibling();
            //flagDataFile << ", " << (averageUpdateTime / (double) frameCount) << ", " << (averageRenderTime / (double) frameCount) << endl;
            break;
        }

        if (currentTest) {
          theApp->loadTest(currentTest, currentScenario);
        }
        else {
          break;
        }

        averageUpdateTime = averageRenderTime = 0.0f;
        frameCount = 0;
        simulationRunningTime = 0.0;

        timeLastFrame = getCounter();
      }
    }
  }

  delete theApp;
  theApp = nullptr;

  cout << "Average update time: " << (averageUpdateTime / (double) frameCount) << "ms" << endl;
  cout << "Average render time: " << (averageRenderTime / (double) frameCount) << "ms" << endl;

  //flagDataFile << ", " << (averageUpdateTime / (double) frameCount) << ", " << (averageRenderTime / (double) frameCount) << endl;
  sheetDataFile.close();
  flagDataFile.close();

  timeEndPeriod(wTimerRes);
  system("PAUSE");
  return (int) msg.wParam;
}