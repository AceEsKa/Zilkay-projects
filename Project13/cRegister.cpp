#include "Includes.h"
#include "Registration.h"

void cRegisterNew::Registration(wxCommandEvent& evt)
{

	{
		wxString str = wx_Password->GetValue();
		int num;
		num = wxAtoi(str);

		wxString mystring = wx_UserName->GetValue();
		std::string Buffer = std::string(mystring.mb_str());

		unsigned int password1 = num;
		//check if username is available
		if (1)
		{

		}

		//check if password size matches 8
		if (sizeof(password1) / sizeof(unsigned int) == 8)
		{
			int num2;
			str = wx_RepeatPassword->GetValue();
			num2 = wxAtoi(str);
			unsigned int password2 = num2;
			//check if password1 is the same as password1
			if (num == num2)
			{
				//
				//B_registration = false;
			}
			else
			{
				wxStaticText* failed = new wxStaticText(this, wxID_ANY, "Usare Name:", wxPoint(x / 2 - 50 - 80, y / 2 - 28), wxSize(160, 40));
			}
		}
	}
}