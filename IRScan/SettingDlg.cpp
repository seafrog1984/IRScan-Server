// SettingDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "IRScan.h"
#include "SettingDlg.h"
#include "afxdialogex.h"
#include <fstream>
#include <iostream>

extern CString g_port;
extern CString g_ip;
extern CString g_uport;
extern CString g_user;
extern CString g_passwd;

using namespace std;
// CSettingDlg �Ի���

IMPLEMENT_DYNAMIC(CSettingDlg, CDialogEx)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingDlg::IDD, pParent)
	, m_ip(_T(""))
	, m_port(_T(""))
	, m_list(_T(""))
	, m_uport(_T(""))
{

}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IP);
	DDX_Control(pDX, IDC_EDIT1, m_PORT);

	DDX_Text(pDX, IDC_EDIT1, m_port);
	DDX_Control(pDX, IDC_LIST1, m_LIST);
	DDX_LBString(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_EDIT_USER, m_USER);
	DDX_Control(pDX, IDC_EDIT_PASSWD, m_PASSWD);
	DDX_Control(pDX, IDC_CONN, mb_conn);
	DDX_Control(pDX, IDC_CHECK1, m_CHECK1);
	DDX_Control(pDX, IDC_CHECK2, m_CHECK2);
	DDX_Control(pDX, IDC_CHECK3, m_CHECK3);
	DDX_Control(pDX, IDC_EDIT3, m_UPORT);
	DDX_Text(pDX, IDC_EDIT3, m_uport);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CONN, &CSettingDlg::OnBnClickedConn)
	ON_BN_CLICKED(IDC_LIST_USER, &CSettingDlg::OnBnClickedListUser)
	ON_BN_CLICKED(IDC_RADIO1, &CSettingDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CSettingDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CSettingDlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDOK, &CSettingDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO4, &CSettingDlg::OnBnClickedRadio4)
	ON_LBN_SELCHANGE(IDC_LIST1, &CSettingDlg::OnSelchangeList1)
	ON_BN_CLICKED(IDC_RADIO5, &CSettingDlg::OnBnClickedRadio5)
END_MESSAGE_MAP()


// CSettingDlg ��Ϣ�������


BOOL CSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_ip = g_ip;
	m_port = g_port;
	m_uport = g_uport;
	m_user = g_user;
	m_passwd = g_passwd;
	m_level = 0;
	m_opt_level = 0;

	m_PORT.SetWindowText(m_port);
	m_UPORT.SetWindowText(m_uport);

	DWORD dwIP = ntohl(inet_addr(m_ip));
	m_IP.SetAddress(dwIP);

	OnBnClickedRadio1();
	OnBnClickedRadio4();

	m_PORT.GetWindowText(m_port);
	m_IP.GetWindowText(m_ip);
	m_msg = "����IP: " + m_ip;
	m_msg.Append("\n�˿ں�: ");
	m_msg.Append(m_port);
//	MessageBox(m_msg);

	if (m_cli.init(m_ip.GetBuffer(), _ttoi(m_uport)))
	{
//	m_msg = "���ӳɹ�";
		/*	m_PORT.EnableWindow(FALSE);
		m_IP.EnableWindow(FALSE);
		mb_conn.EnableWindow(FALSE);*/
	}
	else
	{
		m_msg = "����ʧ��\n��ȷ��IP��˿ں�";
		MessageBox(m_msg);
	}
//	MessageBox(m_msg);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CSettingDlg::OnBnClickedConn()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	ofstream fout("config.ini");

	if (fout.fail())
	{
		AfxMessageBox(_T("û���ҵ������ļ���"));
		exit(-1);
	}

	m_PORT.GetWindowText(g_port);
	m_UPORT.GetWindowText(g_uport);
	m_IP.GetWindowText(g_ip);


	fout << g_ip << ' '<<g_port<<' '<<g_uport;

	fout.close();
	AfxMessageBox(_T("�޸ĳɹ���"));
}

