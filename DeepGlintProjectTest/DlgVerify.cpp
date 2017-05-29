// DlgVerify.cpp : 实现文件
//

#include "stdafx.h"
#include "DeepGlintProjectTest.h"
#include "DlgVerify.h"
#include "afxdialogex.h"

static int lastLabel = 10000;
// CDlgVerify 对话框

IMPLEMENT_DYNAMIC(CDlgVerify, CDialogEx)

CDlgVerify::CDlgVerify(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgVerify::IDD, pParent)
	, m_result(_T(""))
{
	//====================================================
	//初始化
	writer = 0;

	bool_cameOpen = false;//全局变量 标志摄像头是否打开
	bool_picNum = false;//全局变量 标志训练图片是否为空
	bool_detec_reco = false;//全局变量 
	dConfidence = 0;//置信度
	use_nested_cascade = 0;
	cascade_name = "../data/haarcascades/haarcascade_frontalface_alt.xml";
	nested_cascade_name = "../data/haarcascade_eye_tree_eyeglasses.xml";
	scale = 1;
	num_components = 9;
	facethreshold = 9.0;


	IplImage *frame, *frame_copy = 0;
	IplImage *image = 0;
	const char* scale_opt = "--scale="; // 分类器选项指示符号 
	scale_opt_len = (int)strlen(scale_opt);
	cascade_opt = "--cascade=";
	cascade_opt_len = (int)strlen(cascade_opt);
	nested_cascade_opt = "--nested-cascade";
	nested_cascade_opt_len = (int)strlen(nested_cascade_opt);
	input_name = 0;

	first = true;
	//====================================================
}

CDlgVerify::~CDlgVerify()
{
}

void CDlgVerify::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RESULT, m_result);
}


BEGIN_MESSAGE_MAP(CDlgVerify, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_BEGIN, &CDlgVerify::OnBnClickedBtnBegin)
	ON_BN_CLICKED(IDC_BTN_END, &CDlgVerify::OnBnClickedBtnEnd)
END_MESSAGE_MAP()


// CDlgVerify 消息处理程序


void CDlgVerify::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnClose();
}


void CDlgVerify::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO:  在此处添加消息处理程序代码
}


//LRESULT CDlgVerify::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	// TODO:  在此添加专用代码和/或调用基类
//
//	return CDialogEx::DefWindowProc(message, wParam, lParam);
//}


void CDlgVerify::OnTimer(UINT_PTR nIDEvent)
{
	//显示摄像头
	CvvImage m_CvvImage;
	IplImage* m_Frame;
	m_Frame = cvQueryFrame(capture);
	switch (nIDEvent)
	{
	case 1:			
		detect_and_draw(m_Frame);//检测人脸
		if (first){
			detect();
			first = false;
		}
		break;
	case 2:
		// 判断是检测还是识别人脸
	//	recog_and_draw(m_Frame);//检测和识别人脸
		detect();
		break;
	default:
		break;
	}
	m_CvvImage.CopyOf(m_Frame, 1);
	if (true)
	{
		m_CvvImage.DrawToHDC(hDC, &rect);
	}
	CDialogEx::OnTimer(nIDEvent);
}


