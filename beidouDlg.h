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

#define PHONEID 1   //暂把电话的标识设为1,3G和卫星可设为2,卫星电话3,后可以统一规划
#define G3ID 2
#define SATELLITEID 3
#define AUTORESPONSE_TIME 20000//20s未输入，则自动挂断
#define NOPICKUP_TIME 60000//60s未接听，则自动挂断

#define Sig 10
#define Pos 1
#define Msg 2
#define Bst 3
#define Icc 4
#define Sts 5
#define Zst 6
#define Zrd 7
#define Tim 8
#define FKXX 9
#define received_frame_size 5//缓冲区数组个数

#define QUERY_INTERVAL 3000//定时器2中断间隔，10s查询一次子板的连接状况
#define QUERY_WT 7000//有线电话基于查询间隔的偏移时间
#define QUERY_3G 10000//3G基于查询间隔的偏移时间
#define QUERY_BD 6000//北斗基于查询间隔的偏移时间
#define QUERY_ST 3000//卫星电话基于查询间隔的偏移时间
#define QUERY_YW 1500//运维板基于查询间隔的偏移时间
#define QUERY_TOLERATE_TIMES 2//查询该次数后若未联通，则判断模块断开
/////////////////////////////////////////////////////////////////////////////
// CBeidouDlg dialog

class CBeidouDlg : public CDialog
{
// Construction
public:
	void sound_switch(int index);//音频开关手动切换
	void module_reset(int index);//功能模块复位
	void chuanhao(char num);//有线电话传号
	int strHex(CString str, unsigned char * data,int num);
	void DeFKXX(unsigned char *BUFF);
	void DeMsg(unsigned char *BUFF);
	void DeSts(unsigned char *BUFF);
	void DeSig(unsigned char *BUFF);
	unsigned char XOR(unsigned char *BUFF,int len);
	void decodeheads (unsigned char *BUFF);
	void DeIcc(unsigned char * BUFF);
	CBeidouDlg(CWnd* pParent = NULL);	// standard constructor
	HICON m_hIconRed;    //串口打开时的红灯图标句柄
	HICON m_hIconOff;    //串口关闭时的指示图标句柄

	CRect rectLarge;//全部框
	CRect rectSmall;//电话框
	CRect rectMiddle;//电话短信框
	CRect rectSeparator;

	int switch_state;//顶部通信模式串口切换标志位，区分接下来是要打电话还是发短信，0：打电话窗口；1：发短信窗口；
	bool state_system[4];//本软件功能模块可用状态标志。[0]：有线电话可用状态标志1：故障，0：可用；[1]：3G电话状态；[2]：卫星电话状态；[3]：北斗状态；

	int m_DCom;//北斗串口号
	int m_DCom_WT;//有线电话串口号
	int m_DCom_YW;//运维板串口号
	int m_DStopbits;
	char m_DParity;
	int m_DDatabits;
	LONG m_DBaud;	
	
	BOOL SerialPortOpenCloseFlag;//北斗串口打开关闭标志位
	unsigned int frame_len[5];//各个缓冲帧的数据长度
	int frame_index;//帧的索引
	bool frame_flag[5];//帧准等待处理标志位
	unsigned int framelen;//帧长计数器
	bool comm_init;//串口初次初始化标志位
	CStatusBarCtrl *m_StatBar;//状态栏
	int timer_board_disconnect_times_BD;//定时器8,9统计尝试连接次数，达到3次则判断北斗未连接
	/***************有线电话**************************/
	BOOL SerialPortOpenCloseFlag_WT;//有线电话串口打开关闭标志位
	int WT_state;//有线电话状态。0：空闲；1：摘机；2：拨号；3：通话；4：；
	CString call_in_number;//打进来的电话号码
	bool flag_PW_in_busy;//有线电话接听电话状态。1:接电话中;0：空闲中；
	bool flag_PW_out_busy;//有线电话拨出电话状态。1:拨出电话中;0：空闲中；
	CString send_string;//被叫后，传号的号码，以#结束
	int timer_board_disconnect_times_WT;//定时器4,5统计尝试连接次数，达到3次则判断有线电话未连接
	/***************运维******************************/
	BOOL SerialPortOpenCloseFlag_YW;//运维串口打开关闭标志位
	bool flag_com_init_ack_YW;//上位机软件查询运维板，运维板对查询信息的应答标志位。1:连接成功；0：连接失败；
	unsigned char index_wakeup_times;//连接帧发送次数计数器，上下位机通信，保证每帧数据都不同
	unsigned char index_scan_times;//频谱扫描帧发送次数计数器，上下位机通信，保证每帧数据都不同
	unsigned char index_control_times;//控制帧发送次数计数器，上下位机通信，保证每帧数据都不同
	int index_resent_data_frame;//重传帧编号，0：空闲(查询帧：查询帧不使用出错重传机制，因为是不停的查询的)；1~3：唤醒帧编号；4：报警帧编号；5：认证帧编号；6：运维板复位帧编号；7：频谱扫描，继电器控制帧；
	int frame_index_YW;//运维串口接收缓冲帧的索引
	int timer_board_disconnect_times_YW;//定时器6,7统计尝试连接次数，达到3次则判断运维板未连接
	bool modulereset;//手动复位按钮的触发器.
	bool soundswitch;//音频信号源手动切换
	/***************3G电话**************************/



// Dialog Data
	//{{AFX_DATA(CBeidouDlg)
	enum { IDD = IDD_BEIDOU_DIALOG };
	CStatic	m_board_led_BD;
	CStatic	m_board_led_WT;
	CStatic	m_board_led_YW;
	CStatic	m_ctrlIconOpenoff_YW;
	CComboBox	m_Com_YW;
	CEdit	m_c_FKXX;
	CStatic	m_openoff_WT;
	CComboBox	m_com_WT;
	CProgressCtrl	m_timer;
	CProgressCtrl	m_sata3;
	CProgressCtrl	m_sata2;
	CProgressCtrl	m_sata1;
	CStatic	m_ctrlIconOpenoff;
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
	CMSComm	m_comm_YW;
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
	afx_msg void OnChangeEditFkxx();
	afx_msg void OnComm_YW();
	afx_msg void OnButtonConnect_YW();
	afx_msg void OnSelendokComboComselectYw();
	afx_msg void OnModuleReset();
	afx_msg void OnSoundSwitch();
	afx_msg void OnButtonClearmsg();
	afx_msg void OnChangeEditSendmsg();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BEIDOUDLG_H__A64D1192_0B9D_4863_8924_CCA2D32F12B0__INCLUDED_)
