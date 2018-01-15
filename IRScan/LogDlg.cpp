// LogDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "IRScan.h"
#include "LogDlg.h"
#include "afxdialogex.h"
#include"IRScanDlg.h"

extern CString g_user;
extern CString g_passwd;
extern int g_log_flag;

// CLogDlg �Ի���

IMPLEMENT_DYNAMIC(CLogDlg, CDialogEx)

CLogDlg::CLogDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLogDlg::IDD, pParent)
	, m_user(_T(""))
	, m_passwd(_T(""))
{

}

CLogDlg::~CLogDlg()
{
}

void CLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_USER, m_USER);
	DDX_Control(pDX, IDC_PASSWD, m_PASSWD);
	DDX_Text(pDX, IDC_USER, m_user);
	DDX_Text(pDX, IDC_PASSWD, m_passwd);
}


BEGIN_MESSAGE_MAP(CLogDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLogDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLogDlg ��Ϣ�������


void CLogDlg::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	m_USER.GetWindowText(m_user);
	g_user = m_user;
	m_PASSWD.GetWindowText(m_passwd);
	g_passwd = m_passwd;
	//	m_PORT.GetWindowText(m_port);
	//	m_IP.GetWindowText(m_ip);
	/*m_msg = "����IP: " + m_ip;
	m_msg.Append("\n�˿ں�: ");
	m_msg.Append(m_port);
	MessageBox(m_msg);*/

	if (m_cli.init(m_ip.GetBuffer(), _ttoi(m_port)))
	{
		m_msg = "���ӳɹ�";
		//	m_PORT.EnableWindow(FALSE);
		//	m_IP.EnableWindow(FALSE);

		m_USER.EnableWindow(FALSE);
		m_PASSWD.EnableWindow(FALSE);

		//	mb_login.EnableWindow(FALSE);
		MessageBox(m_msg);
	}
	else
	{
		m_msg = "����ʧ��\n��ȷ��IP��˿ں�";
		MessageBox(m_msg);
		return;
	}
	//2-
	std::string sPermissions;
	// ע�⣺ 0��ʾ���Կͻ��ˣ� 1��ʾ��ʽ�ͻ���
	int iRet = m_cli.login_auth(m_user, m_passwd, sPermissions, iTestFlag);
	if (0 > iRet)
	{
		m_msg = "��ȡ��Ȩ��ʧ��\n";
		m_msg.Append(m_cli.get_msg().c_str());
		m_cli.close();
		//	mb_conn.EnableWindow(TRUE);
	}
	else
	{
		m_msg = "��ȡ��Ȩ��ɹ�\n";
		CString sAuth = m_cli.get_auth().c_str();
		CString sRep1 = sAuth.Mid(8, 16);
		CString sRep2('*', sRep1.GetLength());
		sAuth.Replace(sRep1, sRep2);
		if (0 == iRet)
		{
			m_msg.Append("���Ի���: ");

		}
		m_msg.Append(sAuth);

		int iPermissions = atoi(sPermissions.c_str());
		permissions_t pt;
		if (iPermissions & 0x01)
		{
			pt.p1 = true;
		}
		if (iPermissions & 0x02)
		{
			pt.p2 = true;
		}
		if (iPermissions & 0x04)
		{
			pt.p3 = true;
		}

	//	m_msg.Append("\nȨ��: ");
	//	m_msg.Append("ͼ��ɨ��");
		if (pt.p1||pt.p3)
		{
			g_log_flag = 1;

		}
		else
		{
			m_msg.Append("û��Ȩ��");
			MessageBox(m_msg);
		}
	
	}

//	
	CDialogEx::OnOK();
}


BOOL CLogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_port = "44444";
	//m_ip = "10.70.77.81";
	//m_ip="192.168.31.157";
	m_ip = "119.29.233.186";
	m_user = "test";
	m_passwd = "test@1234";
	m_USER.SetWindowText(m_user);
	m_PASSWD.SetWindowText(m_passwd);
	iTestFlag = 1;
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}
