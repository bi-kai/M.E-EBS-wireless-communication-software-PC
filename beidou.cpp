// beidou.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "beidou.h"
#include "beidouDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBeidouApp

BEGIN_MESSAGE_MAP(CBeidouApp, CWinApp)
	//{{AFX_MSG_MAP(CBeidouApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBeidouApp construction

CBeidouApp::CBeidouApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_nBaud=115200;
	m_nCom=1;
	m_cParity='N';
	m_nDatabits=8;
    m_nStopbits=1;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBeidouApp object

CBeidouApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBeidouApp initialization

BOOL CBeidouApp::InitInstance()
{
	AfxEnableControlContainer();
	skinppLoadSkin(_T("Vista.ssk"));//Vista.sskΪ��Ŀ�µ�Ƥ���ļ�

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CBeidouDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}