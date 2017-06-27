
// FX3ViewerDlg.cpp : ���� ����
// 
// SourceTrr ���� ����

#include "stdafx.h"
#include "FX3Viewer.h"
#include "FX3ViewerDlg.h"
#include "afxdialogex.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream> 

#include "utils.h"

using namespace cv;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VideoCapture *g_capture;
Mat g_frame;

DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE,
	0xae18aa60, 0x7f6a, 0x11d4, 0x97, 0xdd, 0x00, 0x01, 0x02, 0x29, 0xb9, 0x59);


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.



CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFX3ViewerDlg ��ȭ ����



CFX3ViewerDlg::CFX3ViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FX3VIEWER_DIALOG, pParent)
	, m_chk_wled(FALSE)
	, m_chk_red_ld(FALSE)
	, m_chk_blue_ld(FALSE)
	, m_CyUSBDevice(NULL)
	, m_nExpourseTime(3)
	, m_nRedGain(0x4D)
	, m_nBlueGain(0x27)
	, m_CamWidth(0)
	, m_CamHeight(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFX3ViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_WLED, m_chk_wled);
	DDX_Check(pDX, IDC_CHECK_RED_LD, m_chk_red_ld);
	DDX_Check(pDX, IDC_CHECK_BLUE_LD, m_chk_blue_ld);
}

BEGIN_MESSAGE_MAP(CFX3ViewerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_WLED, &CFX3ViewerDlg::OnBnClickedCheckWled)
	ON_BN_CLICKED(IDC_CHECK_RED_LD, &CFX3ViewerDlg::OnBnClickedCheckRedLd)
	ON_BN_CLICKED(IDC_CHECK_BLUE_LD, &CFX3ViewerDlg::OnBnClickedCheckBlueLd)
	ON_BN_CLICKED(IDC_BUTTON_WRITE, &CFX3ViewerDlg::OnBnClickedButtonWrite)
	ON_BN_CLICKED(IDC_BUTTON_READ, &CFX3ViewerDlg::OnBnClickedButtonRead)
	ON_BN_CLICKED(IDC_BUTTON_EXP_SET, &CFX3ViewerDlg::OnBnClickedButtonExpSet)
	ON_BN_CLICKED(IDC_BUTTON_RED_GAIN_SET, &CFX3ViewerDlg::OnBnClickedButtonRedGainSet)
	ON_BN_CLICKED(IDC_BUTTON_BLUE_GAIN_SET, &CFX3ViewerDlg::OnBnClickedButtonBlueGainSet)
	ON_BN_CLICKED(IDC_BUTTON_ALL_SET, &CFX3ViewerDlg::OnBnClickedButtonAllSet)
	ON_BN_CLICKED(IDC_BUTTON_CAPTURE, &CFX3ViewerDlg::OnBnClickedButtonCapture)
END_MESSAGE_MAP()


// CFX3ViewerDlg �޽��� ó����

