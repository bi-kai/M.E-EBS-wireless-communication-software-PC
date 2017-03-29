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

#define PHONEID 1   //�ݰѵ绰�ı�ʶ��Ϊ1,3G�����ǿ���Ϊ2,���ǵ绰3,�����ͳһ�滮
#define G3ID 2
#define SATELLITEID 3
#define AUTORESPONSE_TIME 20000//20sδ���룬���Զ��Ҷ�
#define NOPICKUP_TIME 20000//20sδ���������Զ��Ҷ�

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
#define received_frame_size 5//�������������
#define SMSSIZE_10 12 //�Գ����ŵĴ���

#define QUERY_INTERVAL 5000//��ʱ��2�жϼ����10s��ѯһ���Ӱ������״��
#define QUERY_WT 7000//���ߵ绰���ڲ�ѯ�����ƫ��ʱ��
#define QUERY_3G 13000//3G���ڲ�ѯ�����ƫ��ʱ��
#define QUERY_BD 10000//�������ڲ�ѯ�����ƫ��ʱ��
#define QUERY_ST 9000//���ǵ绰���ڲ�ѯ�����ƫ��ʱ��
#define QUERY_YW 8000//��ά����ڲ�ѯ�����ƫ��ʱ��
#define QUERY_TOLERATE_TIMES 2//��ѯ�ô�������δ��ͨ�����ж�ģ��Ͽ�
/////////////////////////////////////////////////////////////////////////////
// CBeidouDlg dialog

typedef struct 
{
	LPCSTR strTransmit;//Ҫ��TTSModule���ݵ��ַ���
	int nPlayTimers;//���Ŵ���		
	}structForTTS;

UINT ThreadProc(LPVOID lpParameter);
BOOL TTSModule(CString str);

class CBeidouDlg : public CDialog
{
// Construction
public:
	CString strCollectForSTCall;
	BOOL bIsTTSPlay;//TTS�Ƿ�����еı�־��Ĭ��ΪFALSE
//	LONG OnCommunication(WPARAM ch, LPARAM port);
	void applay_broadcast(int n);
	bool bRadioIsReady;//�㲥�Ƿ�׼���ɹ���Ĭ��ΪFALSE
	CString strForTTS;
	structForTTS ttsStruct;
	CWinThread* pThread;
	bool bIsAutoResponseProcess;//�Ƿ����Զ�Ӧ������У�Ĭ��ΪFALSE

	void longSMSStructINI();
	void OnClearAll();
	SMSInfoALL ReadSMS(char *pSrc,SMSInfoALL smsb, int nTxRxFlag);
	void SendAtCmd(CString strAT,int nATID);//3G�绰����ATָ��
	void sound_switch(int index);//��Ƶ�����ֶ��л�
	void module_reset(int index);//����ģ�鸴λ
	CString ChuanHaoID;
	void chuanhao(char num,CString ID);//���ߺ�3G�绰���� ��char ID ֻ��3g�����ߵ绰û����
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

	int switch_state;//����ͨ��ģʽ�����л���־λ�����ֽ�������Ҫ��绰���Ƿ����ţ�0����绰���ڣ�1�������Ŵ��ڣ�
	bool state_system[5];//���������ģ�����״̬��־��[0]�����ߵ绰����״̬��־1�����ϣ�0�����ã�[1]��3G�绰״̬��[2]�����ǵ绰״̬��[3]��3G���ţ�[4]������״̬��

	int m_DCom;//�������ں�
	int m_DCom_WT;//���ߵ绰���ں�
	int m_DCom_YW;//��ά�崮�ں�
	int m_DCom_3G;//3G���ں�
	int m_DCom_ST;//���ǵ绰
	CSerialPort m_Port_ST;  //���ǵ绰��CSerialPort�����
	int m_DStopbits;
	char m_DParity;
	int m_DDatabits;
	LONG m_DBaud;	

	int broad_prepare;//����·����㲥״̬��0����ʼֵ��1���㲥��·��ͨ�ɹ���2���㲥��·�Ͽ��ɹ���
	
