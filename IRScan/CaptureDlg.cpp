// CaptureDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "IRScan.h"
#include "CaptureDlg.h"
#include "afxdialogex.h"
#include <time.h>
#include <fstream>
#include <iostream>  
#include <stdlib.h>  
#include <string>


#include <opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

#define F_TO_C(x)  ((((x)-32.0) * 5.0) / 9.0)
#define C_TO_F(x)    ((32.0) + ((x) * (9.0/5.0)))
#define HEIGHT 240
#define WIDTH 320

int pic_num = 0;
vector<Mat> showimg(12);


// CCaptureDlg �Ի���

IMPLEMENT_DYNAMIC(CCaptureDlg, CDialogEx)

CCaptureDlg::CCaptureDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CCaptureDlg::IDD, pParent)
, m_cardNO(_T(""))
, m_NO(_T(""))
, m_name(_T(""))
, m_sex(_T(""))
, m_age(0)
, m_ID(_T(""))
{


}

CCaptureDlg::~CCaptureDlg()
{
	m_FG.SetCameraFocusSpeed(0, 0);  // stop!
	m_FG.DestroyCamera();
	//free(m_ImageData8);
}

void CCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSCOMM1, m_CtrlCom);
	DDX_Text(pDX, IDC_CARDNO, m_cardNO);
	DDX_Text(pDX, IDC_NO, m_NO);
	DDX_Text(pDX, IDC_NAME, m_name);
	DDX_Text(pDX, IDC_SEX, m_sex);
	DDX_Text(pDX, IDC_AGE, m_age);
	DDX_Text(pDX, IDC_ID, m_ID);

}


BEGIN_MESSAGE_MAP(CCaptureDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CON, &CCaptureDlg::OnBnClickedCon)
	ON_BN_CLICKED(IDC_SAVE, &CCaptureDlg::OnBnClickedSave)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_INIT, &CCaptureDlg::OnBnClickedInit)
//	ON_BN_CLICKED(IDC_ROT, &CCaptureDlg::OnBnClickedRot)
	ON_BN_CLICKED(IDC_READCARD, &CCaptureDlg::OnBnClickedReadcard)
	ON_BN_CLICKED(IDC_ADJ, &CCaptureDlg::OnBnClickedAdj)
//	ON_BN_CLICKED(IDC_ROT2, &CCaptureDlg::OnBnClickedRot2)
ON_BN_CLICKED(IDC_P1, &CCaptureDlg::OnBnClickedP1)
ON_BN_CLICKED(ID_P2, &CCaptureDlg::OnBnClickedP2)
ON_BN_CLICKED(IDC_P3, &CCaptureDlg::OnBnClickedP3)
ON_BN_CLICKED(IDC_P4, &CCaptureDlg::OnBnClickedP4)
ON_BN_CLICKED(IDC_CONFIRM, &CCaptureDlg::OnBnClickedConfirm)
ON_BN_CLICKED(IDC_TESTSERV, &CCaptureDlg::OnBnClickedTestserv)
END_MESSAGE_MAP()


// CCaptureDlg ��Ϣ�������


void CCaptureDlg::OnBnClickedCon()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	SetTimer(1, 0, NULL);

	// set the timer to interrogate the focus position
	// interrogating the focus position takes time away
	// from image transfers so should not be used excessively
	SetTimer(2, 300, NULL);

	m_CameraType = ICI_CAMERA_7000;
	// Initialize camera
	m_FG.InitializeCamera(m_CameraType);
	//m_FG.SetCalibrationFormula(3);    // will auto-set based on camera internal settings