BOOL CFX3ViewerDlg::OnInitDialog()
{
	// Dialog �ʱ�ȭ
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	// ��� �޴��� �ʱ�ȭ
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	// ��ư �ʱ�ȭ // üũ�ڽ��� �ҷ��� ��밡���ϰ��ϰ� üũ�������·� ����
	(CButton *)GetDlgItem(IDC_BUTTON_WRITE)->EnableWindow(FALSE);
	(CButton *)GetDlgItem(IDC_BUTTON_READ)->EnableWindow(FALSE);
	(CButton *)GetDlgItem(IDC_CHECK_WLED)->EnableWindow(FALSE);
	(CButton *)GetDlgItem(IDC_CHECK_RED_LD)->EnableWindow(FALSE);
	(CButton *)GetDlgItem(IDC_CHECK_BLUE_LD)->EnableWindow(FALSE);

	// Cam �ʱ�ȭ
	m_pCamDC = GetDlgItem(IDC_STATIC_PIC)->GetDC();



	// Cam img �ʱ�ȭ
	m_image = new CImage();

	// Timer ����
	m_bFirstData = TRUE;
	SetTimer(TIMER_ID_WAIT_UVC, 500, NULL);	//SetTimer(Timer ID , ms, �Լ���(NULL���� OnTimer))

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CFX3ViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CFX3ViewerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ��� �� �Լ��� ȣ���մϴ�.
HCURSOR CFX3ViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFX3ViewerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CDialogEx::OnTimer(nIDEvent);

	if (nIDEvent == TIMER_ID_WAIT_UVC)
	{
		g_capture = new VideoCapture(CAM_PORT);	
		if (!g_capture->isOpened())
		{
			g_capture->release();

			LightControlClear();

			(CButton *)GetDlgItem(IDC_BUTTON_WRITE)->EnableWindow(FALSE);
			(CButton *)GetDlgItem(IDC_BUTTON_READ)->EnableWindow(FALSE);
			(CButton *)GetDlgItem(IDC_CHECK_WLED)->EnableWindow(FALSE);
			(CButton *)GetDlgItem(IDC_CHECK_RED_LD)->EnableWindow(FALSE);
			(CButton *)GetDlgItem(IDC_CHECK_BLUE_LD)->EnableWindow(FALSE);
			(CButton *)GetDlgItem(IDC_BUTTON_EXP_SET)->EnableWindow(FALSE);
			(CButton *)GetDlgItem(IDC_BUTTON_RED_GAIN_SET)->EnableWindow(FALSE);
			(CButton *)GetDlgItem(IDC_BUTTON_BLUE_GAIN_SET)->EnableWindow(FALSE);
			(CButton *)GetDlgItem(IDC_BUTTON_ALL_SET)->EnableWindow(FALSE);
		}
		else
		{
			m_bFirstData = TRUE;

			KillTimer(TIMER_ID_WAIT_UVC);
			SetTimer(TIMER_ID_RUN_UVC, 30, NULL);
			SetTimer(TIMER_ID_BULK, 100, NULL);

			m_CyUSBDevice = new CCyUSBDevice(this->m_hWnd, CYUSBDRV_GUID, true);

			(CButton *)GetDlgItem(IDC_BUTTON_WRITE)->EnableWindow(TRUE);
			(CButton *)GetDlgItem(IDC_BUTTON_READ)->EnableWindow(TRUE);
			(CButton *)GetDlgItem(IDC_CHECK_WLED)->EnableWindow(TRUE);
			(CButton *)GetDlgItem(IDC_CHECK_RED_LD)->EnableWindow(TRUE);
			(CButton *)GetDlgItem(IDC_CHECK_BLUE_LD)->EnableWindow(TRUE);
			(CButton *)GetDlgItem(IDC_BUTTON_EXP_SET)->EnableWindow(TRUE);
			(CButton *)GetDlgItem(IDC_BUTTON_RED_GAIN_SET)->EnableWindow(TRUE);
			(CButton *)GetDlgItem(IDC_BUTTON_BLUE_GAIN_SET)->EnableWindow(TRUE);
			(CButton *)GetDlgItem(IDC_BUTTON_ALL_SET)->EnableWindow(TRUE);
		}
	}
	else if (nIDEvent == TIMER_ID_RUN_UVC)
	{
		if (!g_capture->read(g_frame))
		{
			g_capture->release();
			KillTimer(TIMER_ID_RUN_UVC);
			KillTimer(TIMER_ID_BULK);
			SetTimer(TIMER_ID_WAIT_UVC, 500, NULL);

			CEdit* pEditADC = (CEdit *)GetDlgItem(IDC_EDIT_ADC);
			CEdit* pEditFrameCount = (CEdit *)GetDlgItem(IDC_EDIT_FRAME_COUNT);
			CEdit* pEditFPS = (CEdit *)GetDlgItem(IDC_EDIT_FPS);

			pEditADC->SetWindowTextW(_T(""));
			pEditFrameCount->SetWindowTextW(_T(""));
			pEditFPS->SetWindowTextW(_T(""));

			Invalidate();

			if (m_CyUSBDevice)
			{
				if (m_CyUSBDevice->IsOpen()) m_CyUSBDevice->Close();
				delete m_CyUSBDevice;
				m_CyUSBDevice = NULL;
			}

			return;
		}

		if (m_CamWidth != g_frame.cols || m_CamHeight != g_frame.rows)
		{
			CEdit* pEditResolution = (CEdit *)GetDlgItem(IDC_EDIT_RESOLUTION);

			m_CamWidth = g_frame.cols;
			m_CamHeight = g_frame.rows;

			CString strRes;
			strRes.Format(_T("%d x %d"), m_CamWidth, m_CamHeight);
			pEditResolution->SetWindowTextW(strRes);
		}

		RECT r;
		GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&r);

		int width, height;

		if (g_frame.cols > CAMERA_WIDTH_MAX || g_frame.rows > CAMERA_HEIGHT_MAX)
		{
			width = CAMERA_WIDTH_MAX;
			height = CAMERA_HEIGHT_MAX;
			cv::resize(g_frame, g_frame, cv::Size(width, height));
		}
		else
		{
			width = g_frame.cols;
			height = g_frame.rows;
		}

		cv::Size winSize(width, height);

		cv::flip(g_frame, g_frame, 0);

		BITMAPINFO bitInfo;
		bitInfo.bmiHeader.biBitCount = 24;
		bitInfo.bmiHeader.biWidth = winSize.width;
		bitInfo.bmiHeader.biHeight = winSize.height;
		bitInfo.bmiHeader.biPlanes = 1;
		bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitInfo.bmiHeader.biCompression = BI_RGB;
		bitInfo.bmiHeader.biClrImportant = 0;
		bitInfo.bmiHeader.biClrUsed = 0;
		bitInfo.bmiHeader.biSizeImage = 0;
		bitInfo.bmiHeader.biXPelsPerMeter = 0;
		bitInfo.bmiHeader.biYPelsPerMeter = 0;

		m_image->Create(winSize.width, winSize.height, 24);

		StretchDIBits(m_image->GetDC(), 0, 0,
			winSize.width, winSize.height, 0, 0,
			winSize.width, winSize.height,
			g_frame.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY);

		int x = (r.right - winSize.width) / 2;
		int y = (r.bottom - winSize.height) / 2;

		m_image->BitBlt(m_pCamDC->m_hDC, x, y);

		if (m_image)
		{
			m_image->ReleaseDC();
			m_image->Destroy();
		}
	}
	else if (nIDEvent == TIMER_ID_BULK)
	{
		UCHAR buffer[BULK_DATA_MAX];
		LONG length = 0;

		CEdit* pEditADC = (CEdit *)GetDlgItem(IDC_EDIT_ADC);
		CEdit* pEditFrameCount = (CEdit *)GetDlgItem(IDC_EDIT_FRAME_COUNT);
		CEdit* pEditFPS = (CEdit *)GetDlgItem(IDC_EDIT_FPS);

		if (m_bFirstData)
		{
			CEdit* pEditExp = (CEdit *)GetDlgItem(IDC_EDIT_EXP_TIME);
			CEdit* pEditRedGain = (CEdit *)GetDlgItem(IDC_EDIT_RED_GAIN);
			CEdit* pEditBlueGain = (CEdit *)GetDlgItem(IDC_EDIT_BLUE_GAIN);

			SetCameraInitValue();

			CString strNum;

			strNum.Format(_T("%d"), m_nExpourseTime);
			pEditExp->SetWindowTextW(strNum);

			strNum.Format(_T("%02X"), m_nRedGain);
			pEditRedGain->SetWindowTextW(strNum);

			strNum.Format(_T("%02X"), m_nBlueGain);
			pEditBlueGain->SetWindowTextW(strNum);

			m_bFirstData = FALSE;
			return;
		}

		length = 0;
		buffer[length++] = 0x03;

		SendBulkData(buffer, length, buffer, 9);

		if (buffer[0] == 0x03)
		{
			UINT adc_value = 0;
			UINT frame_count = 0;
			CString strNum;

			adc_value = buffer[1] << 24;
			adc_value |= buffer[2] << 16;
			adc_value |= buffer[3] << 8;
			adc_value |= buffer[4];

			frame_count = buffer[5] << 24;
			frame_count |= buffer[6] << 16;
			frame_count |= buffer[7] << 8;
			frame_count |= buffer[8];

			strNum.Format(_T("%d"), adc_value);
			pEditADC->SetWindowTextW(strNum);

			strNum.Format(_T("%d"), frame_count);
			pEditFrameCount->SetWindowTextW(strNum);

			strNum.Format(_T("%.2f"), update_fps(frame_count));
			pEditFPS->SetWindowTextW(strNum);
		}
	}
}


