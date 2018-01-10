#pragma once
#include "afxwin.h"
#include "./incl/client.h"

// CLogDlg �Ի���

class CLogDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLogDlg)

public:
	CLogDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLogDlg();

// �Ի�������
	enum { IDD = IDD_LOG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_USER;
	CEdit m_PASSWD;
	CString m_user;
	CString m_passwd;

	CString m_msg;

	client_t m_cli;
	CString m_port;

	CString m_ip;
	int iTestFlag;

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
