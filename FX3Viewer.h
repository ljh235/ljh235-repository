
// FX3Viewer.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CFX3ViewerApp:
// �� Ŭ������ ������ ���ؼ��� FX3Viewer.cpp�� �����Ͻʽÿ�.
//

class CFX3ViewerApp : public CWinApp
{
public:
	CFX3ViewerApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CFX3ViewerApp theApp;