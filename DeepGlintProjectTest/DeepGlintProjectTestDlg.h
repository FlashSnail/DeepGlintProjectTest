
// DeepGlintProjectTestDlg.h : ͷ�ļ�
//

#pragma once
//============================================
#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include "detect_recog.h"
#include <conio.h>
//============================================

// CDeepGlintProjectTestDlg �Ի���
class CDeepGlintProjectTestDlg : public CDialogEx
{
// ����
public:
	CDeepGlintProjectTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DEEPGLINTPROJECTTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnVerify();
	afx_msg void OnBnClickedBtnSample();
};
