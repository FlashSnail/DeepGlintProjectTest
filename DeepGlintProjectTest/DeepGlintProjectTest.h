
// DeepGlintProjectTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDeepGlintProjectTestApp: 
// �йش����ʵ�֣������ DeepGlintProjectTest.cpp
//

class CDeepGlintProjectTestApp : public CWinApp
{
public:
	CDeepGlintProjectTestApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDeepGlintProjectTestApp theApp;