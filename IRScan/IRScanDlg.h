
// IRScanDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "CaptureDlg.h"
#include "SettingDlg.h"


// CIRScanDlg dialog
class CIRScanDlg : public CDialogEx
{
// Construction
public:
	CIRScanDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IRSCAN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tab;
	CCaptureDlg capDlg;
	CSettingDlg setDlg;
	afx_msg void OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
};
