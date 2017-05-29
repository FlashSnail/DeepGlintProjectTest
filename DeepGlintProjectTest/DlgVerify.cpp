// DlgVerify.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DeepGlintProjectTest.h"
#include "DlgVerify.h"
#include "afxdialogex.h"

static int lastLabel = 10000;
// CDlgVerify �Ի���

IMPLEMENT_DYNAMIC(CDlgVerify, CDialogEx)

CDlgVerify::CDlgVerify(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgVerify::IDD, pParent)
	, m_result(_T(""))
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


// CDlgVerify ��Ϣ�������


void CDlgVerify::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnClose();
}


void CDlgVerify::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO:  �ڴ˴������Ϣ����������
}


//LRESULT CDlgVerify::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	// TODO:  �ڴ����ר�ô����/����û���
//
//	return CDialogEx::DefWindowProc(message, wParam, lParam);
//}


void CDlgVerify::OnTimer(UINT_PTR nIDEvent)
{
	//��ʾ����ͷ
	CvvImage m_CvvImage;
	IplImage* m_Frame;
	m_Frame = cvQueryFrame(capture);
	switch (nIDEvent)
	{
	case 1:			
		detect_and_draw(m_Frame);//�������
		if (first){
			detect();
			first = false;
		}
		break;
	case 2:
		// �ж��Ǽ�⻹��ʶ������
	//	recog_and_draw(m_Frame);//����ʶ������
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
	//��ʼ��
	pwnd = GetDlgItem(IDC_VIDEO);
	pDC = pwnd->GetDC();
	hDC = pDC->GetSafeHdc();
	pwnd->GetClientRect(&rect);
	if (read_img_number()>0)
		bool_picNum = true;
	else
		bool_picNum = false;

	return TRUE;  // return TRUE  unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CDlgVerify::OnBnClickedBtnBegin()
{
	//==============================================
	//������ͷ
	if (!capture)
	{
		capture = cvCaptureFromCAM(0);
	}

	if (!capture)
	{
		AfxMessageBox(_T("�޷�������ͷ"));
		return;
	}
	// ����
	IplImage* m_Frame;
	m_Frame = cvQueryFrame(capture);
	CvvImage m_CvvImage;
	m_CvvImage.CopyOf(m_Frame, 1);
	if (true)
	{
		m_CvvImage.DrawToHDC(hDC, &rect);
	}

	// ���ü�ʱ��,ÿ10ms����һ���¼�
	SetTimer(1, 10, NULL);	//���
	SetTimer(2, 8000, NULL);	//ʶ��һ��ʶ��Ҫ��3s����

	cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name, 0, 0, 0); // ���ط����� 
	if (!cascade)
	{
		MessageBox(_T("�޷����ط������ļ�����ȷ�ϣ�"));
	}
	storage = cvCreateMemStorage(0); // �����ڴ�洢�� 

	capture = cvCaptureFromCAM(!input_name ? 0 : input_name[0] - '0'); // ������Ƶ��ȡ�ṹ 
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
		// �ļ������⣬����ɶҲ�������ˣ��˳���
		exit(1);
	}

	//���û�ж����㹻��ͼƬ�����˳�
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
	//�ر�����ͷ
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
		CWnd* pwnd = GetDlgItem(IDC_EDIT_RESULT); // CEdit������  
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
	str = _T("ʶ���С�����");
	if (predicted_confidence <= dConfidence){
		if (lastLabel == 10000)	//10000�������õı�־������û��label��Ϣ
		{
			lastLabel = predictedLabel;
		}
		else
		{
			if (lastLabel != predictedLabel)	//����������μ���label����ͬ�����ʼ������ͬ�����
				lastLabel = 10000;
			else
			{
				if (predictedLabel == 1)
				{
					str = _T("����");
				}
				else if (predictedLabel == 2)
				{
					str = _T("Qi");
				}
				else
				{
					str = _T("δ�ڿ���");
				}
			}
		}
	}
	else
	{
		str = _T("δ�ڿ���");
	}
	UpdateData(true);
	m_result = str;
	GetDlgItem(IDC_EDIT_RESULT)->SetWindowTextW(str);
	UpdateData(false);
}