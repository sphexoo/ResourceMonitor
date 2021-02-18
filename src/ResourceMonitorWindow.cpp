#include "ResourceMonitorWindow.h"
#include <shobjidl.h> 
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <pdhmsg.h>
#include <iostream>
#include <string>

#include <sstream>

std::wstring to_wstring_precision(double n, int prec)
{
    std::wstringstream out;
    out.precision(prec);
    out << std::fixed << n;
    return out.str();
}


void ManuallyQueryResourceCounters()
{
    PDH_STATUS Status;
    HQUERY Query = NULL;
    HCOUNTER Counter;
    PDH_FMT_COUNTERVALUE DisplayValue;
    DWORD CounterType;
    PDH_BROWSE_DLG_CONFIG BrowseDlgData;
    WCHAR CounterPathBuffer[PDH_MAX_COUNTER_PATH];

    //
    // Create a query.
    //

    Status = PdhOpenQuery(NULL, NULL, &Query);
    if (Status != ERROR_SUCCESS)
    {
        wprintf(L"\nPdhOpenQuery failed with status 0x%x.", Status);
        goto Cleanup;
    }

    //
    // Initialize the browser dialog window settings.
    //

    ZeroMemory(&CounterPathBuffer, sizeof(CounterPathBuffer));
    ZeroMemory(&BrowseDlgData, sizeof(PDH_BROWSE_DLG_CONFIG));

    BrowseDlgData.bIncludeInstanceIndex = FALSE;
    BrowseDlgData.bSingleCounterPerAdd = TRUE;
    BrowseDlgData.bSingleCounterPerDialog = TRUE;
    BrowseDlgData.bLocalCountersOnly = FALSE;
    BrowseDlgData.bWildCardInstances = TRUE;
    BrowseDlgData.bHideDetailBox = TRUE;
    BrowseDlgData.bInitializePath = FALSE;
    BrowseDlgData.bDisableMachineSelection = FALSE;
    BrowseDlgData.bIncludeCostlyObjects = FALSE;
    BrowseDlgData.bShowObjectBrowser = FALSE;
    BrowseDlgData.hWndOwner = NULL;
    BrowseDlgData.szReturnPathBuffer = CounterPathBuffer;
    BrowseDlgData.cchReturnPathLength = PDH_MAX_COUNTER_PATH;
    BrowseDlgData.pCallBack = NULL;
    BrowseDlgData.dwCallBackArg = 0;
    BrowseDlgData.CallBackStatus = ERROR_SUCCESS;
    BrowseDlgData.dwDefaultDetailLevel = PERF_DETAIL_WIZARD;
    BrowseDlgData.szDialogBoxCaption = NULL;

    //
    // Display the counter browser window. The dialog is configured
    // to return a single selection from the counter list.
    //

    Status = PdhBrowseCounters(&BrowseDlgData);

    if (Status != ERROR_SUCCESS)
    {
        if (Status == PDH_DIALOG_CANCELLED)
        {
            OutputDebugString(L"Error1");//wprintf(L"\nDialog canceled by user.");
        }
        else
        {
            OutputDebugString(L"Error2");//wprintf(L"\nPdhBrowseCounters failed with status 0x%x.", Status);
        }
        goto Cleanup;
    }
    else if (wcslen(CounterPathBuffer) == 0)
    {
        OutputDebugString(L"Error3");//wprintf(L"\nUser did not select any counter.");
        goto Cleanup;
    }
    else
    {
        OutputDebugString(L"Counter selected\n");//wprintf(L"\nCounter selected: %s\n", CounterPathBuffer);
    }

    //
    // Add the selected counter to the query.
    //
    
    Status = PdhAddCounter(Query, CounterPathBuffer, 0, &Counter);
    if (Status != ERROR_SUCCESS)
    {
        OutputDebugString(L"Error5");//wprintf(L"\nPdhAddCounter failed with status 0x%x.", Status);
        goto Cleanup;
    }

    //
    // Most counters require two sample values to display a formatted value.
    // PDH stores the current sample value and the previously collected
    // sample value. This call retrieves the first value that will be used
    // by PdhGetFormattedCounterValue in the first iteration of the loop
    // Note that this value is lost if the counter does not require two
    // values to compute a displayable value.
    //

    Status = PdhCollectQueryData(Query);
    if (Status != ERROR_SUCCESS)
    {
        OutputDebugString(L"Error6");//wprintf(L"\nPdhCollectQueryData failed with 0x%x.\n", Status);
        goto Cleanup;
    }

    //
    // Print counter values until a key is pressed.
    //

    while (!_kbhit())
    {
        Sleep(1000);

        Status = PdhCollectQueryData(Query);
        if (Status != ERROR_SUCCESS)
        {
            OutputDebugString(L"Error7");//wprintf(L"\nPdhCollectQueryData failed with status 0x%x.", Status);
        }

        //
        // Compute a displayable value for the counter.
        //

        Status = PdhGetFormattedCounterValue(Counter,
            PDH_FMT_DOUBLE,
            &CounterType,
            &DisplayValue);
        if (Status != ERROR_SUCCESS)
        {
            OutputDebugString(L"Error8");//wprintf(L"\nPdhGetFormattedCounterValue failed with status 0x%x.", Status);
            goto Cleanup;
        }

        std::wstring data = std::to_wstring(DisplayValue.doubleValue);

        data += L"\n";
        OutputDebugString(data.c_str());//wprintf(L",\"%.20g\"", DisplayValue.doubleValue);
    }

Cleanup:

    //
    // Close the query.
    //

    if (Query)
    {
        PdhCloseQuery(Query);
    }
}


