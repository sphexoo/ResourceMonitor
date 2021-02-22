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
	m_Frame = new Frame();
	m_Frame->Show();
	return true;
}
