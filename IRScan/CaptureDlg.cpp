// CaptureDlg.cpp : 实现文件
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


// CCaptureDlg 对话框

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


// CCaptureDlg 消息处理程序


void CCaptureDlg::OnBnClickedCon()
{
	// TODO:  在此添加控件通知处理程序代码
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
	dlg.m_ofn.lpstrTitle = _T("打开图像文件"); //对话框标题
	dlg.m_ofn.lpstrInitialDir = "E:\\work\\实验室\\红外扫描\\ICI获取的图像"; //默认打开路径
	dlg.m_ofn.lpstrFilter = "bmp (*.bmp)\0*.bmp\0 jpg (*.jpg)\0*.jpg\0 All Files (*.*) \0*.*\0\0"; //打开文件类型

	if (dlg.DoModal() != IDOK)             // 判断是否获得图片
	return;
	m_path = dlg.GetPathName();

	Mat src;

	//多字节字符集下 CString 转 char*  (LPSTR)(LPCSTR)
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
	Mat img_show;  //保存缩放后的图像
	CRect rect;
	GetDlgItem(IDC_PIC)->GetClientRect(&rect); //获取图像显示区

	resize(dst, img_show, Size(rect.Width(), rect.Height()), 0, 0);

	imshow("view", img_show);
	
*/

}


BOOL CCaptureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	/*namedWindow("view", WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle("view");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_PIC)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);*/
	

	if (m_CtrlCom.get_PortOpen())
	{
		m_CtrlCom.put_PortOpen(FALSE);
	}
	m_CtrlCom.put_CommPort(3);                //选择com3，可以根据具体情况更改  
	m_CtrlCom.put_InBufferSize(1024);         //设置输入缓冲区的大小，Bytes  
	m_CtrlCom.put_OutBufferSize(1024);        //设置输出缓冲区的大小，Bytes  
	m_CtrlCom.put_Settings(_T("9600,n,8,1")); //波特率9600，无校验，8个数据位，停止位1  
	m_CtrlCom.put_InputMode(1);               //1:表示以二进制方式检索数据  
	m_CtrlCom.put_RThreshold(1);              //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件  
	m_CtrlCom.put_InputLen(0);                //设置当前接收区长度是0  
	if (!m_CtrlCom.get_PortOpen())
	{
		m_CtrlCom.put_PortOpen(TRUE);
	}
	else
	{
		AfxMessageBox(_T("Can not open serial port!"));
	}
	m_CtrlCom.get_Input();                    //先预读缓冲区以清除残留数据  
	UpdateData(FALSE);



	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CCaptureDlg::OnBnClickedSave()
{
	// TODO:  在此添加控件通知处理程序代码
////	16bit图像保存

		if (tmp == NULL)
			return;
	//	KillTimer(1);
	//	KillTimer(2);

	//	CString strPath = "E:\\IMG\\SUN-0719";
		CString strPath = "E:\\IMG\\" + m_PID;//此处可随意定义，但格式必须与所示一致，会依次创建所有的，如果已经创建好了，则不创建
		CString strWPath = strPath;
		CString strTemp;
		if (!PathFileExists(strPath))//文件夹不存在则创建
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
	// TODO:  在此添加消息处理程序代码和/或调用默认值

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

	//		imshow("【效果图二resize放大】", g_dstImage2);
			imshow("view", g_dstImage3);
			int Scree_W = GetSystemMetrics(SM_CXSCREEN);
			int Scree_H = GetSystemMetrics(SM_CYSCREEN);

			cvMoveWindow("view", (Scree_W - g_dstImage3.cols) / 2 - 300, (Scree_H - g_dstImage3.rows) / 2 - 50);
	//		imshow("【效果图二向下采样】", g_dstImage4);
			//Mat img_show;  //保存缩放后的图像
			//CRect rect;
			//GetDlgItem(IDC_PIC)->GetClientRect(&rect); //获取图像显示区

			//resize(dst, img_show, Size(rect.Width(), rect.Height()), 0, 0);

			//imshow("view", img_show);
			}
		}
	}
}



