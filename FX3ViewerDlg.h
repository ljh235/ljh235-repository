
// FX3ViewerDlg.h : 헤더 파일
//

#pragma once

//#include <setupapi.h>
//#include <initguid.h>
//#include <winusb.h>
#include "CyAPI.h"
#include "Feature.h"

#if 0
typedef struct _DEVICE_DATA {

	BOOL                    HandlesOpen;
	WINUSB_INTERFACE_HANDLE WinusbHandle;
	HANDLE                  DeviceHandle;
	TCHAR                   DevicePath[MAX_PATH];

} DEVICE_DATA, *PDEVICE_DATA;
#endif

#define TIMER_ID_WAIT_UVC 1000
#define TIMER_ID_RUN_UVC 1001
#define TIMER_ID_BULK 1002

#define TIMEOUT_PER_TRANSFER_MILLI_SEC 150

#define WHITE_LED 0x00
#define LD_650 0x01
#define LD_405 0x02

#define BULK_DATA_MAX 64

#define CAMERA_WIDTH_MAX 1280
#define CAMERA_HEIGHT_MAX 720

// CFX3ViewerDlg 대화 상자
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

														// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

class CFX3ViewerDlg : public CDialogEx
{
// 생성입니다.
public:
	CFX3ViewerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FX3VIEWER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	BOOL m_chk_wled;
	BOOL m_chk_red_ld;
	BOOL m_chk_blue_ld;
	afx_msg void OnBnClickedCheckWled();
	afx_msg void OnBnClickedCheckRedLd();
	afx_msg void OnBnClickedCheckBlueLd();

private:
	CCyUSBDevice *m_CyUSBDevice;
	CImage* m_image;
	CDC *m_pCamDC;
	BOOL m_bFirstData;
	UINT m_nExpourseTime;
	UCHAR m_nRedGain;
	UCHAR m_nBlueGain;
	UINT m_CamWidth;
	UINT m_CamHeight;

public:
	afx_msg void OnBnClickedButtonWrite();
	afx_msg void OnBnClickedButtonRead();
private:
	BOOL SendBulkData(UCHAR* outBuf, LONG outLen, UCHAR* inBuf, LONG inLen);
	void LightControlClear();
public:
	HGLRC m_hRC;
	HDC m_hViewDC;
private:
	void SetCameraInitValue();
public:
	afx_msg void OnBnClickedButtonExpSet();
	afx_msg void OnBnClickedButtonRedGainSet();
	afx_msg void OnBnClickedButtonBlueGainSet();
	afx_msg void OnBnClickedButtonAllSet();

	afx_msg void OnBnClickedButtonCapture();
};
