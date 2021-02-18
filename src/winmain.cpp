#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include "ResourceMonitorWindow.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    ResourceMonitorWindow win;

    BOOL success = win.Create(L"ResourceMonitor", WS_OVERLAPPEDWINDOW, 0,
                                CW_USEDEFAULT, CW_USEDEFAULT,
                                640, 480,
                                0, 0);
    if (!success)
        return 0;

    ShowWindow(win.Window(), nCmdShow);

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr))
        return -1;

    // Run the message loop.
    MSG msg{};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize();
    return 0;
}