//	m_ImageData8 = NULL;


	// Allocate later.. after image size is determined
	/*
	CString tmp;
	tmp.Format(_T("%d"), sta);
	AfxMessageBox(tmp);
	*/
	/*
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL);
	dlg.m_ofn.lpstrTitle = _T("��ͼ���ļ�"); //�Ի������
	dlg.m_ofn.lpstrInitialDir = "E:\\work\\ʵ����\\����ɨ��\\ICI��ȡ��ͼ��"; //Ĭ�ϴ�·��
	dlg.m_ofn.lpstrFilter = "bmp (*.bmp)\0*.bmp\0 jpg (*.jpg)\0*.jpg\0 All Files (*.*) \0*.*\0\0"; //���ļ�����

	if (dlg.DoModal() != IDOK)             // �ж��Ƿ���ͼƬ
	return;
	m_path = dlg.GetPathName();

	Mat src;

	//���ֽ��ַ����� CString ת char*  (LPSTR)(LPCSTR)
	src = imread((LPSTR)(LPCSTR)m_path);

	Mat dst, g_src;
	dst.create(src.size(), CV_8UC3);
	cvtColor(src, g_src, CV_BGR2GRAY);

	int i, j;
	for (i = 0; i < dst.rows; i++)
	{
	for (j = 0; j < dst.cols; j++)
	{
	int tmp = g_src.at<uchar>(i, j);

	dst.at<Vec3b>(i, j)[0] = 101.2 - 116.2*cos(tmp*0.08655) + 91.93*sin(tmp*0.08592);
	dst.at<Vec3b>(i, j)[1] = 150.9 - 110.9*cos(tmp*0.08457) - 97.33*sin(tmp*0.08457);
	dst.at<Vec3b>(i, j)[2] = 125.3 + 59.93*cos(tmp*0.04896) - 130.2*sin(tmp*0.04896);


	}
	}

	namedWindow("test");

	imshow("test", g_src);
	Mat img_show;  //�������ź��ͼ��
	CRect rect;
	GetDlgItem(IDC_PIC)->GetClientRect(&rect); //��ȡͼ����ʾ��

	resize(dst, img_show, Size(rect.Width(), rect.Height()), 0, 0);

	imshow("view", img_show);
	
*/

}


BOOL CCaptureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	
	/*namedWindow("view", WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle("view");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_PIC)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);*/
	

	if (m_CtrlCom.get_PortOpen())
	{
		m_CtrlCom.put_PortOpen(FALSE);
	}
	m_CtrlCom.put_CommPort(3);                //ѡ��com3�����Ը��ݾ����������  
	m_CtrlCom.put_InBufferSize(1024);         //�������뻺�����Ĵ�С��Bytes  
	m_CtrlCom.put_OutBufferSize(1024);        //��������������Ĵ�С��Bytes  
	m_CtrlCom.put_Settings(_T("9600,n,8,1")); //������9600����У�飬8������λ��ֹͣλ1  
	m_CtrlCom.put_InputMode(1);               //1:��ʾ�Զ����Ʒ�ʽ��������  
	m_CtrlCom.put_RThreshold(1);              //����1��ʾÿ�����ڽ��ջ��������ж��ڻ����1���ַ�ʱ������һ���������ݵ�OnComm�¼�  
	m_CtrlCom.put_InputLen(0);                //���õ�ǰ������������0  
	if (!m_CtrlCom.get_PortOpen())
	{
		m_CtrlCom.put_PortOpen(TRUE);
	}
	else
	{
		AfxMessageBox(_T("Can not open serial port!"));
	}
	m_CtrlCom.get_Input();                    //��Ԥ���������������������  
	UpdateData(FALSE);



	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CCaptureDlg::OnBnClickedSave()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