ResourceMonitorWindow::ResourceMonitorWindow()
{
    InitQuery();
}

ResourceMonitorWindow::~ResourceMonitorWindow()
{
    CloseQuery();
}

LRESULT ResourceMonitorWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        OnPAINT();
        return 0;
    }
    case WM_RBUTTONUP:
    {
        OnRBUTTONUP();
        return 0;
    }
    case WM_LBUTTONUP:
    {
        OnLBUTTONUP();
        return 0;
    }
    case WM_CLOSE:
    {
        BOOL ok = MessageBox(m_hwnd, L"Press OK to quit", L"Quit", MB_OKCANCEL);
        if (ok)
        {
            StopWorker();
            DestroyWindow(m_hwnd);
        }
        return 0;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    default:
    {
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    }
    return TRUE;
}

void ResourceMonitorWindow::OnRBUTTONUP()
{
    StopWorker();
}

void ResourceMonitorWindow::OnLBUTTONUP()
{
    StartWorker();
}

void ResourceMonitorWindow::StartWorker()
{
    if (!m_IsRunning)
    {
        m_IsRunning = true;
        m_WorkerThread = std::thread{ [this] { this->RunQueryRepeat(); } };
    }
}

void ResourceMonitorWindow::StopWorker()
{
    if (m_IsRunning)
    {
        m_IsRunning = false;
        m_WorkerThread.join();
    }
}

void ResourceMonitorWindow::OnPAINT()
{
    RECT rect;
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hwnd, &ps);

    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
    GetClientRect(m_hwnd, &rect);

    SetTextColor(hdc, RGB(0x00, 0x00, 0x00));
    SetBkMode(hdc, TRANSPARENT);
    rect.left = 40;
    rect.top = 10;
    
    std::wstring rate_KBs = to_wstring_precision(m_ReceivedBytes / 1000.0, 4);
    std::wstring rate_Mbits = to_wstring_precision(m_ReceivedBytes * 8 / 1000000.0, 4);
    std::wstring text_KBs = rate_KBs + std::wstring(L" KB/s\n") + rate_Mbits + std::wstring(L" Mbit/s");

    DrawText(hdc, text_KBs.c_str(), -1, &rect, DT_NOCLIP | DT_RIGHT);
    
    EndPaint(m_hwnd, &ps);
}


void ResourceMonitorWindow::InitQuery()
{
    if (m_Query)
        return;

    PDH_STATUS status;

    status = PdhOpenQuery(NULL, NULL, &m_Query);
    if (status != ERROR_SUCCESS)
    {
        OutputDebugString(L"\n[ERROR]: PdhOpenQuery failed.");
        CloseQuery();
        return;
    }

    std::wstring counterPath = L"\\Netzwerkschnittstelle(*)\\Empfangene Bytes/s";

    status = PdhAddCounter(m_Query, counterPath.c_str(), 0, &m_Counter);
    if (status != ERROR_SUCCESS)
    {
        OutputDebugString(L"\n[ERROR]: PdhAddCounter failed.");
        CloseQuery();
        return;
    }
    
    status = PdhCollectQueryData(m_Query);
    if (status != ERROR_SUCCESS)
    {
        OutputDebugString(L"\n[ERROR]: PdhCollectQueryData");
        CloseQuery();
        return;
    }
}

void ResourceMonitorWindow::RunQuery()
{
    PDH_STATUS status;
    PDH_FMT_COUNTERVALUE DisplayValue;
    DWORD CounterType;

    status = PdhCollectQueryData(m_Query);
    if (status != ERROR_SUCCESS)
    {
        OutputDebugString(L"\n[ERROR]: PdhCollectQueryData");
        CloseQuery();
        return;
    }

    status = PdhGetFormattedCounterValue(m_Counter, PDH_FMT_DOUBLE, &CounterType, &DisplayValue);
    if (status != ERROR_SUCCESS)
    {
        OutputDebugString(L"\n[ERROR]: PdhGetFormattedCounterValue failed");
        CloseQuery();
        return;
    }

    m_ReceivedBytes = DisplayValue.doubleValue;

    RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
}

void ResourceMonitorWindow::CloseQuery()
{
    if (m_Query)
    {
        PdhCloseQuery(m_Query);
        m_Query = NULL;
    }
}

void ResourceMonitorWindow::RunQueryRepeat()
{
    while (m_IsRunning)
    {
        RunQuery();
        Sleep(m_WaitTime);
    }
}