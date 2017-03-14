// beidouDlg.h : header file
//
//{{AFX_INCLUDES()
#include "mscomm.h"
//}}AFX_INCLUDES


#if !defined(AFX_BEIDOUDLG_H__A64D1192_0B9D_4863_8924_CCA2D32F12B0__INCLUDED_)
#define AFX_BEIDOUDLG_H__A64D1192_0B9D_4863_8924_CCA2D32F12B0__INCLUDED_

#include "3G.h"
#include "tts.h"
#include "SerialPort.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PHONEID 1   //暂把电话的标识设为1,3G和卫星可设为2,卫星电话3,后可以统一规划
#define G3ID 2
#define SATELLITEID 3
#define AUTORESPONSE_TIME 20000//20s未输入，则自动挂断
#define NOPICKUP_TIME 20000//20s未接听，则自动挂断

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
#define SMSSIZE_10 12 //对长短信的处理

#define QUERY_INTERVAL 5000//定时器2中断间隔，10s查询一次子板的连接状况
#define QUERY_WT 7000//有线电话基于查询间隔的偏移时间
#define QUERY_3G 13000//3G基于查询间隔的偏移时间
#define QUERY_BD 10000//北斗基于查询间隔的偏移时间
#define QUERY_ST 9000//卫星电话基于查询间隔的偏移时间
#define QUERY_YW 8000//运维板基于查询间隔的偏移时间
#define QUERY_TOLERATE_TIMES 2//查询该次数后若未联通，则判断模块断开
/////////////////////////////////////////////////////////////////////////////
// CBeidouDlg dialog

typedef struct 
{
	LPCSTR strTransmit;//要向TTSModule传递的字符串
	int nPlayTimers;//播放次数		
	}structForTTS;

UINT ThreadProc(LPVOID lpParameter);
BOOL TTSModule(CString str);

class CBeidouDlg : public CDialog
{
// Construction
public:
	CString strCollectForSTCall;
	BOOL bIsTTSPlay;//TTS是否进行中的标志，默认为FALSE
//	LONG OnCommunication(WPARAM ch, LPARAM port);
	void applay_broadcast(int n);
	bool bRadioIsReady;//广播是否准备成功，默认为FALSE
	CString strForTTS;
	structForTTS ttsStruct;
	CWinThread* pThread;
	bool bIsAutoResponseProcess;//是否处于自动应答过程中，默认为FALSE

	void longSMSStructINI();
	void OnClearAll();
	SMSInfoALL ReadSMS(char *pSrc,SMSInfoALL smsb, int nTxRxFlag);
	void SendAtCmd(CString strAT,int nATID);//3G电话发送AT指令
	void sound_switch(int index);//音频开关手动切换
	void module_reset(int index);//功能模块复位
	CString ChuanHaoID;
	void chuanhao(char num,CString ID);//有线和3G电话传号 ，char ID 只是3g，有线电话没有用
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
	bool state_system[5];//本软件功能模块可用状态标志。[0]：有线电话可用状态标志1：故障，0：可用；[1]：3G电话状态；[2]：卫星电话状态；[3]：3G短信；[4]：北斗状态；

	int m_DCom;//北斗串口号
	int m_DCom_WT;//有线电话串口号
	int m_DCom_YW;//运维板串口号
	int m_DCom_3G;//3G串口号
	int m_DCom_ST;//卫星电话
	CSerialPort m_Port_ST;  //卫星电话，CSerialPort类对象
	int m_DStopbits;
	char m_DParity;
	int m_DDatabits;
	LONG m_DBaud;	

	int broad_prepare;//多链路接入广播状态。0：初始值；1：广播链路接通成功；2：广播链路断开成功；
	
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
	int WT_state;//有线电话即将进入的状态。0：空闲；1：摘机；2：拨号；3：挂机；4：虚拟摘机；
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
	int nCountCheck;//自动应答验证的次数。

	CString strShortSMSIndex;//与bRoughLongSMS相对应的普通短息索引
	bool bRoughLongSMS;//初略判断新短信来的时候，是否为长短信。为了解决普通短信也需要12秒的等待时间问题。默认为FALSE
	CString send_3Gstring;//被叫后，传号的号码，以#结束
	bool bTimer48OnOff;//定时器48用于查询CSQ，变量用于短信读取或者发送时，死机或者没反应，开启查询CSQ，默认为FALSE
	CString SMSIndex[SMSSIZE_10];//用于存放读取短信列表，后短信存储的Index
	bool bReadsmsTimeFlag;//新短信来临，用于标识相应定时器是否启动，默认为FALSE
	CString strLongSMSTextStore[SMSSIZE_10];//长短存储位置
	CString strLongSMSIDStore[SMSSIZE_10];//长短信ID存储
	strLongSMS  longSMSStruct;
	void SendLongSMS(strLongSMS  *structUse);
	int nLongSMSSendCount;//要发送长短信的总共条数，默认为0，先最多设为3条，进一步扩展为5条
	bool bIsINIProcess;//是否处于初始化过程中，默认为false
	bool bINIFail;//用于标志初始化是否成功