void CFX3ViewerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	g_capture->release();

	LightControlClear();

	if (m_CyUSBDevice)
	{
		if (m_CyUSBDevice->IsOpen()) m_CyUSBDevice->Close();
		delete m_CyUSBDevice;
	}

	delete m_image;
}


void CFX3ViewerDlg::OnBnClickedCheckWled()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UCHAR buffer[BULK_DATA_MAX];
	LONG length = 0;

	if (IsDlgButtonChecked(IDC_CHECK_RED_LD))
	{
		CheckDlgButton(IDC_CHECK_RED_LD, BST_UNCHECKED);

		length = 0;
		buffer[length++] = 0x02;
		buffer[length++] = LD_650;
		buffer[length++] = 0x00;
		SendBulkData(buffer, length, buffer, 2);
	}
		
	if (IsDlgButtonChecked(IDC_CHECK_BLUE_LD))
	{
		CheckDlgButton(IDC_CHECK_BLUE_LD, BST_UNCHECKED);

		length = 0;
		buffer[length++] = 0x02;
		buffer[length++] = LD_405;
		buffer[length++] = 0x00;
		SendBulkData(buffer, length, buffer, 2);
	}

	if (IsDlgButtonChecked(IDC_CHECK_WLED))
	{
		length = 0;
		buffer[length++] = 0x02;
		buffer[length++] = WHITE_LED;
		buffer[length++] = 0x01;
		SendBulkData(buffer, length, buffer, 2);
	}
	else
	{
		length = 0;
		buffer[length++] = 0x02;
		buffer[length++] = WHITE_LED;
		buffer[length++] = 0x00;
		SendBulkData(buffer, length, buffer, 2);
	}
}


