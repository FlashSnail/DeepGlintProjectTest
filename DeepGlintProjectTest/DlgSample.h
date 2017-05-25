#pragma once
//=============================================
#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include "detect_recog.h"
#include <conio.h>
#include "afxwin.h"
//=============================================

// CDlgSample �Ի���

class CDlgSample : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSample)

public:
	CDlgSample(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSample();

// �Ի�������
	enum { IDD = IDD_DLG_SAMPLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnCamon();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnCamoff();
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnTakepic();
//================================================
//˽�г�Ա����
private:
	bool bool_cameOpen;//ȫ�ֱ��� ��־����ͷ�Ƿ��
	bool bool_picNum;//ȫ�ֱ��� ��־ѵ��ͼƬ�Ƿ�Ϊ��
	bool bool_detec_reco;//ȫ�ֱ��� 

	CvCapture* capture;
	CRect rect;
	CDC *pDC;
	HDC hDC;
	CWnd *pwnd;
	const char* input_name;

	CString strConfidence = _T("70");
	CEdit* pEdtConfidence;
	CString strTip = _T("");
	CEdit* pTip;

	CvVideoWriter* writer;
	int use_nested_cascade;
	const char* cascade_name;
	const char* nested_cascade_name;


	double facethreshold;
	const char* scale_opt;
	int scale_opt_len;
	const char* cascade_opt;
	int cascade_opt_len;
	const char* nested_cascade_opt;
	int nested_cascade_opt_len;
	int i;
//================================================

//================================================
//˽�г�Ա����
private:

//================================================
public:
	CEdit m_ctlTips;
	CString m_valTips;
	afx_msg void OnBnClickedBtnClearpic();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
