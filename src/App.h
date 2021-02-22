#pragma once
#include "wx/wx.h"

#include "Frame.h"

class App : public wxApp
{
public:
	App();
	~App();

private:
	Frame* m_Frame = nullptr;

public:
	virtual bool OnInit();
};