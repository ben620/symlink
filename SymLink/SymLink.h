
// SymLink.h : main header file for the SymLink application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
// CSymLinkApp:
// See SymLink.cpp for the implementation of this class
//

class CSymLinkApp : public CWinApp
{
public:
	CSymLinkApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CSymLinkApp theApp;
