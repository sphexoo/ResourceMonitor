#pragma once
#include "wx/wx.h"
#include "Query.h"

class FrameMain : public wxFrame
{
public:
	FrameMain();
	~FrameMain();

	void OnClick_ButtonStart(wxCommandEvent& e);
	void OnClick_ButtonStop(wxCommandEvent& e);

	void UpdateFrame();

public:
	wxListBox* m_List = nullptr;
	wxButton* m_ButtonStart = nullptr;
	wxButton* m_ButtonStop = nullptr;

private:
	Query m_Query{ std::bind(&FrameMain::UpdateFrame, this) };

	wxDECLARE_EVENT_TABLE();
};