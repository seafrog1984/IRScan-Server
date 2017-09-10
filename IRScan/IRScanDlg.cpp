
// IRScanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IRScan.h"
#include "IRScanDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CIRScanDlg dialog



CIRScanDlg::CIRScanDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIRScanDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIRScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
}

BEGIN_MESSAGE_MAP(CIRScanDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CIRScanDlg::OnSelchangeTab1)
END_MESSAGE_MAP()


// CIRScanDlg message handlers

BOOL CIRScanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CRect tabRect;   // ��ǩ�ؼ��ͻ�����λ�úʹ�С   

	m_tab.InsertItem(0, _T("���ݲɼ�"));         // �����һ����ǩ  
	m_tab.InsertItem(1, _T("ϵͳ����"));  // ����ڶ�����ǩ   
	capDlg.Create(IDD_CAPTURE, &m_tab);    // ������һ����ǩҳ   
	setDlg.Create(IDD_SETTING, &m_tab); // �����ڶ�����ǩҳ   

	m_tab.GetClientRect(&tabRect);    // ��ȡ��ǩ�ؼ��ͻ���Rect   
	// ����tabRect��ʹ�串�Ƿ�Χ�ʺϷ��ñ�ǩҳ   
	tabRect.left += 1;
	tabRect.right -= 1;
	tabRect.top += 23;
	tabRect.bottom -= 1;
	// ���ݵ����õ�tabRect����capture�ӶԻ��򣬲�����Ϊ��ʾ   
	capDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
	// ���ݵ����õ�tabRect����setting�ӶԻ��򣬲�����Ϊ����   
	setDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIRScanDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIRScanDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIRScanDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CIRScanDlg::OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CRect tabRect;    // ��ǩ�ؼ��ͻ�����Rect   

	// ��ȡ��ǩ�ؼ��ͻ���Rect����������������ʺϷ��ñ�ǩҳ   
	m_tab.GetClientRect(&tabRect);
	tabRect.left += 1;
	tabRect.right -= 1;
	tabRect.top += 25;
	tabRect.bottom -= 1;

	switch (m_tab.GetCurSel())
	{
		// �����ǩ�ؼ���ǰѡ���ǩΪ�����Ͻ�ͨ��ѧ��������ʾcapture�Ի�������setting�Ի���   
	case 0:
		capDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
		setDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
		break;
		// �����ǩ�ؼ���ǰѡ���ǩΪ��������ͨ��ѧ����������capture�Ի�����ʾsetting�Ի���   
	case 1:
		capDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
		setDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
		break;
	default:
		break;
	}

	*pResult = 0;
}


/*
void CIRScanDlg::OnBnClickedPseudocolor()
+{
+	// TODO:  �ڴ���ӿؼ�֪ͨ����������
+	Mat g_src, dst;
+	if (src.channels() != 1)
+	{
+		cvtColor(src, g_src, CV_BGR2GRAY);
+	}
+	else
+	{
+		g_src = src.clone();
+	}
+
+	dst.create(src.size(), CV_8UC3);
+
+	int tmp = 0;
+
+
+	for (int i = 0; i < dst.rows; i++)
+	{
+		for (int j = 0; j < dst.cols; j++)
+		{
+			tmp = src.at<uchar>(i, j);
+
+			if (tmp < 64)
+			{
+				dst.at<Vec3b>(i, j)[0] = 255; //blue
+				dst.at<Vec3b>(i, j)[1] = tmp; //green
+				dst.at<Vec3b>(i, j)[2] = 0; //red
+			}
+			else if (tmp < 128)
+			{
+				dst.at<Vec3b>(i, j)[0] = tmp; //blue
+				dst.at<Vec3b>(i, j)[1] = 255; //green
+				dst.at<Vec3b>(i, j)[2] = 0; //red
+			}
+			else if (tmp < 192)
+			{
+				dst.at<Vec3b>(i, j)[0] = 0; //blue
+				dst.at<Vec3b>(i, j)[1] = 255; //green
+				dst.at<Vec3b>(i, j)[2] = tmp; //red
+			}
+			else
+			{
+				dst.at<Vec3b>(i, j)[0] = 0; //blue
+				dst.at<Vec3b>(i, j)[1] = tmp; //green
+				dst.at<Vec3b>(i, j)[2] = 255; //red
+			}
+		}
+	}
+
+	Mat img;  //�������ź��ͼ��
+	CRect rect;
+	GetDlgItem(IDC_PIC)->GetClientRect(&rect); //��ȡͼ����ʾ��
+
+	resize(dst, img, Size(rect.Width(), rect.Height()), 0, 0);
+
+	imshow("view", img);
+	//	imshow("view", dst);
+}
*/