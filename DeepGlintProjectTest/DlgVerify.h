#pragma once
//=============================================
#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include "detect_recog.h"
#include <conio.h>
#include "afxwin.h"
#include "resource.h"
//=============================================

// CDlgVerify �Ի���

class CDlgVerify : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVerify)

public:
	CDlgVerify(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgVerify();

// �Ի�������
	enum { IDD = IDD_DLG_VERIFY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
//	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();

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
	int num_components;
//	int recog_time;
	bool first;
//================================================

//================================================
//˽�г�Ա����
private:
	void detect();
//================================================
public:
	afx_msg void OnBnClickedBtnBegin();
	afx_msg void OnBnClickedBtnEnd();
	CString m_result;
	void setResult(CString str);
};
