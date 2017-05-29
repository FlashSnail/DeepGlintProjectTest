// DlgSample.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DeepGlintProjectTest.h"
#include "DlgSample.h"
#include "afxdialogex.h"

//======================================================
cv::Ptr<cv::FaceRecognizer> model = cv::createLBPHFaceRecognizer();//LBP����������ڵ���������֤����Ч�����
vector<Mat> images;
vector<int> labels;
double scale;
double dConfidence;//���Ŷ�
int num_components;
IplImage *resizeRes;//��ż�⵽������
IplImage* faceGray;//��ż�⵽������ �Ҷ�ͼ��
CvMemStorage* storage = 0;
CvHaarClassifierCascade* cascade = 0;
CvHaarClassifierCascade* nested_cascade = 0;
int predictedLabel;
//======================================================

// CDlgSample �Ի���

IMPLEMENT_DYNAMIC(CDlgSample, CDialogEx)

CDlgSample::CDlgSample(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSample::IDD, pParent)
	, m_valTips(_T(""))
{
	//====================================================
	//��ʼ��
	writer = 0;
	
	bool_cameOpen = false;//ȫ�ֱ��� ��־����ͷ�Ƿ��
	bool_picNum = false;//ȫ�ֱ��� ��־ѵ��ͼƬ�Ƿ�Ϊ��
	bool_detec_reco = false;//ȫ�ֱ��� 
	dConfidence = 0;//���Ŷ�	
	use_nested_cascade = 0;
	cascade_name = "../data/haarcascades/haarcascade_frontalface_alt.xml";
	nested_cascade_name = "../data/haarcascade_eye_tree_eyeglasses.xml";
	scale = 1;
	num_components = 9;
	facethreshold = 9.0;
	
	
	IplImage *frame, *frame_copy = 0;
	IplImage *image = 0;
	const char* scale_opt = "--scale="; // ������ѡ��ָʾ���� 
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


// CDlgSample ��Ϣ�������


void CDlgSample::OnBnClickedBtnCamon()
{
	//=========================================================
	// ������ͷ��Ӧ
	if (!capture)
	{
		capture = cvCaptureFromCAM(0);
	}

	if (!capture)
	{
		AfxMessageBox(_T("�޷�������ͷ"));
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

	// ���ü�ʱ��,ÿ10ms����һ���¼�
	SetTimer(1, 10, NULL);


	cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name, 0, 0, 0); // ���ط����� 
	if (!cascade)
	{
		MessageBox(_T("�޷����ط������ļ�����ȷ�ϣ�"));
	}
	storage = cvCreateMemStorage(0); // �����ڴ�洢�� 

	capture = cvCaptureFromCAM(!input_name ? 0 : input_name[0] - '0'); // ������Ƶ��ȡ�ṹ 

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
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CDlgSample::OnTimer(UINT_PTR nIDEvent)
{
	//=========================================================
	IplImage* m_Frame;
	m_Frame = cvQueryFrame(capture);
	//�ж��Ǽ�⻹��ʶ������
	if (bool_cameOpen)
	{
		if (!bool_detec_reco)//falseֻΪʶ��
		{
			detect_and_draw(m_Frame);//�������
		}
		else if (bool_picNum)//false����ѵ��ͼƬΪ��
			recog_and_draw(m_Frame);//����ʶ������
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
	//�ر�����ͷ
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
	//�رմ���
	cvReleaseCapture(&capture);
	CDialogEx::OnClose();
	//=========================================================
}


void CDlgSample::OnBnClickedBtnTakepic()
{
	//=========================================================
	// ����ȡ��
	if (!faceGray)
	{
		CString tipPhoto = strTip + "\r\n����ʧ�ܣ��뽫����ͷ��׼����";
		m_valTips = tipPhoto;
		UpdateData(false);
		return;
	}
	Mat img(faceGray, 0);
	stringstream ss;
	ss << (read_img_number() + 1);
	string faceImgName = "..//einfacedata//trainingdata//" + ss.str() + ".jpg";
	imwrite(faceImgName, img);

	CString tipPhoto = strTip + "\r\n���ճɹ����Ѵ�Ϊ" + ("/einfacedata/trainingdata/" + ss.str() + ".jpg").c_str();
	m_valTips = tipPhoto;
	UpdateData(false);
	//=========================================================
}


void CDlgSample::OnBnClickedBtnClearpic()
{
	//=========================================================
	// �������
	if (delete_img())
	{
		CString tipPhoto = _T("\r\nɾ���ɹ���");
		m_valTips = tipPhoto;
		UpdateData(false);
		bool_detec_reco = false;
		bool_picNum = false;
	}
	else
	{
		CString tipPhoto = _T("\r\nɾ��ʧ�ܣ�");
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
