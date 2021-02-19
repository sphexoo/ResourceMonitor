#include <string>
#include "Query.h"


Query::Query(std::function<void(void)> fcn_ptr)
    : m_QueryReportFcn(fcn_ptr)
{

}

Query::~Query()
{

}

void Query::StartWorker()
{
    if (!m_IsRunning)
    {
        m_IsRunning = true;
        m_WorkerThread = std::thread{ [this] { this->RunQueryThread(); } };
    }
}

void Query::StopWorker()
{
    if (m_IsRunning)
    {
        m_IsRunning = false;
        m_WorkerThread.join();
    }
}


void Query::InitQuery()
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

void Query::RunQuery()
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
    m_QueryReportFcn();
}

void Query::CloseQuery()
{
    if (m_Query)
    {
        PdhCloseQuery(m_Query);
        m_Query = NULL;
    }
}

void Query::RunQueryThread()
{
    while (m_IsRunning)
    {
        RunQuery();
        Sleep(m_WaitTime);
    }
}

double Query::GetData()
{
    return m_ReceivedBytes;
}