void CFX3ViewerDlg::OnBnClickedCheckRedLd()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UCHAR buffer[16];
	LONG length = 0;

	if (IsDlgButtonChecked(IDC_CHECK_WLED))
	{
		CheckDlgButton(IDC_CHECK_WLED, BST_UNCHECKED);

		length = 0;
		buffer[length++] = 0x02;
		buffer[length++] = WHITE_LED;
		buffer[length++] = 0x00;

		SendBulkData(buffer, length, buffer, 2);
	}
		
	if (IsDlgButtonChecked(IDC_CHECK_BLUE_LD))
	{
		CheckDlgButton(IDC_CHECK_BLUE_LD, BST_UNCHECKED);

		length = 0;
		buffer[length++] = 0x02;
		buffer[length++] = LD_405;
		buffer[length++] = 0x00;

		SendBulkData(buffer, length, buffer, 2);
	}

	if (IsDlgButtonChecked(IDC_CHECK_RED_LD))
	{
		length = 0;
		buffer[length++] = 0x02;
		buffer[length++] = LD_650;
		buffer[length++] = 0x01;

		SendBulkData(buffer, length, buffer, 2);
	}
	else
	{
		length = 0;
		buffer[length++] = 0x02;
		buffer[length++] = LD_650;
		buffer[length++] = 0x00;

		SendBulkData(buffer, length, buffer, 2);
	}
}


