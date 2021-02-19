#pragma once
#include "wx/wx.h"

#include "FrameMain.h"

class App : public wxApp
{
public:
	App();
	~App();

private:
	FrameMain* m_Frame = nullptr;

public:
	virtual bool OnInit();
};