BOOL CDlgVerify::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//=============================================
	//初始化
	pwnd = GetDlgItem(IDC_VIDEO);
	pDC = pwnd->GetDC();
	hDC = pDC->GetSafeHdc();
	pwnd->GetClientRect(&rect);
	if (read_img_number()>0)
		bool_picNum = true;
	else
		bool_picNum = false;

	return TRUE;  // return TRUE  unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CDlgVerify::OnBnClickedBtnBegin()
{
	//==============================================
	//打开摄像头
	if (!capture)
	{
		capture = cvCaptureFromCAM(0);
	}

	if (!capture)
	{
		AfxMessageBox(_T("无法打开摄像头"));
		return;
	}
	// 测试
	IplImage* m_Frame;
	m_Frame = cvQueryFrame(capture);
	CvvImage m_CvvImage;
	m_CvvImage.CopyOf(m_Frame, 1);
	if (true)
	{
		m_CvvImage.DrawToHDC(hDC, &rect);
	}

	// 设置计时器,每10ms触发一次事件
	SetTimer(1, 10, NULL);	//检测
	SetTimer(2, 8000, NULL);	//识别，一次识别要卡3s左右

	cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name, 0, 0, 0); // 加载分类器 
	if (!cascade)
	{
		MessageBox(_T("无法加载分类器文件，请确认！"));
	}
	storage = cvCreateMemStorage(0); // 创建内存存储器 

	capture = cvCaptureFromCAM(!input_name ? 0 : input_name[0] - '0'); // 创建视频读取结构 
	bool_detec_reco = false;
	bool_cameOpen = true;
	GetDlgItem(IDC_BTN_BEGIN)->EnableWindow(false);
	GetDlgItem(IDC_BTN_END)->EnableWindow(true);

	images.clear();
	labels.clear();
	dConfidence = 70;
	model->set("threshold", dConfidence);
	if (!read_img(images, labels))
	{
		AfxMessageBox(_T("Error in reading images!"));
		images.clear();
		labels.clear();
		return;
	}

	string fn_csv = "at.txt";
	try
	{
		images.clear();
		labels.clear();
		read_csv(fn_csv, images, labels);
	}
	catch (cv::Exception& e)
	{
		cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
		// 文件有问题，我们啥也做不了了，退出了
		exit(1);
	}

	//如果没有读到足够的图片，就退出
	if (images.size() < 1)
	{
		MessageBox(_T("This demo needs at least 1 images to work!"));
		return;
	}
/*	//training
	model->train(images, labels);
	model->save("MyFaceLBPHModel.xml");*/
	bool_detec_reco = true;
	bool_picNum = true;
}


void CDlgVerify::OnBnClickedBtnEnd()
{
	//=========================================================
	//关闭摄像头
	cvReleaseVideoWriter(&writer);
	cvReleaseCapture(&capture);
	CDC MemDC;
	CBitmap m_Bitmap1;
	m_Bitmap1.LoadBitmap(IDB_BITMAP1);
	MemDC.CreateCompatibleDC(NULL);
	MemDC.SelectObject(&m_Bitmap1);
	pDC->StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), &MemDC, 0, 0, 48, 48, SRCCOPY);
	GetDlgItem(IDC_BTN_BEGIN)->EnableWindow(true);
	GetDlgItem(IDC_BTN_END)->EnableWindow(false);
	bool_cameOpen = false;
	//=========================================================
}

void CDlgVerify::setResult(CString str)
{
	m_result = str;
	/*if (::IsWindow(GetSafeHwnd()))
	{
		CWnd* pwnd = GetDlgItem(IDC_EDIT_RESULT); // CEdit派生类  
		if (pwnd != NULL)
		{
			pwnd->SetWindowText(str);
		}
	}*/
	//UpdateData(false);
	AfxMessageBox(str);
}

void CDlgVerify::detect()
{
	Mat test = faceGray;
	model->train(images, labels);
	model->save("MyFaceLBPHModel.xml");
	predictedLabel = model->predict(test);
	double predicted_confidence = 0.0;
	model->predict(test, predictedLabel, predicted_confidence);
	CString str;
	str = _T("识别中。。。");
	if (predicted_confidence <= dConfidence){
		if (lastLabel == 10000)	//10000是我设置的标志，代表没有label信息
		{
			lastLabel = predictedLabel;
		}
		else
		{
			if (lastLabel != predictedLabel)	//如果相邻两次检测的label不相同，则初始化，相同则输出
				lastLabel = 10000;
			else
			{
				if (predictedLabel == 1)
				{
					str = _T("张泽华");
				}
				else if (predictedLabel == 2)
				{
					str = _T("Qi");
				}
				else
				{
					str = _T("未在库中");
				}
			}
		}
	}
	else
	{
		str = _T("未在库中");
	}
	UpdateData(true);
	m_result = str;
	GetDlgItem(IDC_EDIT_RESULT)->SetWindowTextW(str);
	UpdateData(false);
}