#include "Frame.h"

#include <sstream>

wxBEGIN_EVENT_TABLE(Frame, wxFrame)
	EVT_BUTTON(10002, OnClick_ButtonStart)
	EVT_BUTTON(10003, OnClick_ButtonStop)
wxEND_EVENT_TABLE()

template <typename T>
std::string to_string_prec(T value, int n = 3)
{
	std::ostringstream out;
	out.precision(n);
	out << std::fixed << value;
	return out.str();
}

Frame::Frame() : wxFrame(nullptr, wxID_ANY, "Resource Monitor", wxPoint(100, 100), wxSize(300, 340))
{
	m_Text_Status = new wxStaticText(this, 10010, "Paused.", wxPoint(190, 265), wxSize(50, 50));
	m_Text_DataRate = new wxStaticText(this, 10020, text_datarate, wxPoint(10, 30), wxSize(50, 50));
	m_ButtonStart = new wxButton(this, 10002, "Start", wxPoint(10, 260), wxSize(80, 30));
	m_ButtonStop = new wxButton(this, 10003, "Stop", wxPoint(90, 260), wxSize(80, 30));
}

Frame::~Frame()
{

}


void Frame::OnClick_ButtonStart(wxCommandEvent& e)
{
	m_Query.StartWorker();
	m_Text_Status->SetLabel("Running...");
	e.Skip();
}

void Frame::OnClick_ButtonStop(wxCommandEvent& e)
{
	m_Query.StopWorker();
	m_Text_Status->SetLabel("Paused.");
	e.Skip();
}

void Frame::UpdateFrame()
{
	double data = m_Query.GetData();
	data = data * 8 / 1000000;
	m_Text_DataRate->SetLabel(text_datarate + to_string_prec(data) + text_mbits);
}
