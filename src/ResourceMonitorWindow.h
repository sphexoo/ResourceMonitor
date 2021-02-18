#include <windows.h>
#include <pdh.h>
#include <thread>

#include "BaseWindow.h"

class ResourceMonitorWindow : public BaseWindow<ResourceMonitorWindow>
{
public:
    ResourceMonitorWindow();
    ~ResourceMonitorWindow();

    PCWSTR  GetName() const { return L"ResourceMonitorWindow Class"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    
    void OnRBUTTONUP();
    void OnLBUTTONUP();
    void OnPAINT();

    void InitQuery();
    void RunQuery();
    void CloseQuery();

    void RunQueryRepeat();

    void StartWorker();
    void StopWorker();

private:
    HQUERY m_Query = NULL;
    HCOUNTER m_Counter = NULL;
    RECT m_Rect;

    double m_ReceivedBytes = 0.0;
    int m_WaitTime = 500;

    std::thread m_WorkerThread;
    bool m_IsRunning = false;
};