void CFX3ViewerDlg::OnBnClickedCheckBlueLd()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UCHAR buffer[16];
	LONG length = 0;

	if (IsDlgButtonChecked(IDC_CHECK_WLED))
	{
		CheckDlgButton(IDC_CHECK_WLED, BST_UNCHECKED);

		length = 0;
		buffer[length++] = 0x02;
		buffer[length++] = WHITE_LED;
		buffer[length++] = 0x00;

		SendBulkData(buffer, length, buffer, 2);
	}
		
	if (IsDlgButtonChecked(IDC_CHECK_RED_LD))
	{
		CheckDlgButton(IDC_CHECK_RED_LD, BST_UNCHECKED);

		length = 0;
		buffer[length++] = 0x02;
		buffer[length++] = LD_650;
		buffer[length++] = 0x00;

		SendBulkData(buffer, length, buffer, 2);
	}

	if (IsDlgButtonChecked(IDC_CHECK_BLUE_LD))
	{
		length = 0;
		buffer[length++] = 0x02;
		buffer[length++] = LD_405;
		buffer[length++] = 0x01;

		SendBulkData(buffer, length, buffer, 2);
	}
	else
	{
		length = 0;
		buffer[length++] = 0x02;
		buffer[length++] = LD_405;
		buffer[length++] = 0x00;

		SendBulkData(buffer, length, buffer, 2);
	}
}

void CFX3ViewerDlg::OnBnClickedButtonWrite()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CEdit* pEditInput = (CEdit *)GetDlgItem(IDC_EDIT_REG_INPUT);
	CEdit* pEditOutput = (CEdit *)GetDlgItem(IDC_EDIT_REG_OUTPUT);
	CString regInput;
	UCHAR buffer[16];
	LONG length = 0;
	
	CString token;
	int pos = 0;
	int index = 0;

	pEditInput->GetWindowTextW(regInput);

	buffer[index++] = 0x01;

	while ((token = regInput.Tokenize(_T(" "), pos)) != _T(""))
	{
		unsigned int temp = 0;
		char* ptr = CString2char(token);
		asciitohex(ptr, &temp);
		buffer[index++] = temp;
		delete[] ptr;
	}
	length = index;

	if (length < 3)
		return;

	SendBulkData(buffer, length, buffer, 2);

	if (buffer[0] == 0x00)
	{
		CString strNum;
		strNum.Format(_T("%02X"), buffer[1]);
		pEditOutput->SetWindowTextW(strNum);
	}
	else
	{
		pEditOutput->SetWindowTextW(_T("Error"));
	}
}


void CFX3ViewerDlg::OnBnClickedButtonRead()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CEdit* pEditInput = (CEdit *)GetDlgItem(IDC_EDIT_REG_INPUT);
	CEdit* pEditOutput = (CEdit *)GetDlgItem(IDC_EDIT_REG_OUTPUT);
	CString regInput;
	UCHAR buffer[16];
	LONG length = 0;

	CString token;
	int pos = 0;
	int index = 0;

	pEditInput->GetWindowTextW(regInput);

	buffer[index++] = 0x00;

	while ((token = regInput.Tokenize(_T(" "), pos)) != _T(""))
	{
		unsigned int temp = 0;
		char* ptr = CString2char(token);
		asciitohex(ptr, &temp);
		buffer[index++] = temp;
		delete[] ptr;
	}
	length = index;

	if (length == 1 || length > 2)
		return;

	SendBulkData(buffer, length, buffer, 2);

	if (buffer[0] == 0x00)
	{
		CString strNum;
		strNum.Format(_T("%02X"), buffer[1]);
		pEditOutput->SetWindowTextW(strNum);
	}
	else
	{
		pEditOutput->SetWindowTextW(_T("Error"));
	}
}


