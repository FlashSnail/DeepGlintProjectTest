// DlgSample.cpp : 实现文件
//

#include "stdafx.h"
#include "DeepGlintProjectTest.h"
#include "DlgSample.h"
#include "afxdialogex.h"

//======================================================
cv::Ptr<cv::FaceRecognizer> model = cv::createLBPHFaceRecognizer();//LBP的这个方法在单个人脸验证方面效果最好
vector<Mat> images;
vector<int> labels;
double scale;
double dConfidence;//置信度
int num_components;
IplImage *resizeRes;//存放检测到的人脸
IplImage* faceGray;//存放检测到的人脸 灰度图像
CvMemStorage* storage = 0;
CvHaarClassifierCascade* cascade = 0;
CvHaarClassifierCascade* nested_cascade = 0;
int predictedLabel;
//======================================================

// CDlgSample 对话框

IMPLEMENT_DYNAMIC(CDlgSample, CDialogEx)

CDlgSample::CDlgSample(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSample::IDD, pParent)
	, m_valTips(_T(""))
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
	//====================================================
}

CDlgSample::~CDlgSample()
{
}

void CDlgSample::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TIPS, m_ctlTips);
	DDX_Text(pDX, IDC_EDIT_TIPS, m_valTips);
}


BEGIN_MESSAGE_MAP(CDlgSample, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CAMON, &CDlgSample::OnBnClickedBtnCamon)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CAMOFF, &CDlgSample::OnBnClickedBtnCamoff)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_TAKEPIC, &CDlgSample::OnBnClickedBtnTakepic)
	ON_BN_CLICKED(IDC_BTN_CLEARPIC, &CDlgSample::OnBnClickedBtnClearpic)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CDlgSample 消息处理程序


void CDlgSample::OnBnClickedBtnCamon()
{
	//=========================================================
	// 打开摄像头响应
	if (!capture)
	{
		capture = cvCaptureFromCAM(0);
	}

	if (!capture)
	{
		AfxMessageBox(_T("无法打开摄像头"));
		return;
	}
	IplImage* m_Frame;
	m_Frame = cvQueryFrame(capture);
	CvvImage m_CvvImage;
	m_CvvImage.CopyOf(m_Frame, 1);
	if (true)
	{
		m_CvvImage.DrawToHDC(hDC, &rect);
	}

	// 设置计时器,每10ms触发一次事件
	SetTimer(1, 10, NULL);


	cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name, 0, 0, 0); // 加载分类器 
	if (!cascade)
	{
		MessageBox(_T("无法加载分类器文件，请确认！"));
	}
	storage = cvCreateMemStorage(0); // 创建内存存储器 

	capture = cvCaptureFromCAM(!input_name ? 0 : input_name[0] - '0'); // 创建视频读取结构 

	GetDlgItem(IDC_BTN_CAMON)->EnableWindow(false);
	GetDlgItem(IDC_BTN_CAMOFF)->EnableWindow(true);
	GetDlgItem(IDC_BTN_TAKEPIC)->EnableWindow(true);
	bool_detec_reco = false;
	bool_cameOpen = true;
	//=========================================================
}


BOOL CDlgSample::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//===================================================
	pTip = (CEdit*)GetDlgItem(IDC_EDIT_TIPS);
	pTip->SetWindowText(strTip);
	pwnd = GetDlgItem(IDC_IMAGE);
	pDC = pwnd->GetDC();
	hDC = pDC->GetSafeHdc();
	pwnd->GetClientRect(&rect);
	if (read_img_number()>0)
		bool_picNum = true;
	else
		bool_picNum = false;
	//===================================================

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CDlgSample::OnTimer(UINT_PTR nIDEvent)
{
	//=========================================================
	IplImage* m_Frame;
	m_Frame = cvQueryFrame(capture);
	//判断是检测还是识别人脸
	if (bool_cameOpen)
	{
		if (!bool_detec_reco)//false只为识别
		{
			detect_and_draw(m_Frame);//检测人脸
		}
		else if (bool_picNum)//false代表训练图片为空
			recog_and_draw(m_Frame);//检测和识别人脸
	}
	CvvImage m_CvvImage;
	m_CvvImage.CopyOf(m_Frame, 1);
	if (true)
	{
		m_CvvImage.DrawToHDC(hDC, &rect);
	}
	//=========================================================
	CDialogEx::OnTimer(nIDEvent);
}


void CDlgSample::OnBnClickedBtnCamoff()
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
	GetDlgItem(IDC_BTN_CAMON)->EnableWindow(true);
	GetDlgItem(IDC_BTN_CAMOFF)->EnableWindow(false);
	GetDlgItem(IDC_BTN_TAKEPIC)->EnableWindow(false);
	bool_cameOpen = false;
	//=========================================================
}


void CDlgSample::OnClose()
{
	//=========================================================
	//关闭窗体
	cvReleaseCapture(&capture);
	CDialogEx::OnClose();
	//=========================================================
}


void CDlgSample::OnBnClickedBtnTakepic()
{
	//=========================================================
	// 拍照取样
	if (!faceGray)
	{
		CString tipPhoto = strTip + "\r\n拍照失败，请将摄像头对准人脸";
		m_valTips = tipPhoto;
		UpdateData(false);
		return;
	}
	Mat img(faceGray, 0);
	stringstream ss;
	ss << (read_img_number() + 1);
	string faceImgName = "..//einfacedata//trainingdata//" + ss.str() + ".jpg";
	imwrite(faceImgName, img);

	CString tipPhoto = strTip + "\r\n拍照成功！已存为" + ("/einfacedata/trainingdata/" + ss.str() + ".jpg").c_str();
	m_valTips = tipPhoto;
	UpdateData(false);
	//=========================================================
}


void CDlgSample::OnBnClickedBtnClearpic()
{
	//=========================================================
	// 清除样本
	if (delete_img())
	{
		CString tipPhoto = _T("\r\n删除成功！");
		m_valTips = tipPhoto;
		UpdateData(false);
		bool_detec_reco = false;
		bool_picNum = false;
	}
	else
	{
		CString tipPhoto = _T("\r\n删除失败！");
		m_valTips = tipPhoto;
		UpdateData(false);
	}
	//=========================================================
}


int CDlgSample::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}
