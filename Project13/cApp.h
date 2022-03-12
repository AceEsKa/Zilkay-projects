#pragma once
#include "Includes.h"
class cApp : public wxApp
{
private:
	cLogin* m_frame1 = nullptr;
public:
	cApp();
	~cApp();

	virtual bool OnInit();
};