BOOL CFX3ViewerDlg::SendBulkData(UCHAR* outBuf, LONG outLen, UCHAR* inBuf, LONG inLen)
{
	CCyUSBEndPoint *epBulkOut = NULL;
	CCyUSBEndPoint *epBulkIn = NULL;

	UCHAR *inContext = NULL;
	OVERLAPPED inOvLap;

	if (m_CyUSBDevice == NULL)
		return FALSE;

#if 0
	int eptCount = m_CyUSBDevice->EndPointCount();

	for (int i = 1; i < eptCount; i++)
	{
		bool bIn = ((m_CyUSBDevice->EndPoints[i]->Address & 0x80) == 0x80);
		bool bBulk = (m_CyUSBDevice->EndPoints[i]->Attributes == 2);

		if (bBulk && bIn)
			epBulkIn = (CCyBulkEndPoint *)m_CyUSBDevice->EndPoints[i];
		if (bBulk && !bIn)
			epBulkOut = (CCyBulkEndPoint *)m_CyUSBDevice->EndPoints[i];
	}
#else
	epBulkIn = m_CyUSBDevice->BulkInEndPt;
	epBulkOut = m_CyUSBDevice->BulkOutEndPt;
#endif

	if (epBulkOut == NULL || epBulkIn == NULL)
		return FALSE;

	inOvLap.hEvent = CreateEvent(NULL, false, false, L"CYUSB_IN");

	epBulkOut->SetXferSize(outLen);
	epBulkOut->XferData(outBuf, outLen);

	inContext = epBulkIn->BeginDataXfer(inBuf, inLen, &inOvLap);
	if (!epBulkIn->WaitForXfer(&inOvLap, TIMEOUT_PER_TRANSFER_MILLI_SEC))
	{
		epBulkIn->Abort();
		if (epBulkIn->LastError == ERROR_IO_PENDING)
			WaitForSingleObject(inOvLap.hEvent, TIMEOUT_PER_TRANSFER_MILLI_SEC);
	}
	epBulkIn->FinishDataXfer(inBuf, inLen, &inOvLap, inContext);

	CloseHandle(inOvLap.hEvent);

	return TRUE;
}

void CFX3ViewerDlg::LightControlClear()
{
	UCHAR buffer[16];
	LONG length = 0;

	if (IsDlgButtonChecked(IDC_CHECK_WLED))
	{
		CheckDlgButton(IDC_CHECK_WLED, BST_UNCHECKED);

		length = 0;
		buffer[length++] = 0x02;
		buffer[length++] = WHITE_LED;
		buffer[length++] = 0x00;

		SendBulkData(buffer, length, buffer, 2);
	}

	if (IsDlgButtonChecked(IDC_CHECK_RED_LD))
	{
		CheckDlgButton(IDC_CHECK_RED_LD, BST_UNCHECKED);

		length = 0;
		buffer[length++] = 0x02;
		buffer[length++] = LD_650;
		buffer[length++] = 0x00;

		SendBulkData(buffer, length, buffer, 2);
	}

	if (IsDlgButtonChecked(IDC_CHECK_BLUE_LD))
	{
		CheckDlgButton(IDC_CHECK_BLUE_LD, BST_UNCHECKED);

		length = 0;
		buffer[length++] = 0x02;
		buffer[length++] = LD_405;
		buffer[length++] = 0x00;

		SendBulkData(buffer, length, buffer, 2);
	}
}



void CFX3ViewerDlg::SetCameraInitValue()
{
	UCHAR buffer[BULK_DATA_MAX];
	UINT exposure_val = 0x1194; //1ms

	//Auto Exposure disable
	buffer[0] = 0x01;
	buffer[1] = 0x03;
	buffer[2] = 0x20;
	SendBulkData(buffer, 3, buffer, 2);

	buffer[0] = 0x01;
	buffer[1] = 0x10;
	buffer[2] = 0x0C;
	SendBulkData(buffer, 3, buffer, 2);

	buffer[0] = 0x01;
	buffer[1] = 0xb0;
	buffer[2] = 0x10;
	SendBulkData(buffer, 3, buffer, 2);

	//Set Default Exposure Time
	buffer[0] = 0x01;
	buffer[1] = 0x83;
	buffer[2] = ((exposure_val*m_nExpourseTime) & 0x00FF0000) >> 16;
	SendBulkData(buffer, 3, buffer, 2);

	buffer[0] = 0x01;
	buffer[1] = 0x84;
	buffer[2] = ((exposure_val*m_nExpourseTime) & 0x0000FF00) >> 8;
	SendBulkData(buffer, 3, buffer, 2);

	buffer[0] = 0x01;
	buffer[1] = 0x85;
	buffer[2] = ((exposure_val*m_nExpourseTime) & 0x000000FF);
	SendBulkData(buffer, 3, buffer, 2);

	//Set Default Red/Blue Gain
	buffer[0] = 0x01;
	buffer[1] = 0x03;
	buffer[2] = 0x22;
	SendBulkData(buffer, 3, buffer, 2);

	buffer[0] = 0x01;
	buffer[1] = 0xb2;
	buffer[2] = m_nRedGain;
	SendBulkData(buffer, 3, buffer, 2);

	buffer[0] = 0x01;
	buffer[1] = 0xb3;
	buffer[2] = m_nBlueGain;
	SendBulkData(buffer, 3, buffer, 2);
}