////	16bitͼ�񱣴�

		if (tmp == NULL)
			return;
	//	KillTimer(1);
	//	KillTimer(2);

	//	CString strPath = "E:\\IMG\\SUN-0719";
		CString strPath = "E:\\IMG\\" + m_PID;//�˴������ⶨ�壬����ʽ��������ʾһ�£������δ������еģ�����Ѿ��������ˣ��򲻴���
		CString strWPath = strPath;
		CString strTemp;
		if (!PathFileExists(strPath))//�ļ��в������򴴽�
		{

			strPath.TrimLeft(strPath.Left(3));
			int i = strPath.Find("\\");
			if (i>0)
			{
				strTemp = strWPath.Left(3) + strPath.Left(i);
			}
			else
			{
				strTemp = strWPath;
			}
			strPath.TrimLeft(strPath.Left(i));
			if (!PathFileExists(strTemp))
				CreateDirectory(strTemp, NULL);

			while (strPath.Find("\\") == 0)
			{
				strPath.TrimLeft(strPath.Left(1));
				int j = strPath.Find("\\");
				if (j > 0)
				{
					strTemp = strTemp + "\\" + strPath.Left(j);
					strPath.TrimLeft(strPath.Left(j));
				}
				else
					strTemp = strTemp + "\\" + strPath;
				if (!PathFileExists(strTemp))
					CreateDirectory(strTemp, NULL);
			}
		}
		
	

		Mat g_dst;
		Ori2Mat(tmp, g_dst);
		Mat g_dstImage4, tempImage;

		rot90(g_dst, tempImage);
		namedWindow("test");
		imshow("test", g_dst);

		pyrDown(tempImage, g_dstImage4, Size(tempImage.cols / 2, tempImage.rows / 2));
		if (pic_num < 12)
		{
			g_dstImage4.copyTo(showimg[pic_num]);
			pic_num++;
		}


		MultiImage_OneWin("T", showimg, cvSize(4, 3), cvSize(160, 120));

		CString picno;
		picno.Format("%d", pic_num);
		strPath = strPath + '\\' + picno + ".dat";
		ofstream fout((LPSTR)(LPCSTR)strPath);
		for (int i = 0; i < img.cols*img.rows; i++)
		{
			fout << *tmp++ << ' ';
		}

		fout.close();

	//	SetTimer(1, 0, NULL);

	//	SetTimer(2, 300, NULL);

}

void CCaptureDlg::InterrogateFocus(void)
{
	long rc = m_FG.UpdateFocusReadings();
	if (rc == 0) {
		// Error reading the focus settings...
		// Either the camera doesn't have a focus motor
		// or the command timed out
	//	SetDlgItemText(IDC_FOCUS_POSITION, "N/A");
		return;
	}
	long curpos;    // current focus position
	rc = m_FG.GetFocusPosition(&curpos);
	if (rc == 0) {
		// error reading the position
		// this should not happen if UpdateFocusReadings 
		// is successful
	//	SetDlgItemText(IDC_FOCUS_POSITION, "N/A");
		return;
	}

//	SetDlgItemInt(IDC_FOCUS_POSITION, curpos, false);
}

void CCaptureDlg::Ori2Mat(unsigned short *tmp,Mat& g_dst)
{
	Mat timg(HEIGHT, WIDTH, CV_8UC1, tmp);

	//	namedWindow("src");
	//	imshow("src", src);

	//	img.create(height, width, CV_16UC1, tmp);
	timg.copyTo(img);
	//	cvtColor(src,img , CV_BGR2GRAY);

	Mat dst, g_src(img);
	dst.create(img.size(), CV_8UC3);
	g_dst.create(img.size(), CV_8UC1);
	int center_x, center_y;
	center_x = WIDTH / 2;
	center_y = HEIGHT/ 2;

	float temperature;
	if (m_CameraType != ICI_CAMERA_SWIR)
		temperature = (float)tmp[center_x + center_y * WIDTH] / 100.0;
	else
		temperature = (float)tmp[center_x + center_y * WIDTH];
//	sprintf_s(buf, sizeof(buf), "%.2f", temperature);

	//	SetDlgItemText(IDC_CENTER_TEMP, buf);

	// highly non-optimal conversion from 16-bit to 8-bit

	// Values are in degrees C * 100.0
	// So, 1 deg C would == 100
	// My mind doesn't work well in deg C so I have a F_TO_C conversion
	//
	// topvalue represents the highest temperature value we want to display
	// bottomvalue represents the lowest temperature value we want to display
	//

	bool auto_range = true; // set to false to use manual range...

	// Manual range values
	float topvalue = (int)(F_TO_C(130) * 100.0);
	float bottomvalue = (int)(F_TO_C(50) * 100.0);


	int i, end, col, row;
	float value;
	unsigned short *src = (unsigned short *)tmp;
	//unsigned char  *dest = (unsigned char  *)m_ImageData8;
	unsigned char displayValue;
	end = WIDTH *HEIGHT;  // image size...

	if (auto_range)
	{
		bottomvalue = 99999;
		topvalue = -99999;
		for (i = 0; i < end; i++)
		{
			value = src[i];
			bottomvalue = min(bottomvalue, value);
			topvalue = max(topvalue, value);
		}
	}

	float range = topvalue - bottomvalue;

	if (range != 0)
	{

		for (i = 0; i < end; i++)
		{
			value = *src++;
			// Limit upper + lower values
			if (value < bottomvalue) value = bottomvalue;
			if (value > topvalue)    value = topvalue;
			// Scale to 0..255 for display
			displayValue = ((value - bottomvalue) * 255) / range;

			row = i / WIDTH;
			col = i%WIDTH;
			g_dst.at<uchar>(row, col) = displayValue;
		}
	}


}
void CCaptureDlg::rot90(Mat g_dst, Mat& tmpimg)
{
	tmpimg.create(Size(g_dst.rows, g_dst.cols), CV_8UC1);
	for (int i = 0; i < g_dst.rows; i++)
	{
		for (int j = 0; j < g_dst.cols; j++)
		{
			tmpimg.at<uchar>(j, HEIGHT - 1 - i) = g_dst.at<uchar>(i, j);
		}

	}
}