	BOOL SerialPortOpenCloseFlag;//�������ڴ򿪹رձ�־λ
	unsigned int frame_len[5];//��������֡�����ݳ���
	int frame_index;//֡������
	bool frame_flag[5];//֡׼�ȴ������־λ
	unsigned int framelen;//֡��������
	bool comm_init;//���ڳ��γ�ʼ����־λ
	CStatusBarCtrl *m_StatBar;//״̬��
	int timer_board_disconnect_times_BD;//��ʱ��8,9ͳ�Ƴ������Ӵ������ﵽ3�����жϱ���δ����
	/***************���ߵ绰**************************/
	BOOL SerialPortOpenCloseFlag_WT;//���ߵ绰���ڴ򿪹رձ�־λ
	int WT_state;//���ߵ绰���������״̬��0�����У�1��ժ����2�����ţ�3���һ���4������ժ����
	CString call_in_number;//������ĵ绰����
	bool flag_PW_in_busy;//���ߵ绰�����绰״̬��1:�ӵ绰��;0�������У�
	bool flag_PW_out_busy;//���ߵ绰�����绰״̬��1:�����绰��;0�������У�
	CString send_string;//���к󣬴��ŵĺ��룬��#����
	int timer_board_disconnect_times_WT;//��ʱ��4,5ͳ�Ƴ������Ӵ������ﵽ3�����ж����ߵ绰δ����
	/***************��ά******************************/
	BOOL SerialPortOpenCloseFlag_YW;//��ά���ڴ򿪹رձ�־λ
	bool flag_com_init_ack_YW;//��λ�������ѯ��ά�壬��ά��Բ�ѯ��Ϣ��Ӧ���־λ��1:���ӳɹ���0������ʧ�ܣ�
	unsigned char index_wakeup_times;//����֡���ʹ���������������λ��ͨ�ţ���֤ÿ֡���ݶ���ͬ
	unsigned char index_scan_times;//Ƶ��ɨ��֡���ʹ���������������λ��ͨ�ţ���֤ÿ֡���ݶ���ͬ
	unsigned char index_control_times;//����֡���ʹ���������������λ��ͨ�ţ���֤ÿ֡���ݶ���ͬ
	int index_resent_data_frame;//�ش�֡��ţ�0������(��ѯ֡����ѯ֡��ʹ�ó����ش����ƣ���Ϊ�ǲ�ͣ�Ĳ�ѯ��)��1~3������֡��ţ�4������֡��ţ�5����֤֡��ţ�6����ά�帴λ֡��ţ�7��Ƶ��ɨ�裬�̵�������֡��
	int frame_index_YW;//��ά���ڽ��ջ���֡������
	int timer_board_disconnect_times_YW;//��ʱ��6,7ͳ�Ƴ������Ӵ������ﵽ3�����ж���ά��δ����
	bool modulereset;//�ֶ���λ��ť�Ĵ�����.
	bool soundswitch;//��Ƶ�ź�Դ�ֶ��л�
	/***************3G�绰**************************/
	int nCountCheck;//�Զ�Ӧ����֤�Ĵ�����

	CString strShortSMSIndex;//��bRoughLongSMS���Ӧ����ͨ��Ϣ����
	bool bRoughLongSMS;//�����ж��¶�������ʱ���Ƿ�Ϊ�����š�Ϊ�˽����ͨ����Ҳ��Ҫ12��ĵȴ�ʱ�����⡣Ĭ��ΪFALSE
	CString send_3Gstring;//���к󣬴��ŵĺ��룬��#����
	bool bTimer48OnOff;//��ʱ��48���ڲ�ѯCSQ���������ڶ��Ŷ�ȡ���߷���ʱ����������û��Ӧ��������ѯCSQ��Ĭ��ΪFALSE
	CString SMSIndex[SMSSIZE_10];//���ڴ�Ŷ�ȡ�����б�����Ŵ洢��Index
	bool bReadsmsTimeFlag;//�¶������٣����ڱ�ʶ��Ӧ��ʱ���Ƿ�������Ĭ��ΪFALSE
	CString strLongSMSTextStore[SMSSIZE_10];//���̴洢λ��
	CString strLongSMSIDStore[SMSSIZE_10];//������ID�洢
	strLongSMS  longSMSStruct;
	void SendLongSMS(strLongSMS  *structUse);
	int nLongSMSSendCount;//Ҫ���ͳ����ŵ��ܹ�������Ĭ��Ϊ0���������Ϊ3������һ����չΪ5��
	bool bIsINIProcess;//�Ƿ��ڳ�ʼ�������У�Ĭ��Ϊfalse
	bool bINIFail;//���ڱ�־��ʼ���Ƿ�ɹ�

