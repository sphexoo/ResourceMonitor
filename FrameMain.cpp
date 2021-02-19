#include "FrameMain.h"

wxBEGIN_EVENT_TABLE(FrameMain, wxFrame)
	EVT_BUTTON(10002, OnClick_ButtonStart)
	EVT_BUTTON(10003, OnClick_ButtonStop)
wxEND_EVENT_TABLE()

FrameMain::FrameMain() : wxFrame(nullptr, wxID_ANY, "Resource Monitor", wxPoint(100, 100), wxSize(300, 400))
{
	m_List = new wxListBox(this, 10001, wxPoint(10 ,10), wxSize(250, 240));
	m_ButtonStart = new wxButton(this, 10002, "Start", wxPoint(10, 260), wxSize(80, 30));
	m_ButtonStop = new wxButton(this, 10003, "Stop", wxPoint(90, 260), wxSize(80, 30));

	m_Query.InitQuery();

}

FrameMain::~FrameMain()
{
	m_Query.StopWorker();
	m_Query.CloseQuery();
}


void FrameMain::OnClick_ButtonStart(wxCommandEvent& e)
{
	m_Query.StartWorker();
	m_List->AppendString("Start");

	e.Skip();
}

void FrameMain::OnClick_ButtonStop(wxCommandEvent& e)
{
	m_Query.StopWorker();
	m_List->AppendString("Stop");
	e.Skip();
}

void FrameMain::UpdateFrame()
{
	double data = m_Query.GetData();
	m_List->AppendString(std::to_string(data));
}