	SMSInfoALL sms;
	CString strLongSMSText;		
	CString strSMSIndex;//SMS在存储器中的位置
	BOOL bIsRecord;//是否在录音，默认为FALSE
	BOOL bIsCaller;//是否为主叫，TRUE为主叫（默认），FALSE为被叫	
	int nEnglishLenth;    //英文字符数
    int nChineseLenth;    //中文字符数
    int nTotalLenth;//总共字符数		

	char chPDU[512];//用于存放PDU数据
	char cReceiveData[2048];//用于存放接收数据	
	int nATCmdID;//AT指令的ID，用于串口响应 默认为0， 分三段：1,1~10，初始化，2、11~20通话，3、21~30
	int nCallFlags;//用于标志call的状态，默认为0，拨号为1，接听为2，挂机为3
	int nSMSCount;//用于指明，当前存储短信的条数
	BOOL bIsNewSMS;//标志新短信，true表示接收到新短信，false(默认)无
	
	BOOL SerialPortOpenCloseFlag_3G;//有线电话串口打开关闭标志位
	int G3_state;//有线电话状态。0：空闲；1：摘机；2：拨号；3：通话；4：；
	CString call_in_number_3G;//打进来的电话号码
	bool flag_3G_in_busy;//3G电话接听电话状态。1:接电话中;0：空闲中；
	bool flag_3G_out_busy;//3G电话拨出电话状态。1:拨出电话中;0：空闲中；
	int timer_board_disconnect_times_3G;//定时器10,11统计尝试连接次数，达到3次则判断3G电话未连接
//	bool flag_com_init_ack_3G;//上位机软件查询3G，3G对查询信息的应答标志位。1:连接成功；0：连接失败；



	int tel_manual_select;//电话打出方式手动选择。0:自动;1:手动;
	int msg_manual_select;//短信发出方式手动选择。0:自动;1:手动;
	int tel_mode_saved;//电话手动模式选择，上一次正确的选项。打电话过程中不允许切换模式。若用户选择了，则自动还原回上一次的选项
	bool flag_bd_is_busy;//北斗发送中.1:发送中;0:空闲中;
 /***************卫星电话**************************/
	bool flag_ST_in_busy;//卫星电话接听电话状态。1:接电话中;0：空闲中；
	bool flag_ST_out_busy;//卫星电话拨出电话状态。1:拨出电话中;0：空闲中；
	BOOL SerialPortOpenCloseFlag_ST;//卫星电话串口打开关闭标志位
	int timer_board_disconnect_times_ST;//定时器12,13统计尝试连接次数，达到3次则判断卫星电话未连接

	CString strdata;
	short comingflag;
	CString coming_num;
	BOOL BINIfail_WX;
	BOOL BISInitial_WX;//是否处于初始化过程中，
	BOOL bISCaller_WX;//主被叫标志，主叫TRUE,默认主叫
	BOOL bIsRecording;//录音标志，正在录音为TRUE，默认为FALSE
	BOOL bISCallProcess_WX;//是否处在通话过程中（主叫被叫），默认为FALSE
	//	CString m_strConfig;//存配置信息
	void Send_AT_Command_ST(CString msg); //发送AT指令
	int nATCmdFlags;//AT指令的标志
	
	
//	CStatusBarCtrl m_StatusBar;//状态栏


// Dialog Data
	//{{AFX_DATA(CBeidouDlg)
	enum { IDD = IDD_BEIDOU_DIALOG };
	CStatic	m_board_led_ST;
	CStatic	m_ctrlIconOpenoff_ST;
	CComboBox	m_Com_ST;
	CComboBox	m_msg_manual_select;
	CStatic	m_3g_m_r;
	CStatic	m_3g_p_r;
	CStatic	m_bd_r;
	CStatic	m_st_r;
	CStatic	m_wt_r;
	CComboBox	m_tel_manual_select;
	CStatic	m_board_led_3G;
	CStatic	m_ctrlIconOpenoff_3G;
	CComboBox	m_Com_3G;
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
	CString	m_target_number;
	CMSComm	m_comm_WT;
	CMSComm	m_comm_YW;
	CMSComm	m_comm_3G;
	CString	m_temperature;
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
	afx_msg void OnButtonConnect_3G();
	afx_msg void OnSelendokComboComselect3G();
	afx_msg void OnComm_3G();
	afx_msg void OnSelendokComboTelManualSelect();
	afx_msg void OnSelendokComboMsgManualSelect();
	afx_msg void On123();
	afx_msg void OnButtonConnect_ST();
	afx_msg void OnSelendokComboComselect_ST();
	afx_msg LONG OnCommunication(WPARAM ch, LPARAM port);
	afx_msg void OnGPS();
	afx_msg void On_CSQ_ST();
	afx_msg void On_Position_ST();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BEIDOUDLG_H__A64D1192_0B9D_4863_8924_CCA2D32F12B0__INCLUDED_)
