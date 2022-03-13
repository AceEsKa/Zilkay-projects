#pragma once
#include "Includes.h"

class cLogin : public wxFrame
{
private:
	
public:
	cLogin();
	~cLogin();
	wxButton *wx_Login = nullptr;
	wxButton *wx_Register = nullptr;
	wxTextCtrl *wx_UserName = nullptr;
	wxTextCtrl *wx_Password = nullptr;
	wxTextCtrl *wx_RepeatPassword = nullptr;
	wxTextCtrl *wx_MyMessage = nullptr;
	wxStaticText *UserName = nullptr;
	wxStaticText *Password = nullptr;
	wxStaticText *Repeat = nullptr;
	wxListBox *wx_LBox = nullptr;
	wxStaticText *Text = nullptr;

	void LoginButton(wxCommandEvent& evt);
	void RegistrationButton(wxCommandEvent& evt);
	void Registration(wxCommandEvent& evt);


	wxDECLARE_EVENT_TABLE();

};

