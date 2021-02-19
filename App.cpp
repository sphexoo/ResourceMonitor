#include "App.h"

wxIMPLEMENT_APP(App);

App::App()
{

}

App::~App()
{

}

bool App::OnInit()
{
	m_Frame = new FrameMain();
	m_Frame->Show();
	return true;
}
