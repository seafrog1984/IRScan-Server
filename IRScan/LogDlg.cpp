// LogDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IRScan.h"
#include "LogDlg.h"
#include "afxdialogex.h"
#include"IRScanDlg.h"

extern CString g_user;
extern CString g_passwd;
extern int g_log_flag;

// CLogDlg 对话框

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


// CLogDlg 消息处理程序


void CLogDlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码

	m_USER.GetWindowText(m_user);
	g_user = m_user;
	m_PASSWD.GetWindowText(m_passwd);
	g_passwd = m_passwd;
	//	m_PORT.GetWindowText(m_port);
	//	m_IP.GetWindowText(m_ip);
	/*m_msg = "连接IP: " + m_ip;
	m_msg.Append("\n端口号: ");
	m_msg.Append(m_port);
	MessageBox(m_msg);*/

	if (m_cli.init(m_ip.GetBuffer(), _ttoi(m_port)))
	{
		m_msg = "连接成功";
		//	m_PORT.EnableWindow(FALSE);
		//	m_IP.EnableWindow(FALSE);

		m_USER.EnableWindow(FALSE);
		m_PASSWD.EnableWindow(FALSE);

		//	mb_login.EnableWindow(FALSE);
		MessageBox(m_msg);
	}
	else
	{
		m_msg = "连接失败\n请确认IP或端口号";
		MessageBox(m_msg);
		return;
	}
	//2-
	std::string sPermissions;
	// 注意： 0表示测试客户端， 1表示正式客户端
	int iRet = m_cli.login_auth(m_user, m_passwd, sPermissions, iTestFlag);
	if (0 > iRet)
	{
		m_msg = "获取授权码失败\n";
		m_msg.Append(m_cli.get_msg().c_str());
		m_cli.close();
		//	mb_conn.EnableWindow(TRUE);
	}
	else
	{
		m_msg = "获取授权码成功\n";
		CString sAuth = m_cli.get_auth().c_str();
		CString sRep1 = sAuth.Mid(8, 16);
		CString sRep2('*', sRep1.GetLength());
		sAuth.Replace(sRep1, sRep2);
		if (0 == iRet)
		{
			m_msg.Append("测试环境: ");

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

	//	m_msg.Append("\n权限: ");
	//	m_msg.Append("图像扫描");
		if (pt.p1||pt.p3)
		{
			g_log_flag = 1;

		}
		else
		{
			m_msg.Append("没有权限");
			MessageBox(m_msg);
		}
	
	}

//	
	CDialogEx::OnOK();
}


BOOL CLogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
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
	// 异常:  OCX 属性页应返回 FALSE
}
