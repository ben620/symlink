
// ChildView.h : interface of the CChildView class
//


#pragma once

#include "SymbolManage.h"
#include "SettingView.h"
// CChildView window

class CChildView : public CWnd
{
	enum enumGameState
	{
		GAME_RUN = 1,
		GAME_DEADLOCK,
		GAME_FINISH
	};
// Construction
public:
	explicit CChildView();

// Attributes
public:
	symbollink::CSymbolManage              m_symbol;
	symbollink::CSymbolManage::IMPOSITION  m_posLast;
	symbollink::CSymbolManage::IMPOSITION  m_curPos;
	symbollink::CSymbolManage::IMPOSITION  m_posMouseOver;
	bool                                   m_bClicked;
	int                                    m_busy;
	CTime                                  m_timeStart;
	CTimeSpan                              m_timeElapsed;
	enumGameState                          m_enumGameState;

private:
	CSettingView *m_pWndSetting;
// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRbtnRestart();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnFileSettings();
	afx_msg LRESULT OnUmSettingOK(WPARAM wParam, LPARAM lParam);
};

