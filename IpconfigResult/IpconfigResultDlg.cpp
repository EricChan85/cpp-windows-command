
// IpconfigResultDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IpconfigResult.h"
#include "IpconfigResultDlg.h"
#include "afxdialogex.h"
#include <atlstr.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIpconfigResultDlg dialog



CIpconfigResultDlg::CIpconfigResultDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIpconfigResultDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIpconfigResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CIpconfigResultDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_Check, &CIpconfigResultDlg::OnBnClickedBtnCheck)
END_MESSAGE_MAP()


// CIpconfigResultDlg message handlers

BOOL CIpconfigResultDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIpconfigResultDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIpconfigResultDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CIpconfigResultDlg::OnBnClickedBtnCheck()
{

	CString sCmd = CString(_T("c:\\windows\\system32\\Cmd.Exe"));
	CString sParms, sCommandText;
	this->GetDlgItemTextW(mc_Command, sCommandText);
	sParms.Format(_T(" /c %s"), sCommandText);

	BOOL fRet = ExecAndProcessOutput((LPCSTR)(LPCTSTR)sCmd, (LPCSTR)(LPCTSTR)sParms);
}

int CIpconfigResultDlg::ReadFromPipeNoWait(HANDLE hPipe, char* pDest, int nMax)
{
	DWORD nBytesRead = 0;
	DWORD nAvailBytes;
	char cTmp;
	memset(pDest, 0, nMax);
	// -- check for something in the pipe
	PeekNamedPipe(hPipe, &cTmp, 1, NULL, &nAvailBytes, NULL);
	if (nAvailBytes == 0) {
		return(nBytesRead);
	}
	// OK, something there... read it
	ReadFile(hPipe, pDest, nMax - 1, &nBytesRead, NULL);
	return(nBytesRead);
}

BOOL CIpconfigResultDlg::ExecAndProcessOutput(LPCSTR szCmd, LPCSTR szParms)
{
	SECURITY_ATTRIBUTES rSA = { 0 };
	rSA.nLength = sizeof(SECURITY_ATTRIBUTES);
	rSA.bInheritHandle = TRUE;

	HANDLE hReadPipe, hWritePipe;
	CreatePipe(&hReadPipe, &hWritePipe, &rSA, 25000);

	PROCESS_INFORMATION rPI = { 0 };
	STARTUPINFO         rSI = { 0 };
	rSI.cb = sizeof(rSI);
	rSI.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	rSI.wShowWindow = SW_HIDE;  // or SW_SHOWNORMAL or SW_MINIMIZE
	rSI.hStdOutput = hWritePipe;
	rSI.hStdError = hWritePipe;

	CString sCmd; 
	sCmd.Format(_T("\"%s\" %s"), (LPCSTR)szCmd, (LPCSTR)szParms);

	BOOL fRet = CreateProcess(NULL, (LPTSTR)(LPCTSTR)sCmd, NULL,
		NULL, TRUE, 0, 0, 0, &rSI, &rPI);
	if (!fRet) {
		return(FALSE);
	}
	//------------------------- and process its stdout every 100 ms
	char dest[1000];
	CString sProgress = CString(_T(""));
	DWORD dwRetFromWait = WAIT_TIMEOUT;
	while (dwRetFromWait != WAIT_OBJECT_0) {
		dwRetFromWait = WaitForSingleObject(rPI.hProcess, 100);
		if (dwRetFromWait == WAIT_ABANDONED) {  // crash?
			break;
		}
		//--- else (WAIT_OBJECT_0 or WAIT_TIMEOUT) process the pipe data
		while (ReadFromPipeNoWait(hReadPipe, dest, sizeof(dest)) > 0) {

			CString str(dest);
			SetDlgItemText(mc_Output, (LPCTSTR)(str));
		}
	}
	CloseHandle(hReadPipe);
	CloseHandle(hWritePipe);
	CloseHandle(rPI.hThread);
	CloseHandle(rPI.hProcess);
	// MessageBox("All done!");
	return TRUE;
}
