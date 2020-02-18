#pragma once



// CSettingView form view

class CSettingView : public CFormView
{
	DECLARE_DYNCREATE(CSettingView)

public:
	CSettingView();           // protected constructor used by dynamic creation
	virtual ~CSettingView();

public:
	enum { IDD = IDD_FORMVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	UINT m_nEditWidth;
	UINT m_nEditHeight;
	UINT m_nEditImage;
	void Init(int w, int h, int n){m_nEditWidth = w; m_nEditHeight = h; m_nEditImage = n; UpdateData(FALSE);};
	afx_msg void OnBnClickedSetOk();
	afx_msg void OnDeltaposSpinWidth(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg void OnDeltaposSpinHeight(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinImage(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSetCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};