void CCaptureDlg::MultiImage_OneWin(const std::string& MultiShow_WinName, const vector<Mat>& SrcImg_V, CvSize SubPlot, CvSize ImgMax_Size = cvSize(320, 240))
{
	//************* Usage *************//
	//vector<Mat> imgs(4);
	//imgs[0] = imread("F:\\SA2014.jpg");
	//imgs[1] = imread("F:\\SA2014.jpg");
	//imgs[2] = imread("F:\\SA2014.jpg");
	//imgs[3] = imread("F:\\SA2014.jpg");
	//MultiImage_OneWin("T", imgs, cvSize(2, 2), cvSize(400, 280));

	//Window's image
	Mat Disp_Img;
	//Width of source image
	CvSize Img_OrigSize = cvSize(SrcImg_V[0].cols, SrcImg_V[0].rows);
	//******************** Set the width for displayed image ********************//
	//Width vs height ratio of source image
	float WH_Ratio_Orig = Img_OrigSize.width / (float)Img_OrigSize.height;
	CvSize ImgDisp_Size = cvSize(100, 100);
	if (Img_OrigSize.width > ImgMax_Size.width)
		ImgDisp_Size = cvSize(ImgMax_Size.width, (int)ImgMax_Size.width / WH_Ratio_Orig);
	else if (Img_OrigSize.height > ImgMax_Size.height)
		ImgDisp_Size = cvSize((int)ImgMax_Size.height*WH_Ratio_Orig, ImgMax_Size.height);
	else
		ImgDisp_Size = cvSize(Img_OrigSize.width, Img_OrigSize.height);
	//******************** Check Image numbers with Subplot layout ********************//
//	int Img_Num = (int)SrcImg_V.size();
	int Img_Num = pic_num;
	if (Img_Num > SubPlot.width * SubPlot.height)
	{
		cout << "Your SubPlot Setting is too small !" << endl;
		exit(0);
	}
	//******************** Blank setting ********************//
	CvSize DispBlank_Edge = cvSize(80, 60);
	CvSize DispBlank_Gap = cvSize(15, 15);
	//******************** Size for Window ********************//
	Disp_Img.create(Size(ImgDisp_Size.width*SubPlot.width + DispBlank_Edge.width + (SubPlot.width - 1)*DispBlank_Gap.width,
		ImgDisp_Size.height*SubPlot.height + DispBlank_Edge.height + (SubPlot.height - 1)*DispBlank_Gap.height), CV_8UC1);
	Disp_Img.setTo(0);//Background
	//Left top position for each image
	int EdgeBlank_X = (Disp_Img.cols - (ImgDisp_Size.width*SubPlot.width + (SubPlot.width - 1)*DispBlank_Gap.width)) / 2;
	int EdgeBlank_Y = (Disp_Img.rows - (ImgDisp_Size.height*SubPlot.height + (SubPlot.height - 1)*DispBlank_Gap.height)) / 2;
	CvPoint LT_BasePos = cvPoint(EdgeBlank_X, EdgeBlank_Y);
	CvPoint LT_Pos = LT_BasePos;

	//Display all images
	for (int i = 0; i < Img_Num; i++)
	{
		//Obtain the left top position
		if ((i%SubPlot.width == 0) && (LT_Pos.x != LT_BasePos.x))
		{
			LT_Pos.x = LT_BasePos.x;
			LT_Pos.y += (DispBlank_Gap.height + ImgDisp_Size.height);
		}
		//Writting each to Window's Image
		Mat imgROI = Disp_Img(Rect(LT_Pos.x, LT_Pos.y, ImgDisp_Size.width, ImgDisp_Size.height));
		resize(SrcImg_V[i], imgROI, Size(ImgDisp_Size.width, ImgDisp_Size.height));

		LT_Pos.x += (DispBlank_Gap.width + ImgDisp_Size.width);
	}

	//Get the screen size of computer
	int Scree_W = GetSystemMetrics(SM_CXSCREEN);
	int Scree_H = GetSystemMetrics(SM_CYSCREEN);

	cvNamedWindow(MultiShow_WinName.c_str(), CV_WINDOW_AUTOSIZE);
	cvMoveWindow(MultiShow_WinName.c_str(), (Scree_W - Disp_Img.cols) / 2+300, (Scree_H - Disp_Img.rows) / 2);//Centralize the window
	cvShowImage(MultiShow_WinName.c_str(), &(IplImage(Disp_Img)));
//	cvWaitKey(0);
	cvDestroyWindow(MultiShow_WinName.c_str());
	
}
void CCaptureDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnTimer(nIDEvent);


	if (nIDEvent == 2)
	{
		InterrogateFocus();
	}
	if (nIDEvent == 1)
	{
		// The first couple images coming out may not have good temperature data...
		// This is because the camera supervisor routine (in the API library)
		// may not have completed the camera setup procedures.
		int ImageReady = m_FG.GetImageReady();
		if (ImageReady)
		{
	
			long width, height;


			if (m_FG.GetImageSize(&width, &height) == 0)
			{
				// error... no camera or dll not loaded
				return;
			}

			tmp = m_FG.GetNextImage();
			if (tmp == NULL)
				return;

		
			//cvtColor(img, g_src, CV_BGR2GRAY);

			if (tmp)
			{
				//float fpa, lens;
				//m_FG.GetLastSensorReadings(&lens, &fpa);

				//char buf[50];
				//sprintf_s(buf, "Fpa, Lens = %.2f, %.2f", fpa, lens);
				////	SetDlgItemText(IDC_FPA_LENS, buf);


				//sprintf_s(buf, "%.1f FPS", m_FG.GetAverageFPS());
				//	SetDlgItemText(IDC_FPS, buf);


				//
				// Sample of picking off a temperature from image data
				//
				Mat g_dst;
				Ori2Mat(tmp, g_dst);
			//int i, j;
			//for (i = 0; i < dst.rows; i++)
			//{
			//	for (j = 0; j < dst.cols; j++)
			//	{
			//		int tmp = g_src.at<uchar>(i, j);

			//		dst.at<Vec3b>(i, j)[0] = 101.2 - 116.2*cos(tmp*0.08655) + 91.93*sin(tmp*0.08592);
			//		dst.at<Vec3b>(i, j)[1] = 150.9 - 110.9*cos(tmp*0.08457) - 97.33*sin(tmp*0.08457);
			//		dst.at<Vec3b>(i, j)[2] = 125.3 + 59.93*cos(tmp*0.04896) - 130.2*sin(tmp*0.04896);


			//	}
			//}

	//		namedWindow("test");

	//		imshow("test", g_dst);

			Mat g_dstImage2, g_dstImage3, g_dstImage4, tempImage;

			rot90(g_dst, tempImage);

	
			resize(tempImage, g_dstImage2, Size(tempImage.cols * 2, tempImage.rows * 2), (0, 0), (0, 0), INTER_LINEAR);

			pyrUp(tempImage, g_dstImage3, Size(tempImage.cols * 2, tempImage.rows * 2));
	//		pyrDown(tempImage, g_dstImage4, Size(tempImage.cols / 2, tempImage.rows / 2));

	//		imshow("��Ч��ͼ��resize�Ŵ�", g_dstImage2);
			imshow("view", g_dstImage3);
			int Scree_W = GetSystemMetrics(SM_CXSCREEN);
			int Scree_H = GetSystemMetrics(SM_CYSCREEN);

			cvMoveWindow("view", (Scree_W - g_dstImage3.cols) / 2 - 300, (Scree_H - g_dstImage3.rows) / 2 - 50);
	//		imshow("��Ч��ͼ�����²�����", g_dstImage4);
			//Mat img_show;  //�������ź��ͼ��
			//CRect rect;
			//GetDlgItem(IDC_PIC)->GetClientRect(&rect); //��ȡͼ����ʾ��

			//resize(dst, img_show, Size(rect.Width(), rect.Height()), 0, 0);

			//imshow("view", img_show);
			}
		}
	}
}