	SMSInfoALL sms;
	CString strLongSMSText;		
	CString strSMSIndex;//SMS�ڴ洢���е�λ��
	BOOL bIsRecord;//�Ƿ���¼����Ĭ��ΪFALSE
	BOOL bIsCaller;//�Ƿ�Ϊ���У�TRUEΪ���У�Ĭ�ϣ���FALSEΪ����	
	int nEnglishLenth;    //Ӣ���ַ���
    int nChineseLenth;    //�����ַ���
    int nTotalLenth;//�ܹ��ַ���		

	char chPDU[512];//���ڴ��PDU����
	char cReceiveData[2048];//���ڴ�Ž�������	
	int nATCmdID;//ATָ���ID�����ڴ�����Ӧ Ĭ��Ϊ0�� �����Σ�1,1~10����ʼ����2��11~20ͨ����3��21~30
	int nCallFlags;//���ڱ�־call��״̬��Ĭ��Ϊ0������Ϊ1������Ϊ2���һ�Ϊ3
	int nSMSCount;//����ָ������ǰ�洢���ŵ�����
	BOOL bIsNewSMS;//��־�¶��ţ�true��ʾ���յ��¶��ţ�false(Ĭ��)��
	
	BOOL SerialPortOpenCloseFlag_3G;//���ߵ绰���ڴ򿪹رձ�־λ
	int G3_state;//���ߵ绰״̬��0�����У�1��ժ����2�����ţ�3��ͨ����4����
	CString call_in_number_3G;//������ĵ绰����
	bool flag_3G_in_busy;//3G�绰�����绰״̬��1:�ӵ绰��;0�������У�
	bool flag_3G_out_busy;//3G�绰�����绰״̬��1:�����绰��;0�������У�
	int timer_board_disconnect_times_3G;//��ʱ��10,11ͳ�Ƴ������Ӵ������ﵽ3�����ж�3G�绰δ����
//	bool flag_com_init_ack_3G;//��λ�������ѯ3G��3G�Բ�ѯ��Ϣ��Ӧ���־λ��1:���ӳɹ���0������ʧ�ܣ�



	int tel_manual_select;//�绰�����ʽ�ֶ�ѡ��0:�Զ�;1:�ֶ�;
	int msg_manual_select;//���ŷ�����ʽ�ֶ�ѡ��0:�Զ�;1:�ֶ�;
	int tel_mode_saved;//�绰�ֶ�ģʽѡ����һ����ȷ��ѡ���绰�����в������л�ģʽ�����û�ѡ���ˣ����Զ���ԭ����һ�ε�ѡ��
	bool flag_bd_is_busy;//����������.1:������;0:������;
 /***************���ǵ绰**************************/
	bool flag_ST_in_busy;//���ǵ绰�����绰״̬��1:�ӵ绰��;0�������У�
	bool flag_ST_out_busy;//���ǵ绰�����绰״̬��1:�����绰��;0�������У�
	BOOL SerialPortOpenCloseFlag_ST;//���ǵ绰���ڴ򿪹رձ�־λ
	int timer_board_disconnect_times_ST;//��ʱ��12,13ͳ�Ƴ������Ӵ������ﵽ3�����ж����ǵ绰δ����

	CString strdata;
	short comingflag;
	CString coming_num;
	BOOL BINIfail_WX;
	BOOL BISInitial_WX;//�Ƿ��ڳ�ʼ�������У�
	BOOL bISCaller_WX;//�����б�־������TRUE,Ĭ������
	BOOL bIsRecording;//¼����־������¼��ΪTRUE��Ĭ��ΪFALSE
	BOOL bISCallProcess_WX;//�Ƿ���ͨ�������У����б��У���Ĭ��ΪFALSE
	//	CString m_strConfig;//��������Ϣ
	void Send_AT_Command_ST(CString msg); //����ATָ��
	int nATCmdFlags;//ATָ��ı�־
	
	
//	CStatusBarCtrl m_StatusBar;//״̬��


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