void CCaptureDlg::OnBnClickedInit()
{
	// TODO:  在此添加控件通知处理程序代码
}


//void CCaptureDlg::OnBnClickedRot()
//{
//	// TODO:  在此添加控件通知处理程序代码
//	m_CtrlCom.put_Output(COleVariant("#2100\r\n")); //发送数据
//}
BEGIN_EVENTSINK_MAP(CCaptureDlg, CDialogEx)
	ON_EVENT(CCaptureDlg, IDC_MSCOMM1, 1, CCaptureDlg::OnOncommMscomm1, VTS_NONE)
END_EVENTSINK_MAP()


void CCaptureDlg::OnOncommMscomm1()
{
	// TODO:  在此处添加消息处理程序代码
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len, k;
	BYTE rxdata[2048]; //设置BYTE数组 An 8-bit integerthat is not signed.
	CString strtemp;
	if (m_CtrlCom.get_CommEvent() == 2) //事件值为2表示接收缓冲区内有字符
	{              ////////以下你可以根据自己的通信协议加入处理代码
		variant_inp = m_CtrlCom.get_Input(); //读缓冲区
		safearray_inp = variant_inp; //VARIANT型变量转换为ColeSafeArray型变量
		len = safearray_inp.GetOneDimSize(); //得到有效数据长度
		for (k = 0; k<len; k++)
			safearray_inp.GetElement(&k, rxdata + k);//转换为BYTE型数组
		for (k = 0; k<len; k++) //将数组转换为Cstring型变量
		{
			BYTE bt = *(char*)(rxdata + k); //字符型
			strtemp.Format("%c", bt); //将字符送入临时变量strtemp存放
			m_sRXDATA += strtemp; //加入接收编辑框对应字符串 
			UpdateData(FALSE);
		}
	}
	SetDlgItemText(IDC_EDIT_RXDATA, m_sRXDATA);
}


void CCaptureDlg::OnBnClickedReadcard()
{
	// TODO:  在此添加控件通知处理程序代码
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
	// TODO:  在此添加控件通知处理程序代码
	m_FG.NucCorrection(GetSafeHwnd());
}


BOOL CCaptureDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->message == WM_LBUTTONDOWN)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_ROT)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\002\00100\r\n"));//按钮按下,正转
		}
		if (pMsg->hwnd == GetDlgItem(IDC_ROT2)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\002\00200\r\n"));//按钮按下,反转
		}
		if (pMsg->hwnd == GetDlgItem(IDC_UP)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\003\00100\r\n"));//按钮按下,正转
		}
		if (pMsg->hwnd == GetDlgItem(IDC_DOWN)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\003\00200\r\n"));//按钮按下,反转
		}
		if (pMsg->hwnd == GetDlgItem(IDC_LEFT)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\001\00100\r\n"));//按钮按下,正转
		}
		if (pMsg->hwnd == GetDlgItem(IDC_RIGHT)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\001\00200\r\n"));//按钮按下,反转
		}
	}
	else if (pMsg->message == WM_LBUTTONUP)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_ROT)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\002\00300\r\n"));//转台02
		}
		if (pMsg->hwnd == GetDlgItem(IDC_ROT2)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\002\00300\r\n"));//
		}
		if (pMsg->hwnd == GetDlgItem(IDC_UP)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\003\00300\r\n"));//上下03
		}
		if (pMsg->hwnd == GetDlgItem(IDC_DOWN)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\003\00300\r\n"));//
		}
		if (pMsg->hwnd == GetDlgItem(IDC_LEFT)->m_hWnd)
		{
			m_CtrlCom.put_Output(COleVariant("#\001\00300\r\n"));//左右01
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
	// TODO:  在此添加控件通知处理程序代码

	m_CtrlCom.put_Output(COleVariant("#\002\00400\r\n"));//15
}


void CCaptureDlg::OnBnClickedP2()
{
	// TODO:  在此添加控件通知处理程序代码
	m_CtrlCom.put_Output(COleVariant("#\002\00600\r\n"));//-15
}


void CCaptureDlg::OnBnClickedP3()
{
	// TODO:  在此添加控件通知处理程序代码
	m_CtrlCom.put_Output(COleVariant("#\002\00700\r\n"));//0
}


