
// IpconfigResultDlg.h : header file
//

#pragma once


// CIpconfigResultDlg dialog
class CIpconfigResultDlg : public CDialogEx
{
// Construction
public:
	CIpconfigResultDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IPCONFIGRESULT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	int ReadFromPipeNoWait(HANDLE, char*, int);
	BOOL ExecAndProcessOutput(LPCSTR, LPCSTR);
public:
	afx_msg void OnBnClickedBtnCheck();
};
