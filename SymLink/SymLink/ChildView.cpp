
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "SymLink.h"
#include "ChildView.h"
#include <MMSystem.h>

#pragma comment(lib, "Winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace symbollink;
// CChildView

CChildView::CChildView()
	: m_bClicked(false)
	, m_posLast(-1, -1)
	, m_posMouseOver(-1, -1)
	, m_busy(0)
	, m_pWndSetting(0)
{
	m_symbol.Init();
	m_timeStart = CTime::GetTickCount();
	m_enumGameState = GAME_RUN;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_RBTN_RESTART, &CChildView::OnRbtnRestart)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_SETTINGS, &CChildView::OnFileSettings)
	ON_MESSAGE(UM_SETTING_OK, &CChildView::OnUmSettingOK)
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	
	CRect rc;
	GetClientRect(&rc);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rc.right, rc.bottom);
	CBitmap *pOldBitmap  = memDC.SelectObject(&bmp);
	memDC.FillSolidRect(&rc, RGB(255, 255, 255));

	CImageList &imageList = m_symbol.GetImageList();
	CSymbolManage::SYMLIST symList = m_symbol.GetSymList();

	POINT pt;

	if (m_enumGameState == GAME_FINISH)
	{
		CFont font;
		font.CreatePointFont(240, TEXT("Arial"), NULL);
		CFont *pOldFont = memDC.SelectObject(&font);
		COLORREF colorText = memDC.SetTextColor(RGB(0, 255, 0));
		
		CString str0(TEXT("You Win!"));
		CString str2 = m_timeElapsed.Format(TEXT("%M:%S"));
		CString str1(TEXT("Elapsed time:"));

		CSize sz = memDC.GetTextExtent(str0);
		int deltaY = (rc.bottom-sz.cy*3) / 2;

		memDC.TextOut((rc.right-sz.cx)/2, deltaY, str0);
		sz = memDC.GetTextExtent(str1);
		memDC.TextOut((rc.right-sz.cx)/2, deltaY+sz.cy, str1);
		sz = memDC.GetTextExtent(str2);
		memDC.TextOut((rc.right-sz.cx)/2, deltaY+sz.cy+sz.cy, str2);

		memDC.SelectObject(pOldFont);
		memDC.SetTextColor(colorText);
		font.DeleteObject();
		goto DRAW_TO_SCREEN;
	}
	else if (GAME_DEADLOCK == m_enumGameState)
	{
		CFont font;
		font.CreatePointFont(360, TEXT("Arial"), NULL);
		CFont *pOldFont = memDC.SelectObject(&font);
		COLORREF colorText = memDC.SetTextColor(RGB(255, 0, 0));
		
		CString str0(TEXT("Game over!"));

		CSize sz = memDC.GetTextExtent(str0);
		int deltaY = (rc.bottom-sz.cy) / 2;

		memDC.TextOut((rc.right-sz.cx)/2, deltaY, str0);

		memDC.SelectObject(pOldFont);
		memDC.SetTextColor(colorText);
		font.DeleteObject();
		goto DRAW_TO_SCREEN;
	}
	
	/*int x = 0;
	int y = 0;
	int nSymbol = m_symbol.GetColum();
	for (int i = 0; i < nSymbol; ++i)
	{
		memDC.MoveTo(0, y/nSymbol);
		memDC.LineTo(rc.right,y/nSymbol);
		y += rc.bottom;
		memDC.MoveTo(x/nSymbol, 0);
		memDC.LineTo(x/nSymbol, rc.bottom);
		x += rc.right;
	}*/

	//draw images
	pt.x  = 0;
	pt.y  = 0;
	for (int i = 0; i < m_symbol.GetRow(); ++i)
	{
		pt.x = 0;
		for (int j = 0; j < m_symbol.GetColum(); ++j)
		{
			int nIndex = m_symbol.PosToIndex(j, i);
			if (symList[nIndex].bValid)
			{		
				imageList.Draw(&memDC, symList[nIndex].nImageIndex, pt, ILD_NORMAL);
			}
			pt.x += CSymbolManage::IMAGE_RESOLUTION;
		}
		pt.y += CSymbolManage::IMAGE_RESOLUTION;
	}
	
	//selection mark
	if (m_bClicked)
	{
		RECT rect;
		rect.left    = CSymbolManage::IMAGE_RESOLUTION*m_posLast.first;
		rect.top     = CSymbolManage::IMAGE_RESOLUTION*m_posLast.second;
		rect.right   = rect.left + CSymbolManage::IMAGE_RESOLUTION;
		rect.bottom  = rect.top + CSymbolManage::IMAGE_RESOLUTION;
		POINT points[5];
		points[0].x = rect.left;
		points[0].y = rect.top;
		points[1].x = rect.left;
		points[1].y = rect.bottom;
		points[2].x = rect.right;
		points[2].y = rect.bottom;
		points[3].x = rect.right;
		points[3].y = rect.top;
		points[4].x = rect.left;
		points[4].y = rect.top;

		CPen pen(PS_DASH, 1, RGB(0, 0, 255));
		CPen *pOldPen = memDC.SelectObject(&pen);
		memDC.Polyline(points, 5);
		memDC.SelectObject(pOldPen);
	}

	//show mouse position
	if (-1 != m_posMouseOver.first)
	{
		RECT rect;
		rect.left    = m_posMouseOver.first*CSymbolManage::IMAGE_RESOLUTION-1;
		rect.top     = m_posMouseOver.second*CSymbolManage::IMAGE_RESOLUTION-1;
		rect.right   = rect.left + CSymbolManage::IMAGE_RESOLUTION+2;
		rect.bottom  = rect.top + CSymbolManage::IMAGE_RESOLUTION+2;
		POINT points[5];
		points[0].x = rect.left;
		points[0].y = rect.top;
		points[1].x = rect.left;
		points[1].y = rect.bottom;
		points[2].x = rect.right;
		points[2].y = rect.bottom;
		points[3].x = rect.right;
		points[3].y = rect.top;
		points[4].x = rect.left;
		points[4].y = rect.top;

		CPen pen(PS_DOT, 1, RGB(255, 0, 0));
		CPen *pOldPen = memDC.SelectObject(&pen);
		memDC.Polyline(points, 5);
		memDC.SelectObject(pOldPen);
		/*pt.x = rect.left;
		pt.y = rect.top;
		if (symList[m_symbol.PosToIndex(&m_posMouseOver)].bValid)
		{
			imageList.Draw(&memDC, symList[m_symbol.PosToIndex(&m_posMouseOver)].nImageIndex, pt, ILD_NORMAL);
		}*/
	}

	//linkup route
	if (-1 != m_symbol.GetCrossing())
	{
		CSymbolManage::IMPOSITION *pCrossingPos = m_symbol.GetCrossingPos();
		CPen pen(PS_SOLID, 1, RGB(255, 0, 0));
		CPen *pOldPen = memDC.SelectObject(&pen);
		POINT startPoint;
		int nHalf = CSymbolManage::IMAGE_RESOLUTION/2;
		startPoint.x = pCrossingPos[0].first*CSymbolManage::IMAGE_RESOLUTION+nHalf;
		startPoint.y = pCrossingPos[0].second*CSymbolManage::IMAGE_RESOLUTION+nHalf;
		
		memDC.MoveTo(startPoint);
		switch (m_symbol.GetCrossing())
		{
		case 0:
			startPoint.x = pCrossingPos[3].first*CSymbolManage::IMAGE_RESOLUTION+nHalf;
			startPoint.y = pCrossingPos[3].second*CSymbolManage::IMAGE_RESOLUTION+nHalf;
			memDC.LineTo(startPoint);
			break;
		case 1:
			startPoint.x = pCrossingPos[1].first*CSymbolManage::IMAGE_RESOLUTION+nHalf;
			startPoint.y = pCrossingPos[1].second*CSymbolManage::IMAGE_RESOLUTION+nHalf;
			memDC.LineTo(startPoint);
			startPoint.x = pCrossingPos[3].first*CSymbolManage::IMAGE_RESOLUTION+nHalf;
			startPoint.y = pCrossingPos[3].second*CSymbolManage::IMAGE_RESOLUTION+nHalf;
			memDC.LineTo(startPoint);
			break;
		case 2:
			startPoint.x = pCrossingPos[1].first*CSymbolManage::IMAGE_RESOLUTION+nHalf;
			startPoint.y = pCrossingPos[1].second*CSymbolManage::IMAGE_RESOLUTION+nHalf;
			memDC.LineTo(startPoint);
			startPoint.x = pCrossingPos[2].first*CSymbolManage::IMAGE_RESOLUTION+nHalf;
			startPoint.y = pCrossingPos[2].second*CSymbolManage::IMAGE_RESOLUTION+nHalf;
			memDC.LineTo(startPoint);
			startPoint.x = pCrossingPos[3].first*CSymbolManage::IMAGE_RESOLUTION+nHalf;
			startPoint.y = pCrossingPos[3].second*CSymbolManage::IMAGE_RESOLUTION+nHalf;
			memDC.LineTo(startPoint);
			break;
		default:
			break;
		}
		memDC.SelectObject(pOldPen);
	}