void CCaptureDlg::OnBnClickedP4()
{
	// TODO:  在此添加控件通知处理程序代码
	m_CtrlCom.put_Output(COleVariant("#\002\00500\r\n"));//180
}


void CCaptureDlg::OnBnClickedConfirm()
{
	// TODO:  在此添加控件通知处理程序代码
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

	//		imshow("【效果图二resize放大】", g_dstImage2);
	imshow("view", g_dstImage3);
	int Scree_W = GetSystemMetrics(SM_CXSCREEN);
	int Scree_H = GetSystemMetrics(SM_CYSCREEN);

	cvMoveWindow("view", (Scree_W - g_dstImage3.cols) / 2 - 300, (Scree_H - g_dstImage3.rows) / 2-50);

	//		imshow("【效果图二向下采样】", g_dstImage4);

			
	MultiImage_OneWin("T", imgs, cvSize(4, 3), cvSize(160, 120));

}


void CCaptureDlg::OnBnClickedTestserv()
{
	// TODO:  在此添加控件通知处理程序代码

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
	m_msg = "连接IP: " + m_ip;
	m_msg.Append("\n端口号: ");
	m_msg.Append(m_port);
	MessageBox(m_msg);

	if (m_cli.init(m_ip.GetBuffer(), _ttoi(m_port)))
	{
		m_msg = "连接成功";
	//	m_PORT.EnableWindow(FALSE);
	//	m_IP.EnableWindow(FALSE);
	//	mb_conn.EnableWindow(FALSE);
	}
	else
	{
		m_msg = "连接失败\n请确认IP或端口号";
	}
	MessageBox(m_msg);


	//授权
	int iRet = m_cli.login_auth("test", "test@1234", 0);
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
		if (0 == iRet)
		{
			m_msg.Append("测试环境");
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

	//发送图片

	vecPngIDReq.clear();

	m_msg = "编号 ";
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
		m_msg = "发送图片 ";
		m_msg.Append(sData[i]);
		if (!m_cli.send_png(sID, sData[i], strlen(sData[i]), vecPngIDReq))
		{
			m_msg.Append(" 失败\n");
			m_msg.Append(m_cli.get_msg().c_str());
			MessageBox(m_msg);
			m_cli.close();
	//		mb_login.EnableWindow(TRUE);
			return;
		}
		else
		{
			m_msg.Append(" 成功");
		}
		MessageBox(m_msg);
	}

	//发送信息
	if (0 == vecPngIDReq.size())
	{
		m_msg = "图片index列表为空\n请先调用「发送图片」接口";
	}
	else
	{
		std::map<std::string, std::string> mapUserInfo;
		mapUserInfo["id"] = sID;
		mapUserInfo["name"] = "张三";
		mapUserInfo["sex"] = "0";
		mapUserInfo["age"] = "18";
		mapUserInfo["identity"] = "337788";
		mapUserInfo["pic"] = vec_join(vecPngIDReq, ',');

		if (!m_cli.send_info(mapUserInfo))
		{
			m_msg = "发送用户信息失败\n";
			m_msg.Append(m_cli.get_msg().c_str());
			m_cli.close();
		//	mb_conn.EnableWindow(TRUE);
		}
		else
		{
			m_msg = "发送用户信息成功";
		}
	}
	MessageBox(m_msg);


	//获取信息
	std::map<std::string, std::string> mapUserInfoResp;
	int ret = m_cli.get_info(sID, mapUserInfoResp);
	if (-1 == ret)
	{
		m_msg = "获取用户信息失败\n";
		m_msg.Append(m_cli.get_msg().c_str());
		m_cli.close();
		//mb_conn.EnableWindow(TRUE);
	}
	else if (0 == ret)
	{
		m_msg = "获取用户信息为空";
	}
	else
	{
		m_msg = "获取用户信息成功";
		MessageBox(m_msg);
		std::map<std::string, std::string>::iterator it = mapUserInfoResp.begin();
		for (; it != mapUserInfoResp.end(); ++it)
		{
			m_msg = "获取 ";
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
