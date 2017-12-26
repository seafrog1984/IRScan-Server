#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "incl\client.h"

// CSettingDlg �Ի���

class CSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSettingDlg();

// �Ի�������
	enum { IDD = IDD_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:

	CString m_msg;
	CString m_user;
	CString m_passwd;

	client_t m_cli;
	std::map<std::string, std::string> m_userinfo;
	int m_level;

	int m_opt;
	int m_opt_level;


	int get_permissions();
	void set_permissions();

	CIPAddressCtrl m_IP;
	CEdit m_PORT;
	CString m_ip;
	CString m_port;
	virtual BOOL OnInitDialog();
	CListBox m_LIST;
	CString m_list;
	CEdit m_USER;
	CEdit m_PASSWD;
	afx_msg void OnBnClickedConn();
	CButton mb_conn;
	afx_msg void OnBnClickedListUser();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	CButton m_CHECK1;
	CButton m_CHECK2;
	CButton m_CHECK3;
	afx_msg void OnBnClickedOk();
};