void CCaptureDlg::OnBnClickedInit()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


//void CCaptureDlg::OnBnClickedRot()
//{
//	// TODO:  �ڴ���ӿؼ�֪ͨ����������
//	m_CtrlCom.put_Output(COleVariant("#2100\r\n")); //��������
//}
BEGIN_EVENTSINK_MAP(CCaptureDlg, CDialogEx)
	ON_EVENT(CCaptureDlg, IDC_MSCOMM1, 1, CCaptureDlg::OnOncommMscomm1, VTS_NONE)
END_EVENTSINK_MAP()


void CCaptureDlg::OnOncommMscomm1()
{
	// TODO:  �ڴ˴������Ϣ����������
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len, k;
	BYTE rxdata[2048]; //����BYTE���� An 8-bit integerthat is not signed.
	CString strtemp;
	if (m_CtrlCom.get_CommEvent() == 2) //�¼�ֵΪ2��ʾ���ջ����������ַ�
	{              ////////��������Ը����Լ���ͨ��Э����봦�����
		variant_inp = m_CtrlCom.get_Input(); //��������
		safearray_inp = variant_inp; //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���
		len = safearray_inp.GetOneDimSize(); //�õ���Ч���ݳ���
		for (k = 0; k<len; k++)
			safearray_inp.GetElement(&k, rxdata + k);//ת��ΪBYTE������
		for (k = 0; k<len; k++) //������ת��ΪCstring�ͱ���
		{
			BYTE bt = *(char*)(rxdata + k); //�ַ���
			strtemp.Format("%c", bt); //���ַ�������ʱ����strtemp���
			m_sRXDATA += strtemp; //������ձ༭���Ӧ�ַ��� 
			UpdateData(FALSE);
		}
	}
	SetDlgItemText(IDC_EDIT_RXDATA, m_sRXDATA);
}