void CSettingDlg::set_permissions()
{
	if (m_list.IsEmpty())
	{
		return;
	}

	if (m_userinfo.end() != m_userinfo.find(m_list.GetBuffer()))
	{
		//MessageBox(m_userinfo[m_list.GetBuffer()].c_str());
		int permissions = atoi(m_userinfo[m_list.GetBuffer()].c_str());
		if (permissions & 0x01)
		{
			m_CHECK1.SetCheck(TRUE);
		}
		else
		{
			m_CHECK1.SetCheck(FALSE);
		}
		if (permissions & 0x02)
		{
			m_CHECK2.SetCheck(TRUE);
		}
		else
		{
			m_CHECK2.SetCheck(FALSE);
		}
		if (permissions & 0x04)
		{
			m_CHECK3.SetCheck(TRUE);
		}
		else
		{
			m_CHECK3.SetCheck(FALSE);
		}
	}
	else
	{
		m_msg = "not find permissions for " + m_list;
		MessageBox(m_msg);
	}
}

int CSettingDlg::get_permissions()
{
	int permissions = 0;
	int state = m_CHECK1.GetCheck();
	if (1 == state)
	{
		permissions |= 1;
		//CString sTmp;
		//sTmp.Format("%d",level);
		//MessageBox(sTmp);
	}

	state = m_CHECK2.GetCheck();
	if (1 == state)
	{
		permissions |= (1 << 1);
		//CString sTmp;
		//sTmp.Format("%d",level);
		//MessageBox(sTmp);
	}
	state = m_CHECK3.GetCheck();
	if (1 == state)
	{
		permissions |= (1 << 2);
		//CString sTmp;
		//sTmp.Format("%d",level);
		//MessageBox(sTmp);
	}
	//CString sTmp;
	//sTmp.Format("%d",permissions);
	//MessageBox(sTmp);
	return permissions;
}



void CSettingDlg::OnBnClickedListUser()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
//	m_user = "admin";
//	m_passwd = "test@1234";
	if (!m_cli.get_users(m_user, m_passwd, m_userinfo, m_level))
	{
		m_msg = "��ȡ�û��б�ʧ��\n";
		m_msg.Append(m_cli.get_msg().c_str());
		m_cli.close();
		mb_conn.EnableWindow(TRUE);
	}
	else
	{
		m_msg = "��ȡ�û��б�ɹ�\n";
		m_LIST.ResetContent();
		std::map<std::string, std::string>::iterator itor = m_userinfo.begin();
		for (; itor != m_userinfo.end(); ++itor)
		{
			//MessageBox((itor->first+"="+itor->second).c_str());
			m_LIST.AddString(itor->first.c_str());
		}
	}
	MessageBox(m_msg);
}


void CSettingDlg::OnBnClickedRadio1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_opt = 0;

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
	GetDlgItem(IDC_STA_TITLE)->SetWindowText("�����û�");
	GetDlgItem(IDOK)->SetWindowText("����");

	GetDlgItem(IDC_RADIO4)->ShowWindow(TRUE);
	GetDlgItem(IDC_RADIO5)->ShowWindow(TRUE);

	GetDlgItem(IDC_STA_USER)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_USER)->ShowWindow(TRUE);
	GetDlgItem(IDC_STA_PASSWD)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_PASSWD)->ShowWindow(TRUE);

	GetDlgItem(IDC_CHECK1)->ShowWindow(TRUE);
	GetDlgItem(IDC_CHECK2)->ShowWindow(TRUE);
	GetDlgItem(IDC_CHECK3)->ShowWindow(TRUE);

	m_USER.SetReadOnly(FALSE);
}


void CSettingDlg::OnBnClickedRadio2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_opt = 1;

	((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(TRUE);
	GetDlgItem(IDC_STA_TITLE)->SetWindowText("ɾ���û�");
	GetDlgItem(IDOK)->SetWindowText("ɾ��");

	GetDlgItem(IDC_RADIO4)->ShowWindow(FALSE);
	GetDlgItem(IDC_RADIO5)->ShowWindow(FALSE);

	GetDlgItem(IDC_STA_USER)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_USER)->ShowWindow(TRUE);
	GetDlgItem(IDC_STA_PASSWD)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_PASSWD)->ShowWindow(FALSE);

	GetDlgItem(IDC_CHECK1)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK2)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK3)->ShowWindow(FALSE);

	m_USER.SetReadOnly(TRUE);

	if ("" != m_list)
	{
		GetDlgItem(IDC_EDIT_USER)->SetWindowText(m_list);
	}
	else
	{
		GetDlgItem(IDC_EDIT_USER)->SetWindowText(_T(""));
	}
}


