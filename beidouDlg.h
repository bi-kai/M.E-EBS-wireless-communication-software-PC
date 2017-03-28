// beidouDlg.h : header file
//
//{{AFX_INCLUDES()
#include "mscomm.h"
//}}AFX_INCLUDES

#if !defined(AFX_BEIDOUDLG_H__A64D1192_0B9D_4863_8924_CCA2D32F12B0__INCLUDED_)
#define AFX_BEIDOUDLG_H__A64D1192_0B9D_4863_8924_CCA2D32F12B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CBeidouDlg dialog

class CBeidouDlg : public CDialog
{
// Construction
public:
	void chuanhao(char num);
	int strHex(CString str, unsigned char * data,int num);
	void DeFKXX(unsigned char *BUFF);
	void DeMsg(unsigned char *BUFF);
	void DeSts(unsigned char *BUFF);
	void DeSig(unsigned char *BUFF);
	unsigned char XOR(unsigned char *BUFF,int len);
	void decodeheads (unsigned char *BUFF);
	void DeIcc(unsigned char * BUFF);
	CBeidouDlg(CWnd* pParent = NULL);	// standard constructor
	HICON m_hIconRed;    //���ڴ�ʱ�ĺ��ͼ����
	HICON m_hIconOff;    //���ڹر�ʱ��ָʾͼ����

	CRect rectLarge;//ȫ����
	CRect rectSmall;//�绰��
	CRect rectMiddle;//�绰���ſ�
	CRect rectSeparator;

	int switch_state;//�л�״̬��־λ��0����绰��1�������ţ�
//	int state_phone[3];//��绰����ģ�����״̬��־��[0]�����ߵ绰����״̬��־0�������ã�1�����ã�[1]��3G�绰״̬��[2]�����ǵ绰״̬��
//	int state_message[2];//[0]��3G����״̬��[1]����������״̬��

	int m_DCom;
	int m_DStopbits;
	char m_DParity;
	int m_DDatabits;
	LONG m_DBaud;
	
	int m_DCom_WT;
	
	BOOL SerialPortOpenCloseFlag;//�������ڴ򿪹رձ�־λ
	BOOL SerialPortOpenCloseFlag_WT;//���ߵ绰���ڴ򿪹رձ�־λ
	BOOL frame_lock;
	unsigned int frame_len[5];//��������֡�ĳ���
	int frame_index;//֡������
	bool frame_flag[5];//֡׼�ȴ������־λ
	unsigned int framelen;//֡��������
	bool comm_init;//���ڳ��γ�ʼ����־λ
	
	/***************���ߵ绰**************************/
	int WT_state;//���ߵ绰״̬��0�����У�1��ժ����2�����ţ�3��ͨ����4����

// Dialog Data
	//{{AFX_DATA(CBeidouDlg)
	enum { IDD = IDD_BEIDOU_DIALOG };
	CStatic	m_openoff_WT;
	CComboBox	m_com_WT;
	CProgressCtrl	m_timer;
	CProgressCtrl	m_sata3;
	CProgressCtrl	m_sata2;
	CProgressCtrl	m_sata1;
	CStatic	m_ctrlIconOpenoff;
	CComboBox	m_StopBits;
	CComboBox	m_Speed;
	CComboBox	m_Parity;
	CComboBox	m_DataBits;
	CComboBox	m_Com;
	CMSComm	m_comm;
	CString	m_showmsg;
	CString	m_sendmsg;
	long	m_cardnumber;
	int		m_category;
	CString	m_cardstate;
	CString	m_basestate;
	CString	m_FKXX;
	int		m_otherID;
	CString	m_target_number;
	CMSComm	m_comm_WT;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBeidouDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:


protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CBeidouDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnComm1();
	afx_msg void OnOpencloseport();
	afx_msg void OnSelendokComboComselect();
	afx_msg void OnSelendokComboSpeed();
	afx_msg void OnSelendokComboParity();
	afx_msg void OnSelendokComboDatabits();
	afx_msg void OnSelendokComboStopbits();
	afx_msg void OnButtonSendMsg();
	afx_msg void OnButtonClear();
	afx_msg void OnButtonSystemcheck();
	afx_msg void OnButtonIccheck();
	afx_msg void OnButton3Powercheck();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCancelMode();
	afx_msg void OnButtonSet();
	afx_msg void OnButtonPhone();
	afx_msg void OnButtonMessage();
	afx_msg void OnComm_WT();
	afx_msg void OnOpencloseportWT();
	afx_msg void OnSelendokComboComselectWT();
	afx_msg void OnButtonClearNum();
	afx_msg void OnButtonCall();
	afx_msg void OnChangeEditTargetnum();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnButton5();
	afx_msg void OnButton6();
	afx_msg void OnButton8();
	afx_msg void OnButton7();
	afx_msg void OnButton9();
	afx_msg void OnButton10();
	afx_msg void OnButtonXing();
	afx_msg void OnButtonJing();
	afx_msg void OnButtonBack();
	afx_msg void OnDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BEIDOUDLG_H__A64D1192_0B9D_4863_8924_CCA2D32F12B0__INCLUDED_)
