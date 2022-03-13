#include "Includes.h"
#include "cApp.h"
#include "Registration.h"


//EVENT
wxBEGIN_EVENT_TABLE(cLogin,wxFrame)
	EVT_BUTTON(1000, LoginButton)
	EVT_BUTTON(1001, RegistrationButton)
	EVT_BUTTON(1002, Registration)
wxEND_EVENT_TABLE()



cLogin::cLogin() : wxFrame(nullptr, 3000, "Aces chat room.", wxPoint(90, 90), wxSize(x, y),wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX)
{
	//B_registration = false;
	delete wx_Register;
	delete wx_UserName;
	delete wx_Login;
	delete wx_RepeatPassword;
	wx_RepeatPassword = nullptr;

	UserName=new wxStaticText(this, wxID_ANY,"Usare Name:",wxPoint(x / 2 - 50-80, y / 2 - 28),wxSize(160,30));
	Password=new wxStaticText(this, wxID_ANY, "Password:", wxPoint(x / 2 - 50 - 80+15, y / 2), wxSize(160, 30));
	wx_Login = new wxButton(this,1000, "Login", wxPoint(x / 2-35-20-20,340),wxSize(70,30), wxBU_EXACTFIT);
	wx_Register = new wxButton(this, 1001, "Register", wxPoint(x / 2 +10, 340), wxSize(70, 30), wxBU_EXACTFIT);
	wx_UserName = new wxTextCtrl(this,2000,"",wxPoint(x / 2 - 50, y / 2 - 30),wxSize(100,20));
	wx_Password = new wxTextCtrl(this, 2001, "", wxPoint(x / 2-50, y/2), wxSize(100, 20));
}

cLogin::~cLogin()
{
	
}

void cLogin::LoginButton(wxCommandEvent& evt)
{
	wxFrame* child = nullptr;

	wxString mystring= wx_UserName->GetValue();
	std::string Buffer = std::string(mystring.mb_str());
	wxString str = wx_Password->GetValue();
	int num;
	num = wxAtoi(str);

	unsigned int password=num;
	//do while loop
		//Connect and send UserName+Password to Authetification Server-TCP
			//get answeer
	//chceck whether server answered with credentials correct 
		//if yes do while loop condition is false exiting
		//else 
	
	//close connection with Authentification server-TCP
	
	//establish connection with Chat Room Server-UDP
		
	//close
	this->Close();

	child = new wxFrame(nullptr, wxID_ANY, "Welcome to Aces chat room!", wxPoint(30, 10), wxSize(1000, 840), wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX);
	{
		child->Show();
		wx_LBox = new wxListBox(child, 4000, wxPoint(5, 5), wxSize(1000-25,840-120));
		wx_MyMessage = new wxTextCtrl(child, 4001, "Message", wxPoint(5,750) , wxSize(1000-25, 40), wxTE_PROCESS_ENTER);
		//send message
	}
	//recieve answer

}

void cLogin::RegistrationButton(wxCommandEvent& evt)
{
	delete wx_Register;
	delete wx_UserName;
	delete wx_Login;
	wx_Login = nullptr;

	Repeat = new wxStaticText(this, wxID_ANY, "Repeat Password:", wxPoint(x / 2 - 50-130+20+5,y/2+30), wxSize(160, 30));
	wx_Register = new wxButton(this, 1002, "Register", wxPoint(x / 2 - 35, y/2+60), wxSize(70, 30), wxBU_EXACTFIT);
	wx_UserName = new wxTextCtrl(this, 2000, "", wxPoint(x / 2 - 50, y / 2 - 30), wxSize(100, 20));
	wx_RepeatPassword = new wxTextCtrl(this, 2001, "", wxPoint(x / 2 - 50, y / 2+30), wxSize(100, 20));
}

#include "Includes.h"
#include "Registration.h"

void cLogin::Registration(wxCommandEvent& evt)
{
		wxString str = wx_Password->GetValue();
		int num;
		num = wxAtoi(str);

		wxString mystring = wx_UserName->GetValue();
		std::string Buffer = std::string(mystring.mb_str());

		unsigned int password1 = num;
		//check if username is available
			//connect to server
			
		//check if password size matches 8
		wxDialog* dlg = nullptr;
		wxStaticText* failed = nullptr;
		int size = log10(password1)+1;
		if (Buffer.length() != 0 /*&& */)
		{
			if (size == 8)
			{
				int num2;
				str = wx_RepeatPassword->GetValue();
				num2 = wxAtoi(str);
				unsigned int password2 = num2;
				//check if password1 is the same as password1
				if (num == num2)
				{
					//
					//bool B_registration = false;
					delete Text;
					Text = new wxStaticText(this, wxID_ANY, "Success", wxPoint(), wxSize(160, 40));
					//return to login screen
				}
				else
				{
					delete Text;
					//dialog window
					Text = new wxStaticText(this, wxID_ANY, "Failed2", wxPoint(), wxSize(160, 40));
				}
			}
			else
			{
				delete Text;
				wxString myString = wxString::Format(wxT("%i"), size);
				//dialog window
				Text = new wxStaticText(this, wxID_ANY, myString, wxPoint(), wxSize(160, 40));
			}
		}
		else
		{
			delete Text;
			//dialog window
			dlg = new wxDialog(this, wxID_ANY, "" , wxPoint(30,30),wxSize(100,100), wxDEFAULT_DIALOG_STYLE);
			{
				dlg->ShowModal();
			}
			Text = new wxStaticText(this, wxID_ANY, "Failed3", wxPoint(), wxSize(160, 40));
		}
		//delete dlg;
}
