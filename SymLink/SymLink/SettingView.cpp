// SettingView.cpp : implementation file
//

#include "stdafx.h"
#include "SymLink.h"
#include "SettingView.h"
#define MIN_RES 4
#define MAX_RES 40
#define MIN_IMAGE 4
#define MAX_IMAGE MAX_RES

// CSettingView

IMPLEMENT_DYNCREATE(CSettingView, CFormView)

CSettingView::CSettingView()
	: CFormView(CSettingView::IDD)
	, m_nEditWidth(MIN_RES)
	, m_nEditHeight(MIN_RES)
	, m_nEditImage(8)
{

}

CSettingView::~CSettingView()
{
}

void CSettingView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_nEditWidth);
	DDV_MinMaxUInt(pDX, m_nEditWidth, MIN_RES, MAX_RES);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_nEditHeight);
	DDV_MinMaxUInt(pDX, m_nEditHeight, MIN_RES, MAX_RES);
	DDX_Text(pDX, IDC_EDIT_IMAGE, m_nEditImage);
	DDV_MinMaxUInt(pDX, m_nEditImage, MIN_IMAGE, MAX_IMAGE);
}

BEGIN_MESSAGE_MAP(CSettingView, CFormView)
	ON_BN_CLICKED(IDC_SET_OK, &CSettingView::OnBnClickedSetOk)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WIDTH, &CSettingView::OnDeltaposSpinWidth)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HEIGHT, &CSettingView::OnDeltaposSpinHeight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_IMAGE, &CSettingView::OnDeltaposSpinImage)
	ON_BN_CLICKED(IDC_SET_CANCEL, &CSettingView::OnBnClickedSetCancel)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSettingView diagnostics

#ifdef _DEBUG
void CSettingView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CSettingView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CSettingView message handlers


void CSettingView::OnBnClickedSetOk()
{
	UpdateData();
	ShowWindow(SW_HIDE);
	GetParent()->PostMessage(UM_SETTING_OK);
}


void CSettingView::OnDeltaposSpinWidth(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;
	m_nEditWidth -= 2*pNMUpDown->iDelta;
	if (m_nEditWidth > MAX_RES)
	{
		m_nEditWidth = MAX_RES;
	}
	if (m_nEditWidth < MIN_RES)
	{
		m_nEditWidth = MIN_RES;
	}
	UpdateData(FALSE);
}



BOOL CSettingView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void CSettingView::OnDeltaposSpinHeight(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	*pResult = 0;
	m_nEditHeight -= 2*pNMUpDown->iDelta;
	if (m_nEditHeight> MAX_RES)
	{
		m_nEditHeight = MAX_RES;
	}
	if (m_nEditHeight< MIN_RES)
	{
		m_nEditHeight = MIN_RES;
	}
	UpdateData(FALSE);
}


void CSettingView::OnDeltaposSpinImage(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;

	m_nEditImage -= pNMUpDown->iDelta;
	if (m_nEditImage> MAX_IMAGE)
	{
		m_nEditImage = MAX_IMAGE;
	}
	if (m_nEditImage< MIN_IMAGE)
	{
		m_nEditImage = MIN_IMAGE;
	}
	UpdateData(FALSE);
}


void CSettingView::OnBnClickedSetCancel()
{
	ShowWindow(SW_HIDE);
}


BOOL CSettingView::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	TRIVERTEX vertex[2] ;
	vertex[0].x     = 0;
	vertex[0].y     = 0;
	vertex[0].Red   = 0x6f00;
	vertex[0].Green = 0x9e00;
	vertex[0].Blue  = 0xe600;
	vertex[0].Alpha = 0x0000;

	vertex[1].x     = rc.Width();
	vertex[1].y     = rc.Height(); 
	vertex[1].Red   = 0x7f00;
	vertex[1].Green = 0xae00;
	vertex[1].Blue  = 0xff00;
	vertex[1].Alpha = 0x0000;

	// Create a GRADIENT_RECT structure that 
	// references the TRIVERTEX vertices. 
	GRADIENT_RECT gRect;
	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;

	// Draw a shaded rectangle. 
	pDC->GradientFill(vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_H);

	//pDC->FillSolidRect(0, 0, rc.Width(), rc.Height(), RGB(111, 158, 230));
	return TRUE;//CFormView::OnEraseBkgnd(pDC);
}


HBRUSH CSettingView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
	if (CTLCOLOR_STATIC == nCtlColor)
	{
		pDC->SetTextColor(RGB(0, 0, 43));
		pDC->SetBkColor(RGB(111, 158, 230));
		hbr = CreateSolidBrush(RGB(111, 158, 230));
		if (pWnd->GetDlgCtrlID() == IDC_STATIC)
		{
			pDC->SetTextColor(RGB(0, 0, 0));
		}
	}
	
	return hbr;
}