DRAW_TO_SCREEN:
	//
	dc.BitBlt(0, 0, rc.right, rc.bottom, &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOldBitmap);
	bmp.DeleteObject();
	memDC.DeleteDC();
}



void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_enumGameState == GAME_FINISH)
	{
		m_symbol.Init();
		m_timeStart = CTime::GetTickCount();
		m_enumGameState = GAME_RUN;
	}

	m_symbol.SetCrossing(-1);
	int x = point.x/CSymbolManage::IMAGE_RESOLUTION;
	int y = point.y/CSymbolManage::IMAGE_RESOLUTION;
	CSymbolManage::IMPOSITION posCur(x, y);
	if (!m_symbol.IsPosValid(posCur))
	{
		return;
	}

	if (!m_bClicked)
	{
		m_bClicked = true;
		m_posLast = posCur;
	}
	else if (x == m_posLast.first && y == m_posLast.second)
	{
		m_bClicked = false;
	}
	else //two different pos clicked
	{
		m_bClicked = false;
		if (m_symbol.PosLinked(m_posLast, posCur))//can linkup
		{
			m_symbol.RemovePos(m_posLast);
			m_symbol.RemovePos(posCur);
			m_symbol.DecressUnlinkedPair();
			

			if (m_symbol.IsGameFinished())//gameover
			{
				CTime timeEnd = CTime::GetTickCount();
				m_timeElapsed = timeEnd - m_timeStart;
				m_enumGameState = GAME_FINISH;
				PlaySound(TEXT("system.wav"), NULL, SND_ASYNC | SND_NODEFAULT | SND_FILENAME);
			}
			SetTimer(1, 1000, NULL);
		}
		else if (m_symbol.IsDeadLock())//check if game in deadlock
		{
			m_enumGameState = GAME_DEADLOCK;
			m_symbol.SetCrossing(-1);
		}
		else
		{
			m_symbol.SetCrossing(-1);
		}
	}
	Invalidate(false);
	CWnd::OnLButtonDown(nFlags, point);
}


