#include "Application.h"
#include <windowsx.h>
#define NOMINMAX

double timeLastFrame, freq, averageUpdateTime, averageRenderTime;
__int64 counterStart;
int frameCount;

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

    std::wostringstream outs;
    outs.precision(6);
    outs << L"Cork    " << L"FPS: " << fps << L"    " << L"Frame Time: " << mspf << L" (ms)";
    app->setWindowCaption(outs);

    frameCnt = 0;
    timeElapsed += 1000.0;
  }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

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
  frameCount = 0;

  if (FAILED(theApp->initialise(hInstance, nCmdShow))) {
    return -1;
  }

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
      timeLastFrame = currentTime;
      frameCount++;
    }
  }

  delete theApp;
  theApp = nullptr;

  cout << "Average update time: " << (averageUpdateTime / (double) frameCount) << "ms" << endl;
  cout << "Average render time: " << (averageRenderTime / (double) frameCount) << "ms" << endl;

  timeEndPeriod(wTimerRes);
  system("PAUSE");
  return (int) msg.wParam;
}