void CCaptureDlg::OnBnClickedReadcard()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	ifstream fin("info.txt");
	string str1, str2, str3,  str5;
	int age;
	fin >> str1 >> str2 >> str3 >> age>> str5;
	m_PID=m_cardNO = str1.c_str();
	m_name = str2.c_str();
	m_sex = str3.c_str();
	m_age = age;
	m_ID = str5.c_str();
	UpdateData(FALSE);

	pic_num = 0;
		
}


void CCaptureDlg::OnBnClickedAdj()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_FG.NucCorrection(GetSafeHwnd());
}


BOOL CCaptureDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_LBUTTONDOWN)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_ROT)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\002\00100\r\n"));//��ť����,��ת
		}
		if (pMsg->hwnd == GetDlgItem(IDC_ROT2)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\002\00200\r\n"));//��ť����,��ת
		}
		if (pMsg->hwnd == GetDlgItem(IDC_UP)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\003\00100\r\n"));//��ť����,��ת
		}
		if (pMsg->hwnd == GetDlgItem(IDC_DOWN)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\003\00200\r\n"));//��ť����,��ת
		}
		if (pMsg->hwnd == GetDlgItem(IDC_LEFT)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\001\00100\r\n"));//��ť����,��ת
		}
		if (pMsg->hwnd == GetDlgItem(IDC_RIGHT)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\001\00200\r\n"));//��ť����,��ת
		}
	}
	else if (pMsg->message == WM_LBUTTONUP)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_ROT)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\002\00300\r\n"));//ת̨02
		}
		if (pMsg->hwnd == GetDlgItem(IDC_ROT2)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\002\00300\r\n"));//
		}
		if (pMsg->hwnd == GetDlgItem(IDC_UP)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\003\00300\r\n"));//����03
		}
		if (pMsg->hwnd == GetDlgItem(IDC_DOWN)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\003\00300\r\n"));//
		}
		if (pMsg->hwnd == GetDlgItem(IDC_LEFT)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\001\00300\r\n"));//����01
		}
		if (pMsg->hwnd == GetDlgItem(IDC_RIGHT)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\001\00300\r\n"));//
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}




