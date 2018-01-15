#pragma once

#include <opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
// CCaptureDlg �Ի���

#include "FGInterface.h"
#include "mscomm1.h"

#include "afxwin.h"
#include "afxcmn.h"

/////////////////////////////////////////////
//20170509
#include "./incl/client.h"


using namespace cv;

class CCaptureDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCaptureDlg)

public:
	CCaptureDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCaptureDlg();

// �Ի�������
	enum { IDD = IDD_CAPTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCon();

public:
	CString m_path;
	FGInterface m_FG;
	Mat img;
	void * m_ImageData8;
	unsigned short *tmp;
	int m_CameraType;
	CString m_sRXDATA;
    CString m_sTXDATA;
	CString m_PID;

public://���ݿ�����
	/////////////////////////////////////////////
	//20170912
	CString m_msg;

	client_t m_cli;

	string sID;
	vector<std::string> vecPngIDReq;
	vector<std::string> vecPngIDResp;

	CString m_port;
	CEdit m_PORT;
	CString m_ip;


	/////////////////////////////////////////////

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSave();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedInit();
//	afx_msg void OnBnClickedRot();
	void InterrogateFocus(void);

	CMscomm1 m_CtrlCom;
	DECLARE_EVENTSINK_MAP()
	void OnOncommMscomm1();
	CString m_cardNO;
	CString m_NO;
	CString m_name;
	CString m_sex;
	int m_age;
	CString m_ID;
	string sScanID;//ɨ���
	string sCardID;//����
	CString m_user;//�û���
	CString m_passwd;//����
	CString strPath;//ͼ����ʱ����·��
	afx_msg void OnBnClickedReadcard();
	afx_msg void OnBnClickedAdj();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	afx_msg void OnBnClickedRot2();
	afx_msg void OnBnClickedP1();
	afx_msg void OnBnClickedP2();
	afx_msg void OnBnClickedP3();
	afx_msg void OnBnClickedP4();
	afx_msg void OnBnClickedConfirm();

	void Ori2Mat(unsigned short *tmp,Mat &g_dst);
	void rot90(Mat g_dst, Mat &tmpimg);
	void MultiImage_OneWin(const std::string& MultiShow_WinName, const vector<Mat>& SrcImg_V, CvSize SubPlot, CvSize ImgMax_Size );
	afx_msg void OnBnClickedTestserv();

	CMscomm1 m_CtrlCard;
	void OnCommMscomm2();
	CEdit m_SCANID;
	afx_msg void OnBnClickedSaveall();
};
