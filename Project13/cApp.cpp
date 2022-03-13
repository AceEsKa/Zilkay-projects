#include "cApp.h"

wxIMPLEMENT_APP(cApp); //generate "main" function

cApp::cApp()
{
};

cApp::~cApp()
{
};

bool cApp::OnInit()
{
	m_frame1 = new cLogin();
	m_frame1->Show();
	return true;
}