void CCaptureDlg::OnBnClickedP1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	m_CtrlCom.put_Output(COleVariant("#\002\00400\r\n"));//15
}


void CCaptureDlg::OnBnClickedP2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_CtrlCom.put_Output(COleVariant("#\002\00600\r\n"));//-15
}


void CCaptureDlg::OnBnClickedP3()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_CtrlCom.put_Output(COleVariant("#\002\00700\r\n"));//0
}


void CCaptureDlg::OnBnClickedP4()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_CtrlCom.put_Output(COleVariant("#\002\00500\r\n"));//180
}


void CCaptureDlg::OnBnClickedConfirm()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	unsigned short *tmp = (unsigned short*)malloc(320 * 240 * sizeof(short));
	//	ori = tmp;
	ifstream fin("E:\\IMG\\JN201706130001\\0.dat");

	for (int i = 0; i < WIDTH*HEIGHT; i++)
	{
		fin >> *(tmp + i);
		if (*(tmp + i) == ' ')
			i--;
	}
	Mat g_dst;
	Ori2Mat(tmp, g_dst);
	
	Mat g_dstImage2, g_dstImage3, g_dstImage4, tempImage;

	vector<Mat> imgs(12);
	rot90(g_dst, tempImage);


	resize(tempImage, g_dstImage2, Size(tempImage.cols * 2, tempImage.rows * 2), (0, 0), (0, 0), INTER_LINEAR);

	pyrUp(tempImage, g_dstImage3, Size(tempImage.cols * 2, tempImage.rows * 2));
	pyrDown(tempImage, g_dstImage4, Size(tempImage.cols / 2, tempImage.rows / 2));

	for (int i = 0; i < 12; i++)
		g_dstImage4.copyTo(imgs[i]);

	//		imshow("��Ч��ͼ��resize�Ŵ�", g_dstImage2);
	imshow("view", g_dstImage3);
	int Scree_W = GetSystemMetrics(SM_CXSCREEN);
	int Scree_H = GetSystemMetrics(SM_CYSCREEN);

	cvMoveWindow("view", (Scree_W - g_dstImage3.cols) / 2 - 300, (Scree_H - g_dstImage3.rows) / 2-50);

	//		imshow("��Ч��ͼ�����²�����", g_dstImage4);

			
	MultiImage_OneWin("T", imgs, cvSize(4, 3), cvSize(160, 120));

}


void CCaptureDlg::OnBnClickedTestserv()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	///////////////////////////////////////////////////
	//20170509
	m_port = "44444";
//	m_PORT.SetWindowText(m_port);
	m_ip = "119.29.233.186";
	DWORD dwIP = ntohl(inet_addr(m_ip));


	sID = "JN201709141437";
	///////////////////////////////////////////////////

