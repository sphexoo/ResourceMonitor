#pragma once
#include <pdh.h>
#include <thread>
#include <functional>

class Query
{
public:
	Query(std::function<void(void)> fcn_ptr);
	~Query();

	void InitQuery();
	void RunQuery();
	void CloseQuery();
	void RunQueryThread();
	void StartWorker();
	void StopWorker();

	double GetData();

private:
	HQUERY m_Query = NULL;
	HCOUNTER m_Counter = NULL;

	double m_ReceivedBytes = 0.0;
	int m_WaitTime = 500;

	std::thread m_WorkerThread;
	bool m_IsRunning = false;
	std::function<void(void)> m_QueryReportFcn;
};