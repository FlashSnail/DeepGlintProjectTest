#pragma once
//=============================================
#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include "detect_recog.h"
#include <conio.h>
#include "afxwin.h"
//=============================================

// CDlgSample 对话框

class CDlgSample : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSample)

public:
	CDlgSample(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSample();

// 对话框数据
	enum { IDD = IDD_DLG_SAMPLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnCamon();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnCamoff();
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnTakepic();
//================================================
//私有成员变量
private:
	bool bool_cameOpen;//全局变量 标志摄像头是否打开
	bool bool_picNum;//全局变量 标志训练图片是否为空
	bool bool_detec_reco;//全局变量 

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
//私有成员函数
private:

//================================================
public:
	CEdit m_ctlTips;
	CString m_valTips;
	afx_msg void OnBnClickedBtnClearpic();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