void CChildView::OnRButtonUp(UINT nFlags, CPoint point)
{
	CMenu mn;
	mn.LoadMenu(IDR_MENU_RBTN);
	CMenu *pSub = mn.GetSubMenu(0);
	ASSERT(pSub);

	ClientToScreen(&point);
	pSub->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

	mn.DestroyMenu();

	CWnd::OnRButtonUp(nFlags, point);
}


void CChildView::OnRbtnRestart()
{
	m_symbol.Init();
	m_timeStart = CTime::GetCurrentTime();
	m_enumGameState = GAME_RUN;
	Invalidate(FALSE);
}


BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return false;
//	return CWnd::OnEraseBkgnd(pDC);
}


void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{

	CSymbolManage::IMPOSITION pos(point.x/CSymbolManage::IMAGE_RESOLUTION, point.y/CSymbolManage::IMAGE_RESOLUTION);
	if (pos == m_posMouseOver)
	{
		return;
	}
	m_posMouseOver = pos;
	/*RECT rect;
	rect.left    = m_posMouseOver.first*CSymbolManage::IMAGE_RESOLUTION;
	rect.top     = m_posMouseOver.second*CSymbolManage::IMAGE_RESOLUTION;
	rect.right   = rect.left + CSymbolManage::IMAGE_RESOLUTION+1;
	rect.bottom  = rect.top + CSymbolManage::IMAGE_RESOLUTION+1;
	InvalidateRect(&rect, false);*/
	Invalidate(false);
//	CWnd::OnMouseMove(nFlags, point);
}


void CChildView::OnTimer(UINT_PTR nIDEvent)
{
	
	if (1 == nIDEvent)
	{
		m_symbol.SetCrossing(-1);
		Invalidate(false);
		KillTimer(1);
	}
	CWnd::OnTimer(nIDEvent);
}

#include "MainFrm.h"
void CChildView::OnFileSettings()
{
	CMainFrame *pMainFrame = (CMainFrame*)theApp.GetMainWnd();
	if (m_pWndSetting == NULL)
	{
		m_pWndSetting = new CSettingView;
		CCreateContext ccc;
		ccc.m_pCurrentDoc = NULL;
		ccc.m_pCurrentFrame = NULL;
		ccc.m_pLastView     = NULL;
		ccc.m_pNewDocTemplate  = NULL;
		ccc.m_pNewViewClass    = NULL;
		CRect rc;
		GetClientRect(&rc);
		m_pWndSetting->Create(TEXT("FormView"), TEXT("FormView"), WS_CHILD|WS_VISIBLE, rc, this, CSettingView::IDD, &ccc);
	}
	m_pWndSetting->Init(m_symbol.GetColum(), m_symbol.GetRow(), m_symbol.GetSortOfImage());
	m_pWndSetting->ShowWindow(SW_SHOW);
}


LRESULT CChildView::OnUmSettingOK(WPARAM, LPARAM)
{
	m_symbol.SetColum(m_pWndSetting->m_nEditWidth);
	m_symbol.SetRow(m_pWndSetting->m_nEditHeight);
	if ((int)m_pWndSetting->m_nEditImage > m_symbol.GetImageList().GetImageCount())
	{
		m_symbol.SetSortOfImage(m_symbol.GetImageList().GetImageCount());
	}
	else
	{
		m_symbol.SetSortOfImage(m_pWndSetting->m_nEditImage);
	}
	m_symbol.Init();
	m_timeStart = CTime::GetCurrentTime();


	int cx = m_symbol.GetColum() * symbollink::CSymbolManage::IMAGE_RESOLUTION + GetSystemMetrics(SM_CXBORDER)*2+8;
	int cy = m_symbol.GetRow()* symbollink::CSymbolManage::IMAGE_RESOLUTION + GetSystemMetrics(SM_CYBORDER)*2+GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYMENU)+8;
	GetParent()->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOMOVE);


	Invalidate(false);
	return 0;
}