#pragma once
#include "wx/wx.h"
#include "Query.h"

class Frame : public wxFrame
{
public:
	Frame();
	~Frame();

	void OnClick_ButtonStart(wxCommandEvent& e);
	void OnClick_ButtonStop(wxCommandEvent& e);

	void UpdateFrame();

public:
	wxStaticText* m_Text_Status = nullptr;
	wxStaticText* m_Text_DataRate = nullptr;
	wxButton* m_ButtonStart = nullptr;
	wxButton* m_ButtonStop = nullptr;

private:
	Query m_Query{ std::bind(&Frame::UpdateFrame, this) };

	std::string text_datarate{ "Data rate: " };
	std::string text_mbits{ " Mbit/s" };

	wxDECLARE_EVENT_TABLE();
};