
// DummyDlg.h : header file
//

#pragma once

#include "Setup/SetupData.h"

using namespace MySetup;

// CDummyDialog dialog
class CDummyDialog : public CDialog
{
// Construction
public:
	CDummyDialog(CSetupData *pSetupData, CWnd *pParent = NULL);

// Dialog Data
	enum { IDD = IDD_SETUPPROJECT_DIALOG };

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
private:
	CSetupData *m_setupData;	
};
