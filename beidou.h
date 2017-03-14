// beidou.h : main header file for the BEIDOU application
//

#if !defined(AFX_BEIDOU_H__28E8B9D1_38AA_431D_83E1_33652A7E965F__INCLUDED_)
#define AFX_BEIDOU_H__28E8B9D1_38AA_431D_83E1_33652A7E965F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#define SKINSPACE _T("/SPATH:") ////  注意：这个必须添加在#include的下面！！！
/////////////////////////////////////////////////////////////////////////////
// CBeidouApp:
// See beidou.cpp for the implementation of this class
//

class CBeidouApp : public CWinApp
{
public:
	CBeidouApp();
	int m_nBaud;       //波特率
	int m_nCom;         //串口号
	char m_cParity;    //校验
	int m_nDatabits;    //数据位
	int m_nStopbits;    //停止位

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBeidouApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBeidouApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BEIDOU_H__28E8B9D1_38AA_431D_83E1_33652A7E965F__INCLUDED_)