//	m_PORT.GetWindowText(m_port);
//	m_IP.GetWindowText(m_ip);
	m_msg = "����IP: " + m_ip;
	m_msg.Append("\n�˿ں�: ");
	m_msg.Append(m_port);
	MessageBox(m_msg);

	if (m_cli.init(m_ip.GetBuffer(), _ttoi(m_port)))
	{
		m_msg = "���ӳɹ�";
	//	m_PORT.EnableWindow(FALSE);
	//	m_IP.EnableWindow(FALSE);
	//	mb_conn.EnableWindow(FALSE);
	}
	else
	{
		m_msg = "����ʧ��\n��ȷ��IP��˿ں�";
	}
	MessageBox(m_msg);


	//��Ȩ
	int iRet = m_cli.login_auth("test", "test@1234", 0);
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
		if (0 == iRet)
		{
			m_msg.Append("���Ի���");
		}
		else
		{
			CString sAuth = m_cli.get_auth().c_str();
			CString sRep1 = sAuth.Mid(8, 16);
			CString sRep2('*', sRep1.GetLength());
			sAuth.Replace(sRep1, sRep2);
			m_msg.Append(sAuth);
		}
	}
	MessageBox(m_msg);

	//����ͼƬ

	vecPngIDReq.clear();

	m_msg = "��� ";
	m_msg.Append(sID.c_str());
	MessageBox(m_msg);



	unsigned short *tmp = (unsigned short*)malloc(320 * 240 * sizeof(short));

	ifstream fin((LPSTR)(LPCSTR)"a.dat");
	long width = 320, height = 240;

	for (int i = 0; i < width*height; i++)
	{
		fin >> *(tmp + i);
		if (*(tmp + i) == ' ')
			i--;
	}
	fin.close();

	char sData[8][20] = { "11111", "22222", "33333", "444444", "5555", "66666", "7777", "888888" };

	for (int i = 0; i<8; ++i)
	{
		m_msg = "����ͼƬ ";
		m_msg.Append(sData[i]);
		if (!m_cli.send_png(sID, sData[i], strlen(sData[i]), vecPngIDReq))
		{
			m_msg.Append(" ʧ��\n");
			m_msg.Append(m_cli.get_msg().c_str());
			MessageBox(m_msg);
			m_cli.close();
	//		mb_login.EnableWindow(TRUE);
			return;
		}
		else
		{
			m_msg.Append(" �ɹ�");
		}
		MessageBox(m_msg);
	}

	//������Ϣ
	if (0 == vecPngIDReq.size())
	{
		m_msg = "ͼƬindex�б�Ϊ��\n���ȵ��á�����ͼƬ���ӿ�";
	}
	else
	{
		std::map<std::string, std::string> mapUserInfo;
		mapUserInfo["id"] = sID;
		mapUserInfo["name"] = "����";
		mapUserInfo["sex"] = "0";
		mapUserInfo["age"] = "18";
		mapUserInfo["identity"] = "337788";
		mapUserInfo["pic"] = vec_join(vecPngIDReq, ',');

		if (!m_cli.send_info(mapUserInfo))
		{
			m_msg = "�����û���Ϣʧ��\n";
			m_msg.Append(m_cli.get_msg().c_str());
			m_cli.close();
		//	mb_conn.EnableWindow(TRUE);
		}
		else
		{
			m_msg = "�����û���Ϣ�ɹ�";
		}
	}
	MessageBox(m_msg);


	//��ȡ��Ϣ
	std::map<std::string, std::string> mapUserInfoResp;
	int ret = m_cli.get_info(sID, mapUserInfoResp);
	if (-1 == ret)
	{
		m_msg = "��ȡ�û���Ϣʧ��\n";
		m_msg.Append(m_cli.get_msg().c_str());
		m_cli.close();
		//mb_conn.EnableWindow(TRUE);
	}
	else if (0 == ret)
	{
		m_msg = "��ȡ�û���ϢΪ��";
	}
	else
	{
		m_msg = "��ȡ�û���Ϣ�ɹ�";
		MessageBox(m_msg);
		std::map<std::string, std::string>::iterator it = mapUserInfoResp.begin();
		for (; it != mapUserInfoResp.end(); ++it)
		{
			m_msg = "��ȡ ";
			m_msg.Append(it->first.c_str());
			m_msg.Append(_T("\n"));
			m_msg.Append(it->second.c_str());
		//	MessageBox(m_msg);
		}

		it = mapUserInfoResp.begin();
		CString rectmp;
			rectmp=it->second.c_str();
			m_age = atoi(rectmp);
			UpdateData(FALSE);

	

		if (mapUserInfoResp.end() != mapUserInfoResp.find("pic"))
		{
			vecPngIDResp.clear();
			int size = split_vec(mapUserInfoResp["pic"].c_str(), vecPngIDResp, ',');
		}
		return;
	}
	MessageBox(m_msg);





}