void CSettingDlg::OnBnClickedRadio3()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_opt = 2;

	((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(TRUE);
	GetDlgItem(IDC_STA_TITLE)->SetWindowText("�޸�����");
	GetDlgItem(IDOK)->SetWindowText("�޸�");

	GetDlgItem(IDC_RADIO4)->ShowWindow(FALSE);
	GetDlgItem(IDC_RADIO5)->ShowWindow(FALSE);

	GetDlgItem(IDC_STA_USER)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_USER)->ShowWindow(TRUE);
	GetDlgItem(IDC_STA_PASSWD)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_PASSWD)->ShowWindow(TRUE);

	GetDlgItem(IDC_CHECK1)->ShowWindow(TRUE);
	GetDlgItem(IDC_CHECK2)->ShowWindow(TRUE);
	GetDlgItem(IDC_CHECK3)->ShowWindow(TRUE);

	m_USER.SetReadOnly(TRUE);
	m_PASSWD.SetWindowText(_T(""));

	if ("" != m_list)
	{
		GetDlgItem(IDC_EDIT_USER)->SetWindowText(m_list);
		set_permissions();
	}
	else
	{
		GetDlgItem(IDC_EDIT_USER)->SetWindowText(_T(""));
		m_CHECK1.SetCheck(FALSE);
		m_CHECK2.SetCheck(FALSE);
		m_CHECK3.SetCheck(FALSE);
	}
}


void CSettingDlg::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
//	CDialogEx::OnOK();
	if (0 == m_opt)
	{
		CString sUser;
		m_USER.GetWindowText(sUser);
		if (sUser.IsEmpty())
		{
			MessageBox("�û�������Ϊ��");
			return;
		}
		CString sPasswd;
		m_PASSWD.GetWindowText(sPasswd);
		if (sPasswd.IsEmpty())
		{
			MessageBox("���벻��Ϊ��");
			return;
		}

		int permissions = get_permissions();
		if (m_cli.new_user(sUser, sPasswd, m_opt_level, permissions, m_user, m_passwd))
		{
			m_LIST.AddString(sUser);
			m_msg = "�����û��ɹ�";
		}
		else
		{
			m_msg = "�����û�ʧ��\n";
			m_msg.Append(m_cli.get_msg().c_str());
		}
		MessageBox(m_msg);
	}
	else if (1 == m_opt)
	{
		CString sUser;
		m_USER.GetWindowText(sUser);
		if (sUser.IsEmpty())
		{
			MessageBox("��ѡ��Ҫɾ�����û�");
			return;
		}

		if (m_cli.del_user(sUser, m_user, m_passwd))
		{
			int n = m_LIST.FindString(0, sUser);
			m_LIST.DeleteString(n);
			GetDlgItem(IDC_EDIT_USER)->SetWindowText(_T(""));
			m_msg = "ɾ���û��ɹ�";
		}
		else
		{
			m_msg = "ɾ���û�ʧ��\n";
			m_msg.Append(m_cli.get_msg().c_str());
		}
		MessageBox(m_msg);

	}
	else
	{
		CString sUser;
		m_USER.GetWindowText(sUser);
		if (sUser.IsEmpty())
		{
			MessageBox("��ѡ��Ҫ�޸ĵ��û�");
			return;
		}

		int permissions = get_permissions();

		CString sPasswd;
		m_PASSWD.GetWindowText(sPasswd);
		if (sPasswd.IsEmpty())
		{
			if (atoi(m_userinfo[m_list.GetBuffer()].c_str()) == permissions)
			{
				MessageBox("δ�޸��κ���Ϣ");
				return;
			}
			sPasswd = "*";
		}

		if (m_cli.update_user(sUser, sPasswd, permissions, m_user, m_passwd))
		{
			m_msg = "�޸��û��ɹ�";
		}
		else
		{
			m_msg = "�޸��û�ʧ��\n";
			m_msg.Append(m_cli.get_msg().c_str());
		}
		MessageBox(m_msg);
	}


}


void CSettingDlg::OnBnClickedRadio4()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(TRUE);
	m_opt_level = 0;
}


void CSettingDlg::OnSelchangeList1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int nSel = m_LIST.GetCurSel();
	m_LIST.GetText(nSel, m_list);
	//MessageBox(m_list);
	if (0 != m_opt)
	{
		GetDlgItem(IDC_EDIT_USER)->SetWindowText(m_list);
		if (2 == m_opt)
		{
			set_permissions();
			m_PASSWD.SetWindowText(_T(""));
		}
	}
}


void CSettingDlg::OnBnClickedRadio5()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	((CButton*)GetDlgItem(IDC_RADIO5))->SetCheck(TRUE);
	m_opt_level = 1;
}