void CFX3ViewerDlg::OnBnClickedButtonExpSet()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UCHAR buffer[BULK_DATA_MAX];
	UINT exposure_val = 0x1194; //1ms
	CString string;

	CEdit* pEditExp = (CEdit *)GetDlgItem(IDC_EDIT_EXP_TIME);

	pEditExp->GetWindowTextW(string);
	m_nExpourseTime = _ttoi(string);

	buffer[0] = 0x01;
	buffer[1] = 0x03;
	buffer[2] = 0x20;
	SendBulkData(buffer, 3, buffer, 2);

	buffer[0] = 0x01;
	buffer[1] = 0x83;
	buffer[2] = ((exposure_val*m_nExpourseTime) & 0x00FF0000) >> 16;
	SendBulkData(buffer, 3, buffer, 2);

	buffer[0] = 0x01;
	buffer[1] = 0x84;
	buffer[2] = ((exposure_val*m_nExpourseTime) & 0x0000FF00) >> 8;
	SendBulkData(buffer, 3, buffer, 2);

	buffer[0] = 0x01;
	buffer[1] = 0x85;
	buffer[2] = ((exposure_val*m_nExpourseTime) & 0x000000FF);
	SendBulkData(buffer, 3, buffer, 2);
}


void CFX3ViewerDlg::OnBnClickedButtonRedGainSet()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UCHAR buffer[BULK_DATA_MAX];
	CString string;

	CEdit* pEditRedGain = (CEdit *)GetDlgItem(IDC_EDIT_RED_GAIN);

	pEditRedGain->GetWindowTextW(string);

	UINT temp = 0;
	char* ptr = CString2char(string);
	asciitohex(ptr, &temp);

	m_nRedGain = temp;

	buffer[0] = 0x01;
	buffer[1] = 0x03;
	buffer[2] = 0x22;
	SendBulkData(buffer, 3, buffer, 2);

	buffer[0] = 0x01;
	buffer[1] = 0xb2;
	buffer[2] = m_nRedGain;
	SendBulkData(buffer, 3, buffer, 2);
}


void CFX3ViewerDlg::OnBnClickedButtonBlueGainSet()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UCHAR buffer[BULK_DATA_MAX];
	CString string;

	CEdit* pEditBlueGain = (CEdit *)GetDlgItem(IDC_EDIT_BLUE_GAIN);

	pEditBlueGain->GetWindowTextW(string);

	UINT temp = 0;
	char* ptr = CString2char(string);
	asciitohex(ptr, &temp);

	m_nBlueGain = temp;

	buffer[0] = 0x01;
	buffer[1] = 0x03;
	buffer[2] = 0x22;
	SendBulkData(buffer, 3, buffer, 2);

	buffer[0] = 0x01;
	buffer[1] = 0xb3;
	buffer[2] = m_nBlueGain;
	SendBulkData(buffer, 3, buffer, 2);
}


void CFX3ViewerDlg::OnBnClickedButtonAllSet()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString string;
	UINT temp = 0;
	char* ptr;

	CEdit* pEditExp = (CEdit *)GetDlgItem(IDC_EDIT_EXP_TIME);
	CEdit* pEditRedGain = (CEdit *)GetDlgItem(IDC_EDIT_RED_GAIN);
	CEdit* pEditBlueGain = (CEdit *)GetDlgItem(IDC_EDIT_BLUE_GAIN);

	pEditExp->GetWindowTextW(string);
	m_nExpourseTime = _ttoi(string);

	pEditRedGain->GetWindowTextW(string);
	ptr = CString2char(string);
	asciitohex(ptr, &temp);
	m_nRedGain = temp;

	pEditBlueGain->GetWindowTextW(string);
	ptr = CString2char(string);
	asciitohex(ptr, &temp);
	m_nBlueGain = temp;

	SetCameraInitValue();
}


void CFX3ViewerDlg::OnBnClickedButtonCapture()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.


}
