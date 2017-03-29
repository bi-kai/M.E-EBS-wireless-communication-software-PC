// beidouDlg.cpp : implementation file
//
#include "stdafx.h"
#include "beidou.h"
#include "beidouDlg.h"
#include <wchar.h>
#include <stdio.h>
#include <math.h> 
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"WINMM.LIB")
#include "time.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
TIMER1,2 ʹ�ܷ�����Ϣ��ť��ˢ�±���������
TIMER3 ���ߵ绰ժ�����Ŷ��û�͸��

TIMER4,5 ���ߵ绰
TIMER6,7 ��ά
TIMER8,9 ����
TIMER10,11 3G
TIMER20,21,22 ���ǵ绰
TIMER14 ���ڼ�鷴����Ϣ���ı����ȣ�����ʱ����
TIMER33,34 �Զ�Ӧ��
*/
/*************����******************************/
unsigned char frame_IC_check[12]={0x24,0x49,0x43,0x4A,0x43,0x00,0x0C,0x00,0x00,0x00,0x01,0x2A};//IC��ѯ
unsigned char frame_SYS_check[11]={0x24,0x53,0x74,0x73,0x5F,0x00,0x0B};//ϵͳ���
unsigned char frame_POWER_check[12]={0x24,0x47,0x4C,0x4A,0x43,0x00,0x0C,0x00,0x00,0x00,0x01,0x2B};//���ʲ�ѯ
unsigned char frame_MSG_check[250]={0x24,0x54,0x58,0x53,0x51};//ͨ������

unsigned char frame_receive[received_frame_size][2000]={0};//��������֡����

/*************���ߵ绰******************************/
unsigned char frame_receive_WT[2000]={0};

/***********��ά��***************/
unsigned char frame_board_check_YW[7+2]={'$','r','e','y','_'};//��ά���Ӽ��֡
unsigned char frame_board_reset_YW[8+2]={'$','r','s','e','_'};//��ά��λ֡
unsigned char frame_board_sound_YW[8+2]={'$','s','w','h','_'};//��ά�л���Ƶ��������֡
unsigned char frame_board_connect_YW[7+2]={'$','p','p','p','_'};//��ά��λ����������໥��ѯ֡
unsigned char frame_board_scan_YW[7+2]={'$','s','c','a','_'};//��άƵ��ɨ�裬�̵�������֡
unsigned char frame_board_ppp[8+2]={'$','p','p','p','_'};//��ά��/�رչ㲥����֡֡
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBeidouDlg dialog

CBeidouDlg::CBeidouDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBeidouDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBeidouDlg)
	m_showmsg = _T("");
	m_sendmsg = _T("");
	m_cardnumber = 0;
	m_category = 0;
	m_cardstate = _T("");
	m_basestate = _T("");
	m_FKXX = _T("");
	m_target_number = _T("");
	m_temperature = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


}

void CBeidouDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBeidouDlg)
	DDX_Control(pDX, IDC_STATIC_BOARD_LED_ST, m_board_led_ST);
	DDX_Control(pDX, IDC_STATIC_ST, m_ctrlIconOpenoff_ST);
	DDX_Control(pDX, IDC_COMBO_COMSELECT_ST, m_Com_ST);
	DDX_Control(pDX, IDC_COMBO_MSG_MANUAL_SELECT, m_msg_manual_select);
	DDX_Control(pDX, IDC_STATIC_3G_M_R, m_3g_m_r);
	DDX_Control(pDX, IDC_STATIC_3G_P_R, m_3g_p_r);
	DDX_Control(pDX, IDC_STATIC_BD_R, m_bd_r);
	DDX_Control(pDX, IDC_STATIC_ST_R, m_st_r);
	DDX_Control(pDX, IDC_STATIC_WT_R, m_wt_r);
	DDX_Control(pDX, IDC_COMBO_TEL_MANUAL_SELECT, m_tel_manual_select);
	DDX_Control(pDX, IDC_STATIC_BOARD_LED_3G, m_board_led_3G);
	DDX_Control(pDX, IDC_STATIC_3G, m_ctrlIconOpenoff_3G);
	DDX_Control(pDX, IDC_COMBO_COMSELECT_3G, m_Com_3G);
	DDX_Control(pDX, IDC_STATIC_BOARD_LED_BD, m_board_led_BD);
	DDX_Control(pDX, IDC_STATIC_BOARD_LED_WT, m_board_led_WT);
	DDX_Control(pDX, IDC_STATIC_BOARD_LED_YW, m_board_led_YW);
	DDX_Control(pDX, IDC_STATIC_YUNWEI, m_ctrlIconOpenoff_YW);
	DDX_Control(pDX, IDC_COMBO_COMSELECT_YW, m_Com_YW);
	DDX_Control(pDX, IDC_EDIT_FKXX, m_c_FKXX);
	DDX_Control(pDX, IDC_STATIC_OPENOFF_WT, m_openoff_WT);
	DDX_Control(pDX, IDC_COMBO_COMSELECT_WT, m_com_WT);
	DDX_Control(pDX, IDC_PROGRESS_TIMER, m_timer);
	DDX_Control(pDX, IDC_PROGRESS3, m_sata3);
	DDX_Control(pDX, IDC_PROGRESS2, m_sata2);
	DDX_Control(pDX, IDC_PROGRESS1, m_sata1);
	DDX_Control(pDX, IDC_STATIC_OPENOFF, m_ctrlIconOpenoff);
	DDX_Control(pDX, IDC_COMBO_COMSELECT, m_Com);
	DDX_Control(pDX, IDC_MSCOMM1, m_comm);
	DDX_Text(pDX, IDC_EDIT_RECEIVEMSG, m_showmsg);
	DDX_Text(pDX, IDC_EDIT_SENDMSG, m_sendmsg);
	DDX_Text(pDX, IDC_EDIT_CARDNUMBER, m_cardnumber);
	DDX_Text(pDX, IDC_EDIT_CATEGORY, m_category);
	DDX_Text(pDX, IDC_EDIT_CARDSTATES, m_cardstate);
	DDX_Text(pDX, IDC_EDIT_BASESTATES, m_basestate);
	DDX_Text(pDX, IDC_EDIT_FKXX, m_FKXX);
	DDX_Text(pDX, IDC_EDIT_TARGETNUM, m_target_number);
	DDX_Control(pDX, IDC_MSCOMM_WT, m_comm_WT);
	DDX_Control(pDX, IDC_MSCOMM_YW, m_comm_YW);
	DDX_Control(pDX, IDC_MSCOMM_3G, m_comm_3G);
	DDX_Text(pDX, IDC_STATIC_temperature, m_temperature);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBeidouDlg, CDialog)
	//{{AFX_MSG_MAP(CBeidouDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPENCLOSEPORT, OnOpencloseport)
	ON_CBN_SELENDOK(IDC_COMBO_COMSELECT, OnSelendokComboComselect)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSendMsg)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_ICCHECK, OnButtonIccheck)
	ON_BN_CLICKED(IDC_BUTTON3_POWERCHECK, OnButton3Powercheck)
	ON_WM_TIMER()
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_BUTTON_SET, OnButtonSet)
	ON_BN_CLICKED(IDC_BUTTON_PHONE, OnButtonPhone)
	ON_BN_CLICKED(IDC_BUTTON_MESSAGE, OnButtonMessage)
	ON_BN_CLICKED(IDC_OPENCLOSEPORT_WT, OnOpencloseportWT)
	ON_CBN_SELENDOK(IDC_COMBO_COMSELECT_WT, OnSelendokComboComselectWT)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_NUM, OnButtonClearNum)
	ON_BN_CLICKED(IDC_BUTTON_CALL, OnButtonCall)
	ON_EN_CHANGE(IDC_EDIT_TARGETNUM, OnChangeEditTargetnum)
	ON_BN_CLICKED(IDC_BUTTON_1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON_2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON_3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON_4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON_5, OnButton5)
	ON_BN_CLICKED(IDC_BUTTON_6, OnButton6)
	ON_BN_CLICKED(IDC_BUTTON_8, OnButton8)
	ON_BN_CLICKED(IDC_BUTTON_7, OnButton7)
	ON_BN_CLICKED(IDC_BUTTON_9, OnButton9)
	ON_BN_CLICKED(IDC_BUTTON_10, OnButton10)
	ON_BN_CLICKED(IDC_BUTTON_XING, OnButtonXing)
	ON_BN_CLICKED(IDC_BUTTON_JING, OnButtonJing)
	ON_BN_CLICKED(IDC_BUTTON_BACK, OnButtonBack)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT_FKXX, OnChangeEditFkxx)
	ON_BN_CLICKED(IDC_BUTTON_CONNECTYUNWEI, OnButtonConnect_YW)
	ON_CBN_SELENDOK(IDC_COMBO_COMSELECT_YW, OnSelendokComboComselectYw)
	ON_BN_CLICKED(IDC_MODULE_RESET, OnModuleReset)
	ON_BN_CLICKED(IDC_SOUND_SWITCH, OnSoundSwitch)
	ON_BN_CLICKED(IDC_BUTTON_CLEARMSG, OnButtonClearmsg)
	ON_EN_CHANGE(IDC_EDIT_SENDMSG, OnChangeEditSendmsg)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT3G, OnButtonConnect_3G)
	ON_CBN_SELENDOK(IDC_COMBO_COMSELECT_3G, OnSelendokComboComselect3G)
	ON_CBN_SELENDOK(IDC_COMBO_TEL_MANUAL_SELECT, OnSelendokComboTelManualSelect)
	ON_CBN_SELENDOK(IDC_COMBO_MSG_MANUAL_SELECT, OnSelendokComboMsgManualSelect)
	ON_BN_CLICKED(IDC_123, On123)
	ON_BN_CLICKED(IDC_BUTTON_CONNECTST, OnButtonConnect_ST)
	ON_CBN_SELENDOK(IDC_COMBO_COMSELECT_ST, OnSelendokComboComselect_ST)
	ON_MESSAGE(WM_COMM_RXCHAR, OnCommunication)
	ON_BN_CLICKED(IDC_GPS, OnGPS)
	ON_BN_CLICKED(IDC_CSQ_WT, On_CSQ_ST)
	ON_BN_CLICKED(IDC_POSITION, On_Position_ST)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBeidouDlg message handlers

BOOL CBeidouDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	bIsTTSPlay = FALSE;

	m_DCom=1;
	m_DStopbits=1;
	m_DParity='N';
	m_DDatabits=8;
	m_DBaud=115200;

	m_DCom_WT=2;
	index_wakeup_times=0;
	SerialPortOpenCloseFlag_YW=0;
	modulereset=FALSE;
	soundswitch=FALSE;
	broad_prepare=0;
	for (int i=0;i<(sizeof(state_system)/sizeof(state_system[0]));i++)//��ӻ�ȡ���鳤�ȣ����ӽ�׳��
	{
		state_system[i]=1;//��ʼ��ʱ��Ĭ��ȫ�������á�ϵͳ����ģ�����״̬��־λ��
	}
	tel_manual_select=0;//��ʼ��
	msg_manual_select=0;//��ʼ��

	m_hIconRed  = AfxGetApp()->LoadIcon(IDI_ICON_RED);
	m_hIconOff	= AfxGetApp()->LoadIcon(IDI_ICON_OFF);
	GetDlgItem(IDC_COMBO_STOPBITS2)->SetWindowText(_T("����"));

	GetDlgItem(IDC_BUTTON_ICCHECK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3_POWERCHECK)->EnableWindow(FALSE);
//	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
//	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(FALSE);

//	GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(FALSE);
/****************************״̬��**************************************/
	m_StatBar=new CStatusBarCtrl;//״̬��
	RECT m_Rect; 
	GetClientRect(&m_Rect); //��ȡ�Ի���ľ�������
	m_Rect.top=m_Rect.bottom-20; //����״̬���ľ�������
	m_StatBar->Create(WS_BORDER|WS_VISIBLE|CBRS_BOTTOM,m_Rect,this,3); 
	
	int nParts[5]= {220,440,660,880,-1}; //�ָ�ߴ�385
	m_StatBar->SetParts(5, nParts); //�ָ�״̬��
	m_StatBar->SetText("���ߵ绰��δ����",0,0); //��һ����������"���ǵ�һ��ָʾ��"
	m_StatBar->SetText("���ǵ绰��δ����",1,0); //��������
	m_StatBar->SetText("3G��δ����",2,0); //��������
										/*Ҳ��ʹ�����·�ʽ����ָʾ������
										m_StatBar.SetPaneText(0,"���ǵ�һ��ָʾ��",0);
										����������m_StatBar->SetIcon(3,SetIcon(AfxGetApp()->LoadIcon(IDI_ICON3),FALSE));
										//�ڵ��ĸ������м���IDΪIDI_ICON3��ͼ��*/
//	m_StatBar->SetIcon(2,SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_OFF),FALSE));
	m_StatBar->SetText("������δ����",3,0); 
	m_StatBar->SetText("��ά�壺δ����",4,0); 
	m_StatBar->ShowWindow(SW_SHOW);
	
/********************1��������������***********************************/	
	m_comm.SetCommPort(1); //ѡ��com1
	m_comm.SetInputMode(1); //���뷽ʽΪ�����Ʒ�ʽ
	m_comm.SetInBufferSize(1024); //�������뻺������С
	m_comm.SetOutBufferSize(10240); //���������������С
	m_comm.SetSettings("115200,n,8,1"); //������115200����У�飬8������λ��1��ֹͣλ	 
	m_comm.SetRThreshold(1); //����1��ʾÿ�����ڽ��ջ��������ж��ڻ����1���ַ�ʱ������һ���������ݵ�OnComm�¼�
	m_comm.SetInputLen(0); //���õ�ǰ���������ݳ���Ϊ0
	//	 m_comm.GetInput();    //��Ԥ���������������������
	flag_bd_is_busy=0;//��ʼ��
/********************2�����ߵ绰��������***********************************/	
	m_comm_WT.SetCommPort(2); //ѡ��com2
	m_comm_WT.SetInputMode(1); //���뷽ʽΪ�����Ʒ�ʽ
	m_comm_WT.SetInBufferSize(1024); //�������뻺������С
	m_comm_WT.SetOutBufferSize(10240); //���������������С
	m_comm_WT.SetSettings("1200,n,8,1"); //������1200����У�飬8������λ��1��ֹͣλ	 
	m_comm_WT.SetRThreshold(1); //����1��ʾÿ�����ڽ��ջ��������ж��ڻ����1���ַ�ʱ������һ���������ݵ�OnComm�¼�
	m_comm_WT.SetInputLen(0); //���õ�ǰ���������ݳ���Ϊ0
	//	 m_comm_WT.GetInput();    //��Ԥ���������������������
	GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(FALSE);//~1~

	send_string="";//�����ַ�����գ�׼�����մ���
/********************3����ά��������***********************************/	
	m_comm_YW.SetCommPort(3); //ѡ��com3
	m_comm_YW.SetInputMode(1); //���뷽ʽΪ�����Ʒ�ʽ
	m_comm_YW.SetInBufferSize(1024); //�������뻺������С
	m_comm_YW.SetOutBufferSize(10240); //���������������С
	m_comm_YW.SetSettings("115200,n,8,1"); //������1200����У�飬8������λ��1��ֹͣλ	 
	m_comm_YW.SetRThreshold(1); //����1��ʾÿ�����ڽ��ջ��������ж��ڻ����1���ַ�ʱ������һ���������ݵ�OnComm�¼�
	m_comm_YW.SetInputLen(0); //���õ�ǰ���������ݳ���Ϊ0
	//	 m_comm_WT.GetInput();    //��Ԥ���������������������
/********************4��3G��������***********************************/	
	m_comm_3G.SetCommPort(4); //ѡ��com4
	m_comm_3G.SetInputMode(1); //���뷽ʽΪ�����Ʒ�ʽ
	m_comm_3G.SetInBufferSize(1024); //�������뻺������С
	m_comm_3G.SetOutBufferSize(10240); //���������������С
	m_comm_3G.SetSettings("115200,n,8,1"); //������1200����У�飬8������λ��1��ֹͣλ	 
	m_comm_3G.SetRThreshold(1); //����1��ʾÿ�����ڽ��ջ��������ж��ڻ����1���ַ�ʱ������һ���������ݵ�OnComm�¼�
	m_comm_3G.SetInputLen(0); //���õ�ǰ���������ݳ���Ϊ0
	//	 m_comm_3G.GetInput();    //��Ԥ���������������������
	call_in_number_3G="";
	flag_3G_in_busy=0;
	flag_3G_out_busy=0;
	bReadsmsTimeFlag = FALSE;
	bTimer48OnOff = FALSE;
	ChuanHaoID = "";
	bIsAutoResponseProcess = FALSE;
	strForTTS = "";
	bRoughLongSMS = FALSE;
	strShortSMSIndex = "";
	bRadioIsReady = FALSE;
	nCountCheck = 0;
	
	for(int t=0; t<SMSSIZE_10; t++)
	{
		SMSIndex[t] = "";
		strLongSMSTextStore[t] = "";
		strLongSMSIDStore[t] = "";
	}	
	longSMSStructINI();
	
	nLongSMSSendCount = 0;
	bIsINIProcess = FALSE;
	nCallFlags = 0;
	nATCmdID = 0;
	nEnglishLenth = 0;
	nChineseLenth = 0;
	nTotalLenth = 0;
	bIsCaller = TRUE;
	bIsRecord = 0;
	nSMSCount = 0;
	strSMSIndex = "";
	strLongSMSText = "";
	bIsNewSMS = FALSE;
	sms.bIsLongSMS = false;
	memset(cReceiveData,0,2048);
	memset(chPDU,0,1024);
	/******************5�����ǵ绰����***************************/
	flag_ST_in_busy=0;
	flag_ST_out_busy=0;
	

	bIsRecording = FALSE;
	nATCmdFlags = 0;
	bISCallProcess_WX = FALSE;
	bISCaller_WX = TRUE; 
	BISInitial_WX = FALSE;
	BINIfail_WX = FALSE;
	strdata = "";
/**********************************************************************/
	for(int j=0;j<received_frame_size;j++){
		frame_flag[j]=0;//û��֡�ȴ�����
		frame_len[j]=0;//����֡���±�
	}
	frame_index=0;//����ʹ�õ���֡
	framelen=0;
	comm_init=0;//�ȴ���ʼ��

	m_sata1.SetRange(0,4);
	m_sata2.SetRange(0,4);
	m_sata3.SetRange(0,4);
	m_sata1.SetStep(1);//���ý������ĵ�ǰλ��
    m_sata1.SetPos(0);
	m_sata2.SetStep(1);//���ý������ĵ�ǰλ��
    m_sata2.SetPos(0);
	m_sata3.SetStep(1);//���ý������ĵ�ǰλ��
	/****************************���̽���**********************************/
	GetWindowRect(&rectLarge);

	GetDlgItem(IDC_STATIC_PHONE)->GetWindowRect(&rectSeparator);
	rectSmall.left=rectSeparator.left;
	rectSmall.top=rectLarge.top;
	rectSmall.right=rectSeparator.right+27;
	rectSmall.bottom=rectLarge.bottom;
	SetWindowPos(NULL,0,0,rectSmall.Width(),rectSmall.Height(),SWP_NOMOVE|SWP_NOZORDER);

	GetDlgItem(IDC_STATIC_MESSAGE)->GetWindowRect(&rectSeparator);
	rectMiddle.left=rectSeparator.left;
	rectMiddle.top=rectLarge.top;
	rectMiddle.right=rectSeparator.right+30;
	rectMiddle.bottom=rectLarge.bottom;

	/**************************INI����*******************************/
	CFileFind finder;   //�����Ƿ����ini�ļ����������ڣ�������һ���µ�Ĭ�����õ�ini�ļ��������ͱ�֤�����Ǹ��ĺ������ÿ�ζ�����
	BOOL ifFind = finder.FindFile(_T(".\\config_phonemessage.ini"));
	if(!ifFind)
	{
		/**********��������**********************/
		::WritePrivateProfileString("ConfigInfo","com","4",".\\config_phonemessage.ini");//��������ѡ����
		::WritePrivateProfileString("ConfigInfo","com_WT","1",".\\config_phonemessage.ini");//��������ѡ����
		::WritePrivateProfileString("ConfigInfo","com_YW","2",".\\config_phonemessage.ini");//��������ѡ����
		::WritePrivateProfileString("ConfigInfo","com_3G","3",".\\config_phonemessage.ini");//��������ѡ����
		::WritePrivateProfileString("ConfigInfo","com_ST","0",".\\config_phonemessage.ini");//��������ѡ����
// 		::WritePrivateProfileString("ConfigInfo","parity","0",".\\config_phonemessage.ini");
// 		::WritePrivateProfileString("ConfigInfo","databits","0",".\\config_phonemessage.ini");
// 		::WritePrivateProfileString("ConfigInfo","speed","5",".\\config_phonemessage.ini");
// 		::WritePrivateProfileString("ConfigInfo","stopbits","0",".\\config_phonemessage.ini");
		
		::WritePrivateProfileString("ConfigInfo","com_r","5",".\\config_phonemessage.ini");//����������ֵ��
		::WritePrivateProfileString("ConfigInfo","com_r_WT","2",".\\config_phonemessage.ini");//��������ѡ����
		::WritePrivateProfileString("ConfigInfo","com_r_YW","3",".\\config_phonemessage.ini");//��������ѡ����
		::WritePrivateProfileString("ConfigInfo","com_r_3G","4",".\\config_phonemessage.ini");//��������ѡ����
		::WritePrivateProfileString("ConfigInfo","com_r_ST","1",".\\config_phonemessage.ini");//��������ѡ����
// 		::WritePrivateProfileString("ConfigInfo","parity_r","N",".\\config_phonemessage.ini");
// 		::WritePrivateProfileString("ConfigInfo","databits_r","8",".\\config_phonemessage.ini");
// 		::WritePrivateProfileString("ConfigInfo","speed_r","115200",".\\config_phonemessage.ini");
// 		::WritePrivateProfileString("ConfigInfo","stopbits_r","1",".\\config_phonemessage.ini");		
	}

	/**********��������**********************/
	CString strBufferReadConfig,strtmpReadConfig;

	GetPrivateProfileString("ConfigInfo","com_r","5",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	m_DCom= (int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);

	GetPrivateProfileString("ConfigInfo","com_r_WT","2",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	m_DCom_WT= (int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);

	GetPrivateProfileString("ConfigInfo","com_r_YW","3",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	m_DCom_YW= (int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);

	GetPrivateProfileString("ConfigInfo","com_r_3G","4",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	m_DCom_3G= (int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);

	GetPrivateProfileString("ConfigInfo","com_r_ST","1",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	m_DCom_ST= (int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);
	
	
// 	GetPrivateProfileString("ConfigInfo","parity_r","N",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
// 	strBufferReadConfig.ReleaseBuffer();
// 	strtmpReadConfig+=","+strBufferReadConfig;
// 	m_DParity= (char)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);
// 	
// 	GetPrivateProfileString("ConfigInfo","databits_r","8",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
// 	strBufferReadConfig.ReleaseBuffer();
// 	strtmpReadConfig+=","+strBufferReadConfig;
// 	m_DDatabits= (int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);
// 	
// 	GetPrivateProfileString("ConfigInfo","speed_r","115200",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
// 	strBufferReadConfig.ReleaseBuffer();
// 	strtmpReadConfig+=","+strBufferReadConfig;
// 	m_DBaud= (long)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);
// 	
// 	GetPrivateProfileString("ConfigInfo","stopbits_r","1",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
// 	strBufferReadConfig.ReleaseBuffer();
// 	strtmpReadConfig+=","+strBufferReadConfig;
// 	m_DStopbits= (int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);
	
	///////////////////////////////////////////////////////////////////////////
	GetPrivateProfileString("ConfigInfo","com","4",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_COMSELECT))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//���õ�n������Ϊ��ʾ�����ݡ�
	
	GetPrivateProfileString("ConfigInfo","com_WT","1",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_COMSELECT_WT))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//���õ�n������Ϊ��ʾ�����ݡ�

	GetPrivateProfileString("ConfigInfo","com_YW","2",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_COMSELECT_YW))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//���õ�n������Ϊ��ʾ�����ݡ�

	GetPrivateProfileString("ConfigInfo","com_3G","3",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_COMSELECT_3G))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//���õ�n������Ϊ��ʾ�����ݡ�

	GetPrivateProfileString("ConfigInfo","com_ST","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_COMSELECT_ST))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//���õ�n������Ϊ��ʾ�����ݡ�

	//�绰������ѡ·ģʽ
	((CComboBox*)GetDlgItem(IDC_COMBO_TEL_MANUAL_SELECT))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_MSG_MANUAL_SELECT))->SetCurSel(0);
	tel_mode_saved=0;

// 	GetPrivateProfileString("ConfigInfo","parity","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
// 	strBufferReadConfig.ReleaseBuffer();
// 	strtmpReadConfig+=","+strBufferReadConfig;
// 	((CComboBox*)GetDlgItem(IDC_COMBO_PARITY))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//���õ�n������Ϊ��ʾ�����ݡ�
// 	
// 	GetPrivateProfileString("ConfigInfo","databits","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
// 	strBufferReadConfig.ReleaseBuffer();
// 	strtmpReadConfig+=","+strBufferReadConfig;
// 	((CComboBox*)GetDlgItem(IDC_COMBO_DATABITS))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//���õ�n������Ϊ��ʾ�����ݡ�
// 	
// 	GetPrivateProfileString("ConfigInfo","speed","5",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
// 	strBufferReadConfig.ReleaseBuffer();
// 	strtmpReadConfig+=","+strBufferReadConfig;
// 	((CComboBox*)GetDlgItem(IDC_COMBO_SPEED))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//���õ�n������Ϊ��ʾ�����ݡ�
// 	
// 	GetPrivateProfileString("ConfigInfo","stopbits","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
// 	strBufferReadConfig.ReleaseBuffer();
// 	strtmpReadConfig+=","+strBufferReadConfig;
// 	((CComboBox*)GetDlgItem(IDC_COMBO_STOPBITS))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//���õ�n������Ϊ��ʾ�����ݡ�
	
	UpdateData(FALSE);
	/*********************************************************************/
	switch_state=0;//��绰
	WT_state=0;//�绰��״̬����ʼ����Ϊ����
	flag_PW_in_busy=0;//��ʼֵ
	flag_PW_out_busy=0;//��ʼֵ
	timer_board_disconnect_times_WT=0;
	timer_board_disconnect_times_YW=0;
	timer_board_disconnect_times_BD=0;
	timer_board_disconnect_times_3G=0;

	SerialPortOpenCloseFlag=FALSE;//Ĭ�Ϲرմ���
	SerialPortOpenCloseFlag_WT=FALSE;//Ĭ�Ϲر����ߵ绰�Ĵ���
	SerialPortOpenCloseFlag_YW=FALSE;//Ĭ�Ϲر���ά�Ĵ���
	SerialPortOpenCloseFlag_3G=FALSE;//Ĭ�Ϲر�3G�绰�Ĵ���
	SerialPortOpenCloseFlag_ST=FALSE;//Ĭ�Ϲر����ǵ绰�Ĵ���

	SetTimer(12,5000,NULL);//���ڽضϷ�����Ϣ�༭���е�����
	
	return TRUE;  // return TRUE  unless you set the focus to a control
	}
	
	void CBeidouDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBeidouDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBeidouDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BEGIN_EVENTSINK_MAP(CBeidouDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CBeidouDlg)
	ON_EVENT(CBeidouDlg, IDC_MSCOMM1, 1 /* OnComm */, OnComm1, VTS_NONE)
	ON_EVENT(CBeidouDlg, IDC_MSCOMM_WT, 1 /* OnComm */, OnComm_WT, VTS_NONE)
	ON_EVENT(CBeidouDlg, IDC_MSCOMM_YW, 1 /* OnComm */, OnComm_YW, VTS_NONE)
	ON_EVENT(CBeidouDlg, IDC_MSCOMM_3G, 1 /* OnComm */, OnComm_3G, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void CBeidouDlg::OnComm1() 
{
	// TODO: Add your control notification handler code here
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len,k;
	BYTE rxdata[2048]; //����BYTE����
	static bool frameplus=0;//0:$Ϊ��һλ��1��$�ڴ����м�
	CString strDisp="",strTmp="";

	if((m_comm.GetCommEvent()==2)) //�¼�ֵΪ2��ʾ���ջ����������ַ�
	{	
		Sleep(100);//һ֡�ϳ���Ϊ������հ�֡�ʹ�����Ӧ���������δ������ж϶���Ӵ����������롣
		//�˴�����һ�ֱ�ͨ�ķ�ʽ������һ���ڴ����¼�ʱ���ӳ�һ���ٶ����ݣ���֤ÿ֡���������ġ�I'm very smart������
		//������������Ķ�֡��������ʽ�����ܼ�֡�����������£�����$���ֿ��洢����������һ֡�ٴ������ַ�����֤����©֡

		variant_inp=m_comm.GetInput(); //��������
		safearray_inp=variant_inp;  //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���
		len=safearray_inp.GetOneDimSize(); //�õ���Ч���ݳ���
		for(k=0;k<len;k++)
		{
			safearray_inp.GetElement(&k,rxdata+k);//ת��ΪBYTE������
		}

//			AfxMessageBox("OK",MB_OK,0);
//			frame=frame_len[frame_index];
//			frame_len[frame_index]=0;


		for(k=0;k<len;k++)//������ת��ΪCString����
		{
			BYTE bt=*(char*)(rxdata+k);    //�ַ���
			if (bt=='$')
			{
				if (rxdata[0]=='$')
				{
					frameplus=0;//0:'$'Ϊ��һλ��1��'$'�ڴ����м�
				} 
				else//'$'�����������м�λ��
				{
					frameplus=1;
					frame_index++;//��һ���ѽ��գ���Ҫ����
				}

				if (frame_index!=0)
				{
	//				frame_index++;//�µ�һ֡����
					if(frameplus==1)//ֻ��һ������֡������ʱ�Ž��������ҽ�ȷ���������յ���֡���д���
					{
						frame_flag[frame_index-1]=1;//��һ������֡���Ϊ������״̬��
						frame_len[frame_index-1]=framelen;//��һ������֡���ȼ�¼������
						framelen=0;
						for (short i=0;i<received_frame_size;i++)
						{
							if (frame_flag[i]==1)
							{
								decodeheads (frame_receive[i]);
								frame_flag[i]=0;//���Ϊδʹ��
							}
						}
						
					}
					if(frame_index==received_frame_size)frame_index=0;//�������飬����һ����ٴӵ�һ����ʼ�á�Ĭ�ϲ���һ�ΰѻ���ȫ������
				}
				
			}
			frame_receive[frame_index][framelen]=bt;
			framelen++;
//			strTmp.Format("%02x ",bt);    //���ַ�������ʱ����strtemp���
//			strDisp+=strTmp;  //������ձ༭���Ӧ�ַ���
			
		}//���λ������е�����ȫ��ȡ�������洢����

		if (framelen==(unsigned char)(frame_receive[frame_index][5]*256+frame_receive[frame_index][6]))//�Ա��ν��յĻ���֡�����һ����ֻ��һ��������е����һ���������������ϣ����д���
		{
			//֡�������
// 			CString buf;
//			buf.Format("%d",frame_index);
// 			AfxMessageBox(buf,MB_OK,0);

			frame_len[frame_index]=framelen;
			framelen=0;
			frame_flag[frame_index]=1;
			for (short i=0;i<received_frame_size;i++)
			{
				if (frame_flag[i]==1)
				{
					decodeheads (frame_receive[i]);
					frame_flag[i]=0;//���Ϊδʹ��
				}
			}
			if(frame_index==received_frame_size) frame_index=0;
			
		}
//		m_showmsg+=strDisp;
//		m_showmsg+="\r\n";
		UpdateData(FALSE);
	}//end of �����¼�	
}



void CBeidouDlg::OnOpencloseport() 
{
	// TODO: Add your control notification handler code here
// 	char buff[2];
// 	CString string1="",string2="";
// 	buff[1]='\0';
// 	buff[0]=m_DParity;
// 	string1.Format(_T("%d"),m_DBaud);
// 	string1+=",";
// 	string2=buff;
// 	string1+=string2;
// 	string1+=",";
// 	string2.Format(_T("%d"),m_DDatabits); 
// 	string1+=string2;
// 	string1+=",";
// 	string2.Format(_T("%d"),m_DStopbits);
// 	string1+=string2;
	CString string1="115200,n,8,1";
/*
	CString   tmp;
	tmp.Format( "%d ",string1);
	MessageBox( "config:"+string1);
*/
	if(SerialPortOpenCloseFlag==FALSE)
	{
		SerialPortOpenCloseFlag=TRUE;

		//�����Ǵ��ڵĳ�ʼ������
		if(m_comm.GetPortOpen())//�򿪶˿�ǰ�ļ�⣬�ȹأ��ٿ�
			MessageBox("�����޷���");
//			m_comm.SetPortOpen(FALSE);	//	
		m_comm.SetCommPort(m_DCom); //ѡ��˿ڣ�Ĭ����com1
		m_comm.SetSettings((LPSTR)(LPCTSTR)string1); //������256000����У�飬8������λ��1��ֹͣλ
		if(!m_comm.GetPortOpen())
		{			
			m_comm.SetPortOpen(TRUE);//�򿪴���
			GetDlgItem(IDC_OPENCLOSEPORT)->SetWindowText("�رմ���");
			m_ctrlIconOpenoff.SetIcon(m_hIconRed);
			m_StatBar->SetText("����������������",3,0);
			GetDlgItem(IDC_BUTTON_ICCHECK)->EnableWindow(TRUE);
//			GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(TRUE);
//			GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON3_POWERCHECK)->EnableWindow(TRUE);
			
			Sleep(20);//�ȴ������ȶ����ٷ��ͱ����Ĳ�ѯ��Ϣ
			//�򿪴��ں����IC��ѯ��ȡ������
			CByteArray Array;
			Array.RemoveAll();
            Array.SetSize(12);
			for (int i=0; i<12; i++)
			{
				Array.SetAt(i,frame_IC_check[i]);
			}
			
			
			if(m_comm.GetPortOpen())
			{
				m_comm.SetOutput(COleVariant(Array));//��������
			}
			SetTimer(8,(QUERY_INTERVAL+QUERY_BD),NULL);//�Զ�ģʽ�²Ŵ򿪶�ʱ��
		}
		else
			MessageBox("can not open serial port");	 
	}
	else
	{
		SerialPortOpenCloseFlag=FALSE;
		GetDlgItem(IDC_OPENCLOSEPORT)->SetWindowText("�򿪴���");
		m_ctrlIconOpenoff.SetIcon(m_hIconOff);
		m_comm.SetPortOpen(FALSE);//�رմ���
		m_StatBar->SetText("�����������ѶϿ�",3,0);
		state_system[4]=1;//�������ڹر�
		m_board_led_BD.SetIcon(m_hIconOff);
		m_bd_r.SetIcon(m_hIconOff);
		GetDlgItem(IDC_STATIC_BOARDCONNECT_BD)->SetWindowText(" �����ѶϿ���");
		KillTimer(8);
		GetDlgItem(IDC_BUTTON_ICCHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON3_POWERCHECK)->EnableWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(FALSE);
	}
}

void CBeidouDlg::OnSelendokComboComselect() 
{
	// TODO: Add your control notification handler code here
	m_DCom=m_Com.GetCurSel()+1;
	UpdateData();	

	CString strTemp;
	strTemp.Format(_T("%d"),m_DCom-1);
	::WritePrivateProfileString("ConfigInfo","com",strTemp,".\\config_phonemessage.ini");
	
	strTemp.Format(_T("%d"),m_DCom);
	::WritePrivateProfileString("ConfigInfo","com_r",strTemp,".\\config_phonemessage.ini");
}

// void CBeidouDlg::OnSelendokComboSpeed() 
// {
// 	// TODO: Add your control notification handler code here
// 	int i=m_Speed.GetCurSel();
// 
// 	CString strTemp;
// 	strTemp.Format(_T("%d"),i);
// 	::WritePrivateProfileString("ConfigInfo","speed",strTemp,".\\config_phonemessage.ini");
// 
// 	switch(i)
// 	{
// // 	case 0:
// // 		i=300;
// // 		break;
// // 	case 1:
// // 		i=600;
// // 		break;
// // 	case 2:
// // 		i=1200;
// // 		break;
// // 	case 3:
// // 		i=2400;
// // 		break;
// // 	case 4:
// // 		i=4800;
// // 		break;
// // 	case 5:
// // 		i=9600;
// // 		break;
// // 	case 6:
// // 		i=19200;
// // 		break;
// // 	case 7:
// // 		i=38400;
// // 		break;
// // 	case 8:
// // 		i=43000;
// // 		break;
// // 	case 9:
// // 		i=56000;
// // 		break;
// // 	case 10:
// // 		i=57600;
// // 		break;
// // 	case 11:
// // 		i=115200;
// // 		break;
// // 	default:
// // 		break;
// 	case 0:
// 		i=9600;
// 		break;
// 	case 1:
// 		i=19200;
// 		break;
// 	case 2:
// 		i=43000;
// 		break;
// 	case 3:
// 		i=56000;
// 		break;
// 	case 4:
// 		i=57600;
// 		break;
// 	case 5:
// 		i=115200;
// 		break;
// 	default:
// 		break;
// 		
// 	}
// 	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //����ָ��Ӧ�ó������ָ��
// 	app->m_nBaud=i;
// 	m_DBaud=app->m_nBaud;
// 	UpdateData();	
// 
// 	strTemp.Format(_T("%d"),m_DBaud);
// 	::WritePrivateProfileString("ConfigInfo","speed_r",strTemp,".\\config_phonemessage.ini");
// 
// }
// 
// void CBeidouDlg::OnSelendokComboParity() 
// {
// 	// TODO: Add your control notification handler code here
// 	char temp;
// 	int i=m_Parity.GetCurSel();
// 
// 	CString strTemp;
// 	strTemp.Format(_T("%d"),i);
// 	::WritePrivateProfileString("ConfigInfo","parity",strTemp,".\\config_phonemessage.ini");
// 
// 	switch(i)
// 	{
// 	case 0:
// 		temp='N';
// 		break;
// 	case 1:
// 		temp='O';
// 		break;
// 	case 2:
// 		temp='E';
// 		break;
// 	}
// 	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //����ָ��Ӧ�ó������ָ��
// 	app->m_cParity=temp;
// 	m_DParity=app->m_cParity;
// 	UpdateData();	
// 
// 	strTemp.Format(_T("%c"),m_DParity);
// 	::WritePrivateProfileString("ConfigInfo","parity_r",strTemp,".\\config_phonemessage.ini");
// 
// }
// 
// void CBeidouDlg::OnSelendokComboDatabits() 
// {
// 	// TODO: Add your control notification handler code here
// 	int i=m_DataBits.GetCurSel();
// 
// 	CString strTemp;
// 	strTemp.Format(_T("%d"),i);
// 	::WritePrivateProfileString("ConfigInfo","databits",strTemp,".\\config_phonemessage.ini");
// 
// 	switch(i)
// 	{
// 	case 0:
// 		i=8;
// 		break;
// 	case 1:
// 		i=7;
// 		break;
// 	case 2:
// 		i=6;
// 		break;
// 	}
// 	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //����ָ��Ӧ�ó������ָ��
// 	app->m_nDatabits=i;
// 	m_DDatabits=app->m_nDatabits;
// 	UpdateData();
// 	
// 	strTemp.Format(_T("%d"),m_DDatabits);
// 	::WritePrivateProfileString("ConfigInfo","databits_r",strTemp,".\\config_phonemessage.ini");
// }
// 
// void CBeidouDlg::OnSelendokComboStopbits() 
// {
// 	// TODO: Add your control notification handler code here
// 	int i=m_StopBits.GetCurSel();
// 
// 	CString strTemp;
// 	strTemp.Format(_T("%d"),i);
// 	::WritePrivateProfileString("ConfigInfo","stopbits",strTemp,".\\config_phonemessage.ini");
// 
// 	switch(i)
// 	{
// 	case 0:
// 		i=1;
// 		break;
// 	case 1:
// 		i=2;
// 		break;
// 	}
// 	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //����ָ��Ӧ�ó������ָ��
// 	app->m_nStopbits=i;
// 	m_DStopbits=app->m_nStopbits;
// 	UpdateData();
// 	
// 	strTemp.Format(_T("%d"),m_DStopbits);
// 	::WritePrivateProfileString("ConfigInfo","stopbits_r",strTemp,".\\config_phonemessage.ini");
// }

void CBeidouDlg::OnButtonSendMsg() 
{
	// TODO: Add your control notification handler code here
//		CString sendTmp="";
	UpdateData(FALSE);
	if (state_system[3]==0)//������ʱ:3G���ã�����3G
	{
		if(bTimer48OnOff == FALSE)
		{
			bTimer48OnOff = TRUE;
			SetTimer(48,120*1000,NULL);//���Ž���������û�أ�����CSQ
		}
		KillTimer(10);
		if((flag_3G_in_busy==1)||(flag_3G_out_busy==1))this->GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(FALSE);
		UpdateData(TRUE);  		
		BOOL bIsSendSMS = FALSE;	
		BOOL bChinese = FALSE;
		CString stra = "";
		bool bIsSendLongSMS = FALSE;
		
		strCount countNum;
		countNum = Statistic(m_sendmsg);//�ַ�ͳ��
		nEnglishLenth = countNum.nEnglishLenth;
		nChineseLenth = countNum.nChineseLenth;
		nTotalLenth = countNum.nTotalLenth;
		stra.Format("Ӣ: %d,��: %d\r\n����: %d",nEnglishLenth , nChineseLenth , nTotalLenth);
		m_StatBar->SetText("3G״̬: "+stra,2,0); //��������
		if (nChineseLenth == 0)
			bChinese = FALSE;	
		else
			bChinese = TRUE;
		
		if (bChinese)
		{	//�ж������ַ��Ƿ񳬳� 16bit����ַ���70��  //���ü�����Ӣ���ַ��ĸ���		
			if((nChineseLenth+nEnglishLenth) <= 70 )
				bIsSendSMS = TRUE;
			else if (((nChineseLenth+nEnglishLenth) > 70)&&((nChineseLenth+nEnglishLenth) <= 330 ) ) //��ʵ��3��
			{
				bIsSendLongSMS = TRUE;
				nLongSMSSendCount = nTotalLenth/66 + 1;
			}
		} 
		else
		{	//�ж�Ӣ���ַ��Ƿ񳬳� 7bit����ַ���160��
			if(nEnglishLenth <= 160 )
				bIsSendSMS = TRUE;
		}
		
		if ((bIsSendSMS == TRUE)||(bIsSendLongSMS == TRUE))
		{	
			funReturn d = {FALSE,0};//Ĭ��ΪFALSE,0�ַ�
			memset(chPDU,0,512);
			if (bIsSendSMS == TRUE)
			{
		//		d = cEncodePDU(m_strSendPhoneNum,m_strReceicePhoneNum,m_strSMSText,chPDU,bChinese,nTotalLenth);//�����ˣ��ռ��ˣ�����
				d = cEncodePDU(m_target_number,m_sendmsg,chPDU,bChinese,nTotalLenth);//00���ռ��ˣ�����  //Ĭ��������
				if (d.bToReturn)
				{
					if(d.nLenthToReturn <=200)
					{
						CString strTemp = "";
						SendAtCmd("AT+CMGF=0",21); //PDUģʽ
						strTemp.Format("%d",d.nLenthToReturn);
						SendAtCmd("AT+CMGS="+strTemp,22);
						nATCmdID = 23;
						SetTimer(46,1000,NULL);
						bTimer48OnOff = FALSE;
						KillTimer(48);
						SetTimer(10,(QUERY_INTERVAL+QUERY_3G),NULL);//���Ŷ����ˣ��ٴ�3G��ѯ��ʱ��
					}
					else
					{
						m_StatBar->SetText("3G״̬: ���ŷ��ͳ���",2,0); //��������
					}
				}
				else
					m_StatBar->SetText("3G״̬: ���ŷ���ʧ�ܣ���������",2,0); //��������
			}
			else if (bIsSendLongSMS == TRUE)
			{
				longSMSStruct.bChineseFlag = bChinese;
				longSMSStruct.nTextLenth = nTotalLenth;
				longSMSStruct.strDstNum = m_target_number;
				longSMSStruct.strSMSText = m_sendmsg;
				longSMSStruct.nCount = nLongSMSSendCount;
				srand((unsigned)time(NULL));
				longSMSStruct.nLongSMSID = rand() % 100; 
				SendLongSMS(&longSMSStruct);	
				SetTimer(48,60*1000,NULL);// ��ֹ�����ų������ڵ�PDU����
			}
		} 
		else
			AfxMessageBox("�����ַ�������!�ַ��������������330��Ӣ���������160");	
	}
	else if(state_system[4]==0){//������ʱ:3G�����ã����ñ���
		KillTimer(8);
		UpdateData();
		if (m_target_number.GetLength()!=6)//(m_otherID==0)
		{
			AfxMessageBox("����ʹ�ñ���ͨ�ţ��Է�����������6λ���֡�");
			return;
		}
		if(m_sendmsg!="")
		{
			int timer_range=(m_category>1)?m_category:60;//ֻ�е������ķ���Ƶ�Ȼ�ȡ����>0��ʱ���Ÿ��������ý�����������ʹ��Ĭ��ֵ60	
			m_timer.SetRange(0,timer_range);
			m_timer.SetPos(0);
			m_timer.SetStep(1);//���ý������ĵ�ǰλ��

			unsigned char hexdata[250];
			int len=strHex(m_sendmsg,hexdata,250);
			if (len>72)
			{	
				CString temp;
				temp.Format("%d",len-72);
				temp="�����������ݳ���"+temp;
				temp+="���ַ������޸ĺ����ԣ�";
				AfxMessageBox(temp);
				return;
			}
			CByteArray Array;
			Array.RemoveAll();
			int total_len=len+19;
			Array.SetSize(total_len);

			frame_MSG_check[5]=total_len/256;//���ݰ��ܳ���
			frame_MSG_check[6]=total_len%256;
			
			frame_MSG_check[10]=0x46;//Ĭ��ʹ�á���ࡱ��ʽ��������

			int target_id=_ttoi(m_target_number);
			frame_MSG_check[11]=(unsigned char)(target_id/pow(2, 16));//�Է�ID
			frame_MSG_check[12]=(unsigned char)(target_id/pow(2, 8));
			frame_MSG_check[13]=(unsigned char)(target_id%256);

			frame_MSG_check[14]=8*len/256;//���ĳ���
			frame_MSG_check[15]=8*len%256+8;

			frame_MSG_check[16]=0;//�Ƿ�Ӧ��
			frame_MSG_check[17]=0x4A;//���ʱ�˴���Ҫ����0x4A

			for (int i=0; i<18; i++)
			{
				Array.SetAt(i,frame_MSG_check[i]);
			}
			int k=0;
			for (int j=18; j<(len+18); j++)
			{
				Array.SetAt(j,hexdata[k]);
				frame_MSG_check[j]=hexdata[k];
				k++;
			}
			frame_MSG_check[total_len-1]=XOR(frame_MSG_check,total_len-1);

			Array.SetAt(total_len-1,frame_MSG_check[total_len-1]);
			if(m_comm.GetPortOpen())
			{
				m_comm.SetOutput(COleVariant(Array));//��������
			}
			SetTimer(1,timer_range*1000,NULL);//61���ʹ��
			SetTimer(2,1000,NULL);//ÿ���ж�һ�Σ�����һ�ν�������ֵ
			GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
			flag_bd_is_busy=1;//���������У�æµ��
		}else{
			AfxMessageBox("������������ݡ�");
		} 
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
		SetTimer(8,(QUERY_INTERVAL+QUERY_BD),NULL);
	}else{
		AfxMessageBox("���޶�����·���ã����Ե����ԣ�");
	}
}

void CBeidouDlg::OnButtonClear() 
{
	// TODO: Add your control notification handler code here
		m_showmsg="";
		strLongSMSText =  "";
		UpdateData(FALSE);	
}

//DEL void CBeidouDlg::OnButtonSystemcheck() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL // 	m_sata1.SetPos(0);
//DEL // 	m_sata2.SetPos(0);
//DEL //  	m_sata3.SetPos(0);
//DEL // 	m_cardnumber=0;
//DEL // 	m_cardstate="";
//DEL // 	m_basestate="";
//DEL // 
//DEL // 	CByteArray Array;
//DEL // 	Array.RemoveAll();
//DEL // 	Array.SetSize(12);
//DEL // 	for (int i=0; i<12; i++)
//DEL // 	{
//DEL // 		Array.SetAt(i,frame_SYS_check[i]);
//DEL // 	}
//DEL // 	
//DEL // 	
//DEL // 	if(m_comm.GetPortOpen())
//DEL // 	{
//DEL // 		m_comm.SetOutput(COleVariant(Array));//��������
//DEL // 	}
//DEL // 	UpdateData(FALSE);
//DEL }

void CBeidouDlg::OnButtonIccheck() 
{
	// TODO: Add your control notification handler code here
	m_cardnumber=0;
	m_category=0;
	m_cardstate ="";
	m_basestate="";

	CByteArray Array;
	Array.RemoveAll();
	Array.SetSize(12);
	for (int i=0; i<12; i++)
	{
		Array.SetAt(i,frame_IC_check[i]);
	}
	
	
	if(m_comm.GetPortOpen())
	{
		m_comm.SetOutput(COleVariant(Array));//��������
	}	
	UpdateData(FALSE);
}

void CBeidouDlg::OnButton3Powercheck() 
{
	// TODO: Add your control notification handler code here	
	m_sata1.SetPos(0);
	m_sata2.SetPos(0);
 	m_sata3.SetPos(0);

	CByteArray Array;
	Array.RemoveAll(); 
	Array.SetSize(12);
	for (int i=0; i<12; i++)
	{
		Array.SetAt(i,frame_POWER_check[i]);
	}
	
	
	if(m_comm.GetPortOpen())
	{
		m_comm.SetOutput(COleVariant(Array));//��������
	}
	UpdateData(FALSE);
}




void CBeidouDlg::DeIcc(unsigned char *BUFF)
{
	long _Useraddr;
	unsigned int IccFrq;
	unsigned char comlev;

	_Useraddr=((long)BUFF[7])*pow(2, 16)+((long)BUFF[8])*pow(2, 8)+((long)BUFF[9]);
	IccFrq= ((int)BUFF[15])*pow(2, 8)+((int)BUFF[16]);
	comlev = BUFF[15];
	
	state_system[4]=0;//����Ӧ�𣺱�Ǳ�������
	timer_board_disconnect_times_BD=0;//��ѯ����������
	m_board_led_BD.SetIcon(m_hIconRed);
	m_bd_r.SetIcon(m_hIconRed);
	GetDlgItem(IDC_STATIC_BOARDCONNECT_BD)->SetWindowText(" ���������ӣ�");
	m_FKXX+="�������Ӳ�ѯ������\r\n";
	UpdateData(FALSE);

	if ((comm_init==0)&&(_Useraddr!=0))//���γ�ʼ�����ڣ��������������ɸ�֡
	{
		comm_init=1;
//		frame_POWER_check[7]=0;
//		frame_POWER_check[8]=0;
//		frame_POWER_check[9]=0;
//		frame_POWER_check[10]=0;
//		frame_POWER_check[11]=XOR(frame_POWER_check,11);

//		frame_SYS_check[7]=0;
//		frame_SYS_check[8]=0;
//		frame_SYS_check[9]=0;
//		frame_SYS_check[10]=XOR(frame_SYS_check,10);

		frame_MSG_check[7]=0;
		frame_MSG_check[8]=0;
		frame_MSG_check[9]=0;
		
		GetDlgItem(IDC_BUTTON3_POWERCHECK)->EnableWindow(TRUE);
//		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
	}

	m_cardnumber=_Useraddr; 
	m_category=IccFrq;
	m_cardstate ="����";
	m_basestate="����";
	UpdateData(FALSE);
} 

void CBeidouDlg::decodeheads(unsigned char *BUFF)
{
	int command_type = 0;
	unsigned char result = 0;
	int len = BUFF[5]*256+BUFF[6],i;
	
	for(result=BUFF[0],i=1;i<(len-1);i++)//len+10
	{
		result ^= BUFF[i];
	}
	
	if(result==BUFF[len-1])
	{
		if(strncmp((const char *)BUFF, "$GLZK",5)==0) command_type= Sig;//���ʼ��		
		else if (strncmp((const char *)BUFF, "$TXXX",5)==0) command_type= Msg;//ͨ������		
		else if (strncmp((const char *)BUFF, "$ICXX",5)==0) command_type= Icc;//IC��ѯ
		else if (strncmp((const char *)BUFF, "$XTZJ",5)==0) command_type= Sts;//ϵͳ�Լ�
		else if (strncmp((const char *)BUFF, "$FKXX",5)==0) command_type= FKXX;//������Ϣ
		else if (strncmp((const char *)BUFF, "$Zst_",5)==0) command_type= Zst;//
		else if (strncmp((const char *)BUFF, "$Zrd_",5)==0) command_type= Zrd;//
		else if (strncmp((const char *)BUFF, "$Tim_",5)==0) command_type= Tim;//
		else if (strncmp((const char *)BUFF, "$Bst_",5)==0) command_type= Bst;//
		else if (strncmp((const char *)BUFF, "$DWSQ",5)==0) command_type= Pos;//

		
		
		switch(command_type)
		{
		case Sig://���ʼ��
			DeSig(BUFF);
			break;
		case Msg://ͨ������
			DeMsg(BUFF);
			break;
		case FKXX://������Ϣ
			DeFKXX(BUFF);
			break;
		case Icc://IC��ѯ
			DeIcc(BUFF);
			break;
		case Sts://ϵͳ�Լ�
			DeSts(BUFF);
			break;
		case Bst://�������
			break;
		case Zst://��ֵ����
			break;
		case Zrd://��ֵ��ȡ
			break;
		case Tim://ʱ�����
			break;
		case Pos://��λ����
			break;
		default:
			break;
		}		
	}		
}

unsigned char CBeidouDlg::XOR(unsigned char *BUFF, int len)
{
	unsigned char result=0;
	int i;
	for(result=BUFF[0],i=1;i<len;i++)
	{
		result ^= BUFF[i];
	}
	return result;
}

void CBeidouDlg::DeSig(unsigned char *BUFF)
{
//	long _Useraddr;
	unsigned char Sigx[6]={0},power_buf=0;
	
	for(short i = 0;i<6;i++)
	Sigx[i]=BUFF[i+10];
//	_Useraddr=((long)BUFF[7])*pow(2, 16)+((long)BUFF[8])*pow(2, 8)+((long)BUFF[9]); 

	power_buf=(Sigx[0]>Sigx[1])?Sigx[0]:Sigx[1];
	m_sata1.SetPos(power_buf);
	power_buf=(Sigx[2]>Sigx[3])?Sigx[2]:Sigx[3];
	m_sata2.SetPos(power_buf);
	power_buf=(Sigx[4]>Sigx[5])?Sigx[4]:Sigx[5];
	m_sata3.SetPos(power_buf);
	UpdateData(FALSE);
}

void CBeidouDlg::DeSts(unsigned char *BUFF)
{
	long _Useraddr;
	unsigned char Sigx[6]={0},power_buf=0;

	if(BUFF[10]&0x80)
		m_cardstate ="�쳣";
	else
		m_cardstate ="����";

	_Useraddr=((long)BUFF[11])*pow(2, 16)+((long)BUFF[12])*pow(2, 8)+((long)BUFF[13]);
	m_cardnumber=_Useraddr;

//	if(BUFF[14]&0x01)
//		m_basestate="����";
//	else
//		m_basestate="������";

	if(BUFF[14]&0x02)
		m_basestate="����";
	else
		m_basestate="��Ĭ";
	
	for(short i = 0;i<6;i++)
		Sigx[i]=BUFF[i+15];	

	power_buf=(Sigx[0]>Sigx[1])?Sigx[0]:Sigx[1];
	m_sata1.SetPos(power_buf);
	power_buf=(Sigx[2]>Sigx[3])?Sigx[2]:Sigx[3];
	m_sata2.SetPos(power_buf);
	power_buf=(Sigx[4]>Sigx[5])?Sigx[4]:Sigx[5];
	m_sata3.SetPos(power_buf);

	UpdateData(FALSE);
}

//DEL void CBeidouDlg::DeMsg(char *BUFF)
//DEL {
//DEL 	int i=0;
//DEL 	int len;
//DEL 	char rec_text[100];
//DEL 
//DEL 	Recvaddr=((long)BUFF[10])*pow(2,16)+((long)BUFF[11])*pow(2,8)+((long)BUFF[12]);
//DEL 	Hour = BUFF[13];
//DEL 	Min = BUFF[14];
//DEL 	len = ((int)BUFF[15])*pow(2,8)+((int)BUFF[16]);
//DEL 	for(i=0;i<(int)(len/8);i++)
//DEL 	{
//DEL 		rec_text[i]=BUFF[17+i];
//DEL 	}
//}

void CBeidouDlg::DeMsg(unsigned char *BUFF)
{
 	int i=0,len;
	long Recvaddr;
	short Hour;
	short Min;
 	CString strTmp,strDisp;
	unsigned char rec_text[650]={0};//���֧�ֽ���600���ַ�
 
 	Recvaddr=((long)BUFF[11])*pow(2,16)+((long)BUFF[12])*pow(2,8)+((long)BUFF[13]);
 	Hour = BUFF[14];
 	Min = BUFF[15];
 	len = ((int)BUFF[16])*pow(2,8)+((int)BUFF[17]);
 	for(i=0;i<(int)(len/8);i++)
 	{
 		rec_text[i]=BUFF[19+i];
 	}

	for (int j=0;j<(BUFF[5]*256+BUFF[6]);j++)
	{
		strTmp.Format("%02x ",BUFF[j]);    //���ַ�������ʱ����strtemp���
		strDisp+=strTmp;  //������ձ༭���Ӧ�ַ���
	}
	
	CTime curtime = CTime::GetCurrentTime();
	CString strTime = curtime.Format("%Y-%m-%d %H:%M:%S");

	m_showmsg+=strTime;
	m_showmsg+="��";
	strTmp.Format("%d",Recvaddr);
	m_showmsg+=strTmp;
	m_showmsg+="�յ���Ϣ��\r\n";
	m_showmsg+=rec_text;
//	m_showmsg+="\t\t���ݰ���";
//	m_showmsg+=strDisp;
	m_showmsg+="\r\n";
	strTmp = "";
	strTmp = (char*)rec_text;
	strForTTS = strTmp;
	ttsStruct.nPlayTimers = 1; //��ʱ��������
	ttsStruct.strTransmit = (LPCSTR)strTmp;
//	pThread=AfxBeginThread(ThreadProc, &ttsStruct);
	pThread=AfxBeginThread(ThreadProc, this);
//	TTSModule(strTmp);
}

void CBeidouDlg::DeFKXX(unsigned char *BUFF)
{
	long _Useraddr;
	_Useraddr=((long)BUFF[7])*pow(2,16)+((long)BUFF[8])*pow(2,8)+((long)BUFF[9]);
	CString temp;
	temp.Format("%d",BUFF[11]);
	
	switch(BUFF[10])
	{
	case 0x00:
		m_FKXX="������Ϣ�����ͳɹ���";
		break;
	case 0x01:
		m_FKXX="������Ϣ������ʧ�ܣ�";
		break;
	case 0x02:
		m_FKXX="�������ź�Ϊ������";
		break;
	case 0x03:
		m_FKXX="�����ĵ������㣡";
		break;
	case 0x04:
		m_FKXX="�����ķ���Ƶ��δ����\r\n��"+temp+"������ԡ�";
		break;
	case 0x05:
		m_FKXX="�����ļӽ��ܴ���";
		break;
	case 0x06:
		m_FKXX="������CRC����";
		break;
	default:
		m_FKXX="��������չ���ܡ�";
		break;
		}
	UpdateData(FALSE);
}

//DEL void CBeidouDlg::EnMsg()
//DEL {
//DEL // 	char vlen[2];
//DEL // 	char vkeywordcnt = len_text+6;
//DEL // 	char vkeyword[100];
//DEL // 	vlen[0]=((len_text+17)>>8)&0x00ff;
//DEL // 	vlen[1]=(len_text+17)&0x00ff;
//DEL // 	vkeyword[0]=(Sendaddr>>16)&0x000000ff;
//DEL // 	vkeyword[1]=(Sendaddr>>8)&0x000000ff;
//DEL // 	vkeyword[2]=(Sendaddr)&0x000000ff;
//DEL // 	if(Format==0)
//DEL // 		vkeyword[3]=0x46;
//DEL // 	else
//DEL // 		vkeyword[3]=0x44;	
//DEL // 	vkeyword[4]=(len_text>>8)&0x00ff;
//DEL // 	vkeyword[5]=(len_text)&0x00ff;
//DEL // 	for(int i = 0;i<len_text;i++)
//DEL // 		vkeyword[i]=text[i];
//DEL //	ProtFrame(heads[Msg],vlen,vkeyword,vkeywordcnt);
//DEL }

int CBeidouDlg::strHex(CString str, unsigned char *data, int num)
{
	int i=0,len=str.GetLength();
	
	memset(data,0x00,num);
	memcpy(data,(LPCSTR)str,str.GetAllocLength());
	
	for(i=0;i<len;)
	{
//		unsigned char buf=0;
		if (data[i]>128)    //((0<h)&&(h<128))     //�жϴ��ַ��Ƿ�Ϊ����
		{
//			buf=data[i];
//			data[i]=data[i+1];
//			data[i+1]=buf;
			i+=2;
		}
		else
		{
			i++;
		}
	}
	
	return i;
}

void CBeidouDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent==1)//����������Ϣ����ťʧ��60s
	{
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		flag_bd_is_busy=0;//������ɣ���Ǳ���������
		KillTimer(1);
	} 
	else if(nIDEvent==2)//ÿ���ж�һ�Σ�����һ�ν�������ֵ
	{
		m_timer.StepIt();//ÿ���ж�һ�Σ�����һ�ν�������ֵ
		int timer_range=(m_category>1)?m_category:60;//ֻ�е������ķ���Ƶ�Ȼ�ȡ����>0��ʱ���Ÿ��������ý�����������ʹ��Ĭ��ֵ60
	if(m_timer.GetPos()==timer_range){
		m_timer.SetPos(0);
		KillTimer(2);	
       }
		UpdateData(FALSE);
	}else if (nIDEvent==3)
	{
		OnButtonCall();//ʹժ���������û�͸�����Ȳ���һ��ժ���ٵ���һ�Σ����ڲ���
		KillTimer(3);
	}else if(nIDEvent==4){//���ߵ绰�����ּ�
		if((tel_manual_select==0)||(m_tel_manual_select.GetCurSel()==1)){//�Զ�ģʽ,����ָ�������ߵ绰ʱ�Ų�ѯ
		char lpOutBuffer[] = {'A','T','H','\r','\n'};//�����ϴ�ATHָ����йһ�
		CByteArray Array;
		Array.RemoveAll();    
		Array.SetSize(5);		
		
		for (int i=0; i<5; i++)
		{
			Array.SetAt(i,lpOutBuffer[i]);
		}
		if(m_comm_WT.GetPortOpen())
		{
			m_comm_WT.SetOutput(COleVariant(Array));//��������
		}
		if(timer_board_disconnect_times_WT==0)timer_board_disconnect_times_WT++;
		SetTimer(5,2000,NULL);//��ʱ��4�����ּ��ѯ֡�󣬴򿪶�ʱ��5��3�γ�ʱtimer_board_disconnect_times_WTδ�����㣬���ǹ���
		}
	}else if(nIDEvent==5){
		if (timer_board_disconnect_times_WT!=0)
		{
			timer_board_disconnect_times_WT++;
		}
		if (timer_board_disconnect_times_WT>=QUERY_TOLERATE_TIMES)
		{
			state_system[0]=1;//���ߵ绰��ʱ��������ߵ绰����
			m_FKXX+="���ߵ绰���Ӳ�ѯ������\r\n";
			UpdateData(FALSE);
			timer_board_disconnect_times_WT=0;
			m_board_led_WT.SetIcon(m_hIconOff);
			m_wt_r.SetIcon(m_hIconOff);
			GetDlgItem(IDC_STATIC_BOARDCONNECT_WT)->SetWindowText("���ߵ绰���Ӷ�ʧ!");
		}
		KillTimer(5);
	}
	//////////////////////�����������ߵ绰/////////////////////////////////
	else if (nIDEvent==6)//��ά���ּ죬�����ڶ�ʱ��4
	{
		if (index_wakeup_times<200)
		{
			index_wakeup_times++;
			if ((index_wakeup_times==0x0d)||(index_wakeup_times==0x24))
			{
				index_wakeup_times++;
			}
		} 
		else
		{
			index_wakeup_times=0;
		}
		frame_board_check_YW[5]=index_wakeup_times;
		frame_board_check_YW[6]=XOR(frame_board_check_YW,6);
		if ((frame_board_check_YW[6]=='$')||(frame_board_check_YW[6]==0x0d))
		{
			frame_board_check_YW[6]++;//����������$��0x0d����ֵ��һ
		}
		frame_board_check_YW[7]='\r';
		frame_board_check_YW[8]='\n';
		CByteArray Array;
		Array.RemoveAll();
		Array.SetSize(7+2);
		
		for (int i=0;i<(7+2);i++)
		{
			Array.SetAt(i,frame_board_check_YW[i]);
		}
		
		if(m_comm_YW.GetPortOpen())
		{
			m_comm_YW.SetOutput(COleVariant(Array));//��������
		}
		
		if(timer_board_disconnect_times_YW==0)timer_board_disconnect_times_YW++;
		SetTimer(7,1000,NULL);//��ʱ��6�����ּ��ѯ֡�󣬴򿪶�ʱ��7��3�γ�ʱtimer_board_disconnect_times_YWδ�����㣬���ǹ���
		
	}else if(nIDEvent==7){//��ά���ѯ��ͨ������ͳ��.�����ڶ�ʱ��
		if (timer_board_disconnect_times_YW!=0)
		{
			timer_board_disconnect_times_YW++;
		}
		if (timer_board_disconnect_times_YW>=QUERY_TOLERATE_TIMES)
		{
			m_FKXX+="��ά�����Ӳ�ѯ������\r\n";
			UpdateData(FALSE);
			timer_board_disconnect_times_YW=0;
			m_board_led_YW.SetIcon(m_hIconOff);
			GetDlgItem(IDC_STATIC_BOARDCONNECT_YW)->SetWindowText("��ά�����Ӷ�ʧ!");
		}
		KillTimer(7);
	}
	///////////////////////����������ά�嶨�ڲ�ѯ///////////////////////////
	else if(nIDEvent==8){//���������ּ�
		if((msg_manual_select==0)||(m_tel_manual_select.GetCurSel()==2)){
		CByteArray Array;
		Array.RemoveAll();
		Array.SetSize(12);
		for (int i=0; i<12; i++)
		{
			Array.SetAt(i,frame_IC_check[i]);
		}
		
		
		if(m_comm.GetPortOpen())
		{
			m_comm.SetOutput(COleVariant(Array));//��������
		}
		if(timer_board_disconnect_times_BD==0)timer_board_disconnect_times_BD++;
		SetTimer(9,1500,NULL);//��ʱ��8�����ּ��ѯ֡�󣬴򿪶�ʱ��9��3�γ�ʱtimer_board_disconnect_times_BDδ�����㣬���ǹ���
		}
	}else if(nIDEvent==9){
		if (timer_board_disconnect_times_BD!=0)
		{
			timer_board_disconnect_times_BD++;
		}
		if (timer_board_disconnect_times_BD>=QUERY_TOLERATE_TIMES)
		{
			state_system[4]=1;//������ʱ����Ǳ�������
			m_FKXX+="�������Ӳ�ѯ������\r\n";
			UpdateData(FALSE);
			timer_board_disconnect_times_BD=0;
			m_board_led_BD.SetIcon(m_hIconOff);
			m_bd_r.SetIcon(m_hIconOff);
			GetDlgItem(IDC_STATIC_BOARDCONNECT_BD)->SetWindowText("�������Ӷ�ʧ!");
		}
		KillTimer(9);
	}
	//////////////////////�������ڱ���/////////////////////////////////
	else if (nIDEvent == 10)
	{
		m_StatBar->SetText("3G״̬�������ѿ���",2,0); //��������
		if((tel_manual_select==0)||(m_tel_manual_select.GetCurSel()==2)){//�Զ�ģʽ,����ָ����3G�绰ʱ�Ų�ѯ
		SendAtCmd("AT+CSQ",6);      //��ѯCSQ
		if(timer_board_disconnect_times_3G==0)timer_board_disconnect_times_3G++;
		SetTimer(11,1500,NULL);//��ʱ��10�����ּ��ѯ֡�󣬴򿪶�ʱ��11��3�γ�ʱtimer_board_disconnect_times_3Gδ�����㣬���ǹ���
		}
	}else if(nIDEvent==11){
		if (timer_board_disconnect_times_3G!=0)
		{
			timer_board_disconnect_times_3G++;
		}
		if (timer_board_disconnect_times_3G>=QUERY_TOLERATE_TIMES)
		{
			if((tel_manual_select==0)||(m_tel_manual_select.GetCurSel()==2)){//�绰���֣��Զ���ѡ��3Gʱ�����޸�
				state_system[1]=1;//3G��ʱ�����3G����
			}
			if((msg_manual_select==0)||(m_msg_manual_select.GetCurSel()==1)){//���Ų��֣��Զ���ѡ��3Gʱ�����޸�
				state_system[3]=1;
			}
			m_FKXX+="3G�źŲ�ѯ������\r\n";
			UpdateData(FALSE);
			timer_board_disconnect_times_3G=0;
			m_board_led_3G.SetIcon(m_hIconOff);
			m_3g_m_r.SetIcon(m_hIconOff);
			m_3g_p_r.SetIcon(m_hIconOff);
			GetDlgItem(IDC_STATIC_BOARDCONNECT_3G)->SetWindowText("3G���Ӷ�ʧ!");
		}
		KillTimer(11);
	}
	else if(nIDEvent==12){//������Ϣ�ı���ʱ���
		CEdit* pEdit = ((CEdit*)GetDlgItem(IDC_EDIT_FKXX));
		int len = pEdit->GetLineCount(); 
		if (len >= 18) //�Զ���ȡ�ı���CEdit������Ϊ64K�ֽ� 
		{ 
			OnButtonClearmsg();
		}
	}else if (nIDEvent==13) {//���ߵ绰�Զ�Ӧ�𣬲�����ʱ
		KillTimer(13);
		if((flag_PW_in_busy==1)||(flag_PW_out_busy==1)){
			WT_state=3;
			OnButtonCall();//�Է����������ʱ���һ�
		}
	}
	else if (nIDEvent==14) {//���ߵ绰����·����㲥��ʱ
		KillTimer(14);
		m_StatBar->SetText("��ά��״̬������·�㲥���볬ʱ",4,0); 
		SetTimer(13,4000,NULL);//���Ų�����ʱ��
		PlaySound(".//Wav//PrepareFailed.wav", NULL, SND_FILENAME|SND_ASYNC);//�㲥׼��ʧ�ܣ����Ժ����� (4s)
	}
	else if (nIDEvent==15) {//���ߵ绰ѭ������������
		sound_switch(8);//�л�����������ģʽ
		PlaySound(".//Wav//ring.wav", NULL, SND_FILENAME|SND_ASYNC);
	}

	else if (nIDEvent==16) {//3G�绰�Զ�Ӧ�𣬲�����ʱ
		KillTimer(16);
		if((flag_3G_in_busy==1)||(flag_3G_out_busy==1)){ 
			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
			OnButtonCall();//�Է����������ʱ���һ�
		}
	}
	else if (nIDEvent==17) {//3G�绰����·����㲥��ʱ
		KillTimer(17);
		m_StatBar->SetText("��ά��״̬������·�㲥���볬ʱ",4,0); 
		SetTimer(16,4000,NULL);//���Ų�����ʱ��
		PlaySound(".//Wav//PrepareFailed.wav", NULL, SND_FILENAME|SND_ASYNC);//�㲥׼��ʧ�ܣ����Ժ����� (4s)
	}
	else if (nIDEvent==18) {//���ߵ绰ѭ������������
		sound_switch(9);//�л�����������ģʽ
		PlaySound(".//Wav//ring.wav", NULL, SND_FILENAME|SND_ASYNC);
	}
	else if (nIDEvent==19) {
		if(broad_prepare==2)
		{
			KillTimer(19);
		}
		if((flag_3G_in_busy==0)&&(flag_3G_out_busy==0)&&(flag_PW_in_busy==0)&&(flag_PW_out_busy==0)&&(flag_ST_in_busy==0)&&(flag_ST_out_busy==0))
		applay_broadcast(2);
	}
	//////////////////////��������3G/////////////////////////////////
	else if (nIDEvent == 20)
	{
		m_StatBar->SetText("���ǵ绰״̬�������ѿ���",1,0); //��������
		if((tel_manual_select==0)||(m_tel_manual_select.GetCurSel()==3)){//�Զ�ģʽ,����ָ�������ǵ绰ʱ�Ų�ѯ
			if ((SerialPortOpenCloseFlag_ST==TRUE)&&(bISCallProcess_WX==FALSE))//ֻ���ڴ����Ѿ��򿪲��Ҳ�����ͨ����ʱ�Ų�ѯ
			{
				Send_AT_Command_ST("AT+CSQ");//��ѯ�ź�����
				SetTimer(22,1000,NULL);
			}      
			if(timer_board_disconnect_times_ST==0)timer_board_disconnect_times_ST++;
			SetTimer(21,2000,NULL);//��ʱ��20�����ּ��ѯ֡�󣬴򿪶�ʱ��21��3�γ�ʱtimer_board_disconnect_times_STδ�����㣬���ǹ���
		}
	}else if(nIDEvent==21){
		if (timer_board_disconnect_times_ST!=0)
		{
			timer_board_disconnect_times_ST++;
		}
		if (timer_board_disconnect_times_ST>=QUERY_TOLERATE_TIMES)
		{
			if((tel_manual_select==0)||(m_tel_manual_select.GetCurSel()==3)){
				state_system[2]=1;//���ǵ绰��ʱ��������ǵ绰����
			}

			m_FKXX+="���ǵ绰��λ�źŲ�ѯ������\r\n";
			UpdateData(FALSE);
			timer_board_disconnect_times_ST=0;
			m_board_led_ST.SetIcon(m_hIconOff);
			m_st_r.SetIcon(m_hIconOff);
			GetDlgItem(IDC_STATIC_BOARDCONNECT_ST)->SetWindowText("���ǵ绰���Ӷ�ʧ!");
		}
		KillTimer(21);
	}else if(nIDEvent==22)//��ѯRINFO
	{
		KillTimer(22);
		Send_AT_Command_ST("AT+RINFO");//������Ϣ	
	}else if(nIDEvent==23){//��ʱ��ѯTTS�Ƿ������
		if (bIsTTSPlay==0)
		{
			applay_broadcast(2);//��������㲥
			KillTimer(23);
		}
	}
	//////////////////////////�����������ǵ绰////////////////////////////////
	//-----------------3G�Զ�Ӧ��5 S----------------------
	else if (nIDEvent==31)  
	{
		KillTimer(31);
		this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
		UpdateData(FALSE);
		m_StatBar->SetText("��ά��״̬���Զ�Ӧ�����볬ʱ",4,0); 
		SetTimer(16,3000,NULL);//���Ų�����ʱ��
		PlaySound(".//Wav//TimeOut.wav", NULL, SND_FILENAME|SND_ASYNC);//�Բ������Ĳ����ѳ�ʱ   (3s)
		
	}else if (nIDEvent==32)
	{
		KillTimer(32);
		this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
		UpdateData(FALSE);
		m_StatBar->SetText("��ά��״̬���������˽�����ʱ",4,0);
		SetTimer(16,4000,NULL);//���Ų�����ʱ��
		PlaySound(".//Wav//NoResponse.wav", NULL, SND_FILENAME|SND_ASYNC);//�Բ�������Ӧ�����Ժ�����		
	}
	//-----------------3G�Զ�Ӧ��5 E----------------------
	///////////////////////��������3G�Զ�Ӧ��/////////////////////////////////
	//-----------------���ߵ绰�Զ�Ӧ��5 S----------------------
	else if (nIDEvent==33)  
	{
		KillTimer(33);
		this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
		UpdateData(FALSE);
		m_StatBar->SetText("��ά��״̬���Զ�Ӧ�����볬ʱ",4,0); 
		SetTimer(13,3000,NULL);//���Ų�����ʱ��
		PlaySound(".//Wav//TimeOut.wav", NULL, SND_FILENAME|SND_ASYNC);//�Բ������Ĳ����ѳ�ʱ   (3s)
	}
	else if (nIDEvent==34)
	{
		KillTimer(34);
		this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
		UpdateData(FALSE);
		m_StatBar->SetText("��ά��״̬���������˽�����ʱ",4,0);
		SetTimer(13,4000,NULL);//���Ų�����ʱ��
		PlaySound(".//Wav//NoResponse.wav", NULL, SND_FILENAME|SND_ASYNC);//�Բ�������Ӧ�����Ժ�����		
	}
	//-----------------���ߵ绰�Զ�Ӧ��5 E----------------------
	///////////////////////���������Զ�Ӧ��/////////////////////////////////
	else if (nIDEvent==40)  
	{
		if(bINIFail == TRUE) //������ʼ��
		{
				CString strTemp;
				switch (nATCmdID)
				{
				case 1:			
					KillTimer(40);
					SendAtCmd("AT+CMEE=1",1);//���ش������     ok or error
					nATCmdID = 3;
					SetTimer(40,1000,NULL);
					break;
				case 3:			
					KillTimer(40);
					SendAtCmd("AT+CLIP=1",3);//������ʾ  
					nATCmdID = 5;
					SetTimer(40,1000,NULL);
					break;	
				case 5:
					KillTimer(40);
					SendAtCmd("AT+CLVL=4",5);      //������������Ϊ4
					nATCmdID = 6;
					SetTimer(40,1000,NULL);
					break;
				case 6:
					KillTimer(40);
					SendAtCmd("AT+CSQ",6);      //��ѯCSQ
					nATCmdID = 25;
					SetTimer(40,1000,NULL);
					break;
		/*		case 23:       //���Ͷ���
					KillTimer(40);
					strTemp = chPDU;
					SendAtCmd(strTemp,23);
					nATCmdID = 0;
		//			SetTimer(40,1000,NULL);
					break;*/
				case 25:			
					KillTimer(40);
					SendAtCmd("AT+CNMI=2,1,0,0,0",25);      //�¶���֪ͨ
					nATCmdID = 26;
					SetTimer(40,1000,NULL);
					break;
				case 26:			
					KillTimer(40);
					SendAtCmd("AT+CPMS=\"ME\",\"ME\",\"ME\"",25);      //�¶���֪ͨ
					nATCmdID = 40;
					SetTimer(40,2000,NULL);
					break;
				case 40:
					KillTimer(40);
					if(SerialPortOpenCloseFlag_3G==TRUE)m_ctrlIconOpenoff_3G.SetIcon(m_hIconRed);//����رմ������Ʋ�ͬ��
					SetTimer(10,(QUERY_INTERVAL+QUERY_3G),NULL);//��ѯ��CSQ�����ƶ�����ʼ�����һ����ʱ����
					SetTimer(42,2000,NULL);//���Ų�ѯ �ڶ�ʱ��42���棬2s������
					//����ʼ�����̹ر���ok����
					nATCmdID = 41; //����Ϊ41,ֻ���������б��ѯ���ж�
					break;
				default:
					nATCmdID = 0;
					break;
				}
		}
		else   //���¿�ʼ���г�ʼ��
		{
			KillTimer(40);
			SetTimer(45,10000,NULL);//���ó�ʼ������
		}
	}
	else if (nIDEvent == 41)
	{
		KillTimer(41);
		if (bRoughLongSMS)
		{
			SendAtCmd("AT+CMGD="+strSMSIndex+","+"0",31);     //ɾ�����š�
			strShortSMSIndex = "";
		} 
		else
		{
			SendAtCmd("AT+CMGD="+strShortSMSIndex+","+"0",31);     //ɾ�����š�	
		}		
		SetTimer(43,500,NULL);
		strSMSIndex = "";
	}
	else if (nIDEvent == 42)
	{
		KillTimer(42);
		if (bIsINIProcess == FALSE)
		{
			OnClearAll();
		}
		nSMSCount=0;
		for (int n=0;n<SMSSIZE_10;n++)
		{
				SMSIndex[n] = "";
		}
		strSMSIndex = "";
		SendAtCmd("AT+CMGL=4",40);      //��ѯ����
		bRoughLongSMS = FALSE;
	}
	else if (nIDEvent == 43)
	{
		KillTimer(43);		
		if (nSMSCount!=0)
		{
			for (int n=0;n<SMSSIZE_10;n++)
			{
				if (SMSIndex[n] != "")
				{
					strSMSIndex = SMSIndex[n];
					SMSIndex[n] = "";
					break;
				}
			}			
			SendAtCmd("AT+CMGR="+strSMSIndex,32);//��ȡָ������   AT+CMGR 32
			nSMSCount--;
			//	SetTimer(42,1000,NULL);	
		//	SetTimer(41,2000,NULL);
		}
		else
		{
			SetTimer(10,(QUERY_INTERVAL+QUERY_3G),NULL);
			strSMSIndex = "";
			SetTimer(42,2000,NULL);
		}				
	}
	else if (nIDEvent == 45)
	{
		KillTimer(45);
		OnClearAll();
		bIsINIProcess = TRUE;
		bINIFail = TRUE;
		nATCmdID = 1;
		SetTimer(40,1000,NULL);	
	}
	else if(nIDEvent == 46)
	{
		CString strTempa = "";
		KillTimer(46);
		strTempa = chPDU;
		SendAtCmd(strTempa,23);
		nATCmdID = 0;
	}
	else if(nIDEvent == 47)
	{
		KillTimer(47);
		if (nLongSMSSendCount != 0)
		{
			memset(chPDU,0,512);
			SendLongSMS(&longSMSStruct);
		}		
	}
	else if(nIDEvent == 48)
	{
		bTimer48OnOff = FALSE;
		KillTimer(48);
		SetTimer(10,(QUERY_INTERVAL+QUERY_3G),NULL);	
	}
	else if(nIDEvent == 49)
	{
			bIsINIProcess = TRUE;
			bINIFail = TRUE;
			HWND hWnd = ::FindWindow(NULL, _T("record11"));//�ر�¼��
			if (NULL != hWnd) {
				::SendMessage(hWnd, WM_CLOSE, 0, 0);
							}
			SendAtCmd("ATE0",2);   //�������� 0��MS �����ʹ�TE ���յ��ַ�  1��MS ���ʹ�TE ���յ��ַ���			
	}
	////////////////////���¶�ʱ���������ǵ绰/////////////////////////////////////////
	else if (nIDEvent==50)
	{
		if(BINIfail_WX == TRUE) //������ʼ��
		{
					switch(nATCmdFlags)
					{
						case 1:
							KillTimer(50);
							Send_AT_Command_ST("ATE0");  //�ػ���
							nATCmdFlags = 2;
							SetTimer(50,1000,NULL);
							break;
						case 2:
							KillTimer(50);
							Send_AT_Command_ST("AT+CLIP=1");  //��������ʾ
							nATCmdFlags = 3;
							SetTimer(50,1000,NULL);
							break;
						case 3:
							KillTimer(50);
							Send_AT_Command_ST("AT+GPSSERVICE=1");//��GPS				
							nATCmdFlags = 4;
							SetTimer(50,1000,NULL);
							break;
						case 4:
							KillTimer(50);
							Send_AT_Command_ST("AT+CLVL=4");  //����loudspeaker����Ϊ4�����
							nATCmdFlags = 5;
							SetTimer(50,1000,NULL);
							break;
						case 5:
							KillTimer(50);
							Send_AT_Command_ST("AT+CMEE=1");  //���󷵻����ã���Ϊ���ش������
							nATCmdFlags = 6;
							SetTimer(50,1000,NULL);
							break;
						case 6:
							KillTimer(50);
							Send_AT_Command_ST("AT+CSQ");//��ѯ�ź�����
							nATCmdFlags = 7;
							SetTimer(50,1000,NULL);
							break;
						case 7:
							KillTimer(50);
							Send_AT_Command_ST("AT+CREG=1");  //��ѯע��״̬�����ã�����״̬����
							nATCmdFlags = 8;
							SetTimer(50,1000,NULL);
							break;
						case 8:
							KillTimer(50);
							Send_AT_Command_ST("AT+GPSTRACK=1");//GPS׷��				
							nATCmdFlags = 9;
							SetTimer(50,1000,NULL);
							break;
						case 9:
							KillTimer(50);
							Send_AT_Command_ST("AT+RINFO");//������Ϣ
							SetTimer(20,(QUERY_INTERVAL+QUERY_ST),NULL);//��ʼ���ڼ��CSQ��RINFO
							nATCmdFlags = 10;
			//				SetTimer(50,1000,NULL);
							break;
						case 100:
							nATCmdFlags = 1;
							SetTimer(50,1000,NULL);
							break;
						default :			
							break;
					}
		}
		else
		{
			KillTimer(50);
			SetTimer(55,10000,NULL);
		}
	} 
	else if (nIDEvent == 55)//��ʼ������error�����³�ʼ��
	{
		KillTimer(55);
		bISCallProcess_WX = TRUE;
		BINIfail_WX = TRUE;
		nATCmdFlags = 100;
		SetTimer(50,1000,NULL);	
	}
	else if (nIDEvent == 59)//��ʼ��δӦ�����³�ʼ��
	{
		BISInitial_WX = TRUE;
		BINIfail_WX = TRUE;
		HWND hWnd = ::FindWindow(NULL, _T("record11"));//�ر�¼��
		if (NULL != hWnd) {
			::SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
		nATCmdFlags = 100;
		Send_AT_Command_ST("AT");
	}



	CDialog::OnTimer(nIDEvent);
}

void CBeidouDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void CBeidouDlg::OnButtonSet() 
{
	// TODO: Add your control notification handler code here
	CString str;
	//��ð�ť�ı�
	GetDlgItemText(IDC_BUTTON_SET,str);
	if(str=="���")
	{
		//���ð�ť�ı�
		SetDlgItemText(IDC_BUTTON_SET,"����");
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_SET,"���");
	}
	if(str=="���")
	{
		//��ʾ"�򻯰�"�Ի���
		if (switch_state==0)//��绰
		{
			SetWindowPos(NULL,0,0,rectSmall.Width(),rectSmall.Height(),SWP_NOMOVE|SWP_NOZORDER);
		}else if(switch_state==1){//������
			SetWindowPos(NULL,0,0,rectMiddle.Width(),rectMiddle.Height(),SWP_NOMOVE|SWP_NOZORDER);
		}
		
	}
	else
	{
		SetWindowPos(NULL,0,0,rectLarge.Width(),rectLarge.Height(),SWP_NOMOVE|SWP_NOZORDER);
	}

}

void CBeidouDlg::OnButtonPhone() 
{
	// TODO: Add your control notification handler code here
	switch_state=0;//��绰
 	SetWindowPos(NULL,0,0,rectSmall.Width(),rectSmall.Height(),SWP_NOMOVE|SWP_NOZORDER);
 	SetDlgItemText(IDC_BUTTON_SET,"����");
}

void CBeidouDlg::OnButtonMessage() 
{
	// TODO: Add your control notification handler code here
	switch_state=1;//������
	SetWindowPos(NULL,0,0,rectMiddle.Width(),rectMiddle.Height(),SWP_NOMOVE|SWP_NOZORDER);
	SetDlgItemText(IDC_BUTTON_SET,"����");
}
 
void CBeidouDlg::OnComm_WT() 
{
	// TODO: Add your control notification handler code here
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len,k;
	BYTE rxdata[2048]; //����BYTE����
	CString strDisp="",strTmp="";
	int frequency_point=0;//Ƶ��ɨ����ܵ�Ƶ����
	double frequency_buf=0;//Ƶ�����
	int frame_index_WT;//֡������	
	
	if((m_comm_WT.GetCommEvent()==2)) //�¼�ֵΪ2��ʾ���ջ����������ַ�
	{
		Sleep(160);
		variant_inp=m_comm_WT.GetInput(); //��������
		safearray_inp=variant_inp;  //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���
		len=safearray_inp.GetOneDimSize(); //�õ���Ч���ݳ���
		for(k=0;k<len;k++)
		{
			safearray_inp.GetElement(&k,rxdata+k);//ת��ΪBYTE������
		}
		frame_index_WT=0;
		for(k=0;k<len;k++)//������ת��ΪCString����
		{
			BYTE bt=*(char*)(rxdata+k);    //�ַ���
			frame_receive_WT[frame_index_WT]=bt;
			frame_index_WT++;			
		}
	
		
	if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='N'))//����Ƿ�ΪDTMF�ź�
	{
		if ((bIsAutoResponseProcess == FALSE)&&(flag_PW_in_busy==1))
		{
			CString tmp;
			tmp.Format("%c",frame_receive_WT[3]);
				send_string+=tmp;
			if(send_string.Find("#")!=-1)
			{
				tmp =  send_string.Mid(0,1);
				send_string = tmp;
				send_string+='#';//����#��
				m_FKXX+="���ߵ绰����:";
				m_FKXX+=send_string;
				m_FKXX+="\r\n";
				UpdateData(FALSE);
				KillTimer(33);//ͣ��ringָ���д򿪵Ķ�ʱ��33
				nCountCheck++;
				if (send_string == "1#")//����·����㲥
				{
					SetTimer(14,20000,NULL);//��ʱʱ�����Ź㲥׼��ʧ����
					applay_broadcast(1);//���뿪ʼ�㲥
					m_StatBar->SetText("��ά��״̬������·�㲥�������·���δӦ��",4,0); 
					PlaySound(".//Wav//BroadPrepare.wav", NULL, SND_FILENAME|SND_ASYNC);//�㲥����׼���У����Ժ�   (3s)
					bIsAutoResponseProcess = TRUE;
				}     
				else if (send_string == "2#")//����ͨ��
				{
					SetTimer(34,NOPICKUP_TIME ,NULL);//�Զ�������ȴ�
					PlaySound(".//Wav//RingConnect.wav", NULL, SND_FILENAME|SND_ASYNC);//ͨ�����������У����Ժ�   (4s)   //�������Ūһ�α�������(���ظ�����)
					bIsAutoResponseProcess = TRUE;
					//���̨��Ա֪ͨ����,���ز���������   
					SetTimer(15,4000,NULL);//ÿ��4s����һ��ring������
					WT_state=4;//�ȴ�����ժ��
					GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�����绰");
					GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
				}
				else
				{
					if (nCountCheck < 3)
					{
						PlaySound(".//Wav//ErrorCmd.wav", NULL, SND_FILENAME|SND_ASYNC);//ָ���������������
						SetTimer(33,AUTORESPONSE_TIME,NULL);//15s�ȴ�ʱ�䣨�ȴ��û����룬ʱ������˲���������ʱ�䣩
					} 
					else
					{
						WT_state=3;
						GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
						if((flag_PW_in_busy==1)||(flag_PW_out_busy==1))OnButtonCall();
					}
					
				}
				send_string="";
				UpdateData(FALSE);
			}	
		}
		
	}
	else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='C')&&(frame_receive_WT[3]=='I')&&(frame_receive_WT[4]=='D'))//����Ƿ�Ϊ������ʾ
	{
		strDisp=frame_receive_WT+5;
		call_in_number=strDisp;//��ʼƴ���������

	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='Z'))//����Ƿ�ժ���ɹ�
	{
		if (flag_PW_out_busy == 1)
		{
			ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);	//��������������¼��
		}		
		m_FKXX+="ժ���ɹ�";
		m_FKXX+="\r\n";
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='D'))//����Ƿ񲦺ųɹ�
	{
		m_FKXX+="���ųɹ�";
		m_FKXX+="\r\n";
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='H'))//����Ƿ�һ��ɹ�
	{
		m_board_led_WT.SetIcon(m_hIconRed);
		m_wt_r.SetIcon(m_hIconRed);
		GetDlgItem(IDC_STATIC_BOARDCONNECT_WT)->SetWindowText(" ���ߵ绰�����ӣ�"); 
		timer_board_disconnect_times_WT=0;//��ѯ����������
		state_system[0]=0;//���ߵ绰Ӧ�𣺱�����ߵ绰����
		if((flag_PW_in_busy==1)||(flag_PW_out_busy==1))	m_FKXX+="�һ��ɹ�\r\n";
		else m_FKXX+="���ߵ绰���Ӳ�ѯ������\r\n";
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='B'))//����Ƿ񴫺ųɹ�
	{
		m_FKXX+="���ųɹ�";
		m_FKXX+="\r\n";
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='S')&&(frame_receive_WT[3]=='2'))//����Ƿ�Ϊ�һ���ʾ
	{
		applay_broadcast(2);//������ֹ�㲥
		SetTimer(19,3000,NULL);//ȫ��������·����ʱ�����ڽ����㲥
		KillTimer(34);//�Է��һ����ر�ժ����ʱ��ʱ��
		KillTimer(15);//�Է��һ���رն���������
		KillTimer(14);
		m_StatBar->SetText("��ά��״̬������·�㲥��ֹ���·���δӦ��",4,0); 
		m_FKXX+="�Է��һ�";
		m_FKXX+="\r\n";
		if((flag_PW_in_busy==1)||(flag_PW_out_busy==1)){
			WT_state=3;
			OnButtonCall();//�Է��һ�����Ҳ�һ����绰״̬����Ϊ���С�������ֶԷ��ȹһ�����֮�������һ����˴������ز�������
		}
		char lpOutBuffer[] = {'A','T','H','\r','\n'};//�����ϴ�ATHָ����йһ�
		CByteArray Array;
		Array.RemoveAll();
		Array.SetSize(5);		
		
		for (int i=0; i<5; i++)
		{
			Array.SetAt(i,lpOutBuffer[i]);
		}
		if(m_comm_WT.GetPortOpen())
		{
			m_comm_WT.SetOutput(COleVariant(Array));//��������
		}
		GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("����绰");
		sound_switch(4);//������ΪĬ��
		SetTimer(4,(QUERY_INTERVAL+QUERY_WT),NULL);//�����һ��ͱ����һ����򿪶�ʱ��//�˴���ɾ��
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='R')&&(frame_receive_WT[3]=='i')&&(frame_receive_WT[4]=='n')&&(frame_receive_WT[5]=='g'))//����Ƿ�Ϊ������ʾ��
	{
		if (state_system[0]==1)
		{
			return;
		}
		KillTimer(4);
		m_FKXX+="Ring!  ";
		flag_PW_in_busy=1;
		send_string="";//�����ַ�����գ�׼�����մ���
		//----------�Զ�Ӧ��2 S-----------
		OnButtonCall();//ժ�����Զ����ŵ�һ������
		sound_switch(5);//�л������ߵ绰����ģʽ�������Զ�Ӧ��
		SetTimer(33,AUTORESPONSE_TIME,NULL);//20s�ȴ�ʱ�䣨�ȴ��û����룬ʱ������˲���������ʱ�䣩
		PlaySound(".//Wav//Guide.wav", NULL, SND_FILENAME|SND_ASYNC);//Ӧ����̨ϵͳ���Զ��㲥�밴1��ԭʼͨ���밴2,#�Ž���
		//----------�Զ�Ӧ��2 E-----------
		
	}else if (rxdata[0] >='0' && rxdata[0]<='9')//����Ƿ��ǵ绰����
	{
		strDisp=frame_receive_WT;
		call_in_number+=strDisp;
		m_FKXX+="\r\n�Է�������:";
		m_FKXX+=call_in_number;
	} 
	else
	{
	//	AfxMessageBox("��λ��֡�д���",MB_OK,0);
		m_FKXX+=frame_receive_WT;
		m_FKXX+="�绰��֡�д��󣬼�����Ƿ�����\r\n";
	}
		UpdateData(FALSE);
	}
}

void CBeidouDlg::OnOpencloseportWT() 
{
	// TODO: Add your control notification handler code here
	CString string1="1200,n,8,1";

	if(SerialPortOpenCloseFlag_WT==FALSE)
	{
		SerialPortOpenCloseFlag_WT=TRUE;

		//�����Ǵ��ڵĳ�ʼ������
		if(m_comm_WT.GetPortOpen())//�򿪶˿�ǰ�ļ�⣬�ȹأ��ٿ�
			MessageBox("�����޷���");
//			m_comm.SetPortOpen(FALSE);	//	
		m_comm_WT.SetCommPort(m_DCom_WT); //ѡ��˿ڣ�Ĭ����com2
		m_comm_WT.SetSettings((LPSTR)(LPCTSTR)string1); //������1200����У�飬8������λ��1��ֹͣλ
		if(!m_comm_WT.GetPortOpen())
		{			
			m_comm_WT.SetPortOpen(TRUE);//�򿪴���
			GetDlgItem(IDC_OPENCLOSEPORT_WT)->SetWindowText("�رմ���");
			m_StatBar->SetText("���ߵ绰������������",0,0);
			m_openoff_WT.SetIcon(m_hIconRed);
			
			Sleep(70);//�ȴ������ȶ����ٷ������ߵ绰�Ĳ�ѯ��Ϣ
			char lpOutBuffer[] = {'A','T','H','\r','\n'};//�����ϴ�ATHָ����йһ�
			CByteArray Array;
			Array.RemoveAll();    
			Array.SetSize(5);		
			
			for (int i=0; i<5; i++)
			{
				Array.SetAt(i,lpOutBuffer[i]);
			}
			if(m_comm_WT.GetPortOpen())
			{
				m_comm_WT.SetOutput(COleVariant(Array));//��������
			}

//			GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
			SetTimer(4,(QUERY_INTERVAL+QUERY_WT),NULL);//�Զ�ģʽ�²Ŵ򿪶�ʱ��
		}
		else
			MessageBox("�����޷���.");	 
	}
	else
	{
		SerialPortOpenCloseFlag_WT=FALSE;
		GetDlgItem(IDC_OPENCLOSEPORT_WT)->SetWindowText("�򿪴���");
		m_openoff_WT.SetIcon(m_hIconOff);
		m_comm_WT.SetPortOpen(FALSE);//�رմ���
		state_system[0]=1;//���ߵ绰���ڹر�
		m_board_led_WT.SetIcon(m_hIconOff);
		m_wt_r.SetIcon(m_hIconOff);
		GetDlgItem(IDC_STATIC_BOARDCONNECT_WT)->SetWindowText(" ���ߵ绰�ѶϿ���"); 
//		GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(FALSE);
		m_StatBar->SetText("���ߵ绰�������ѶϿ�",0,0);
		KillTimer(4);
	}
}

void CBeidouDlg::OnSelendokComboComselectWT() 
{
	// TODO: Add your control notification handler code here
	m_DCom_WT=m_com_WT.GetCurSel()+1;
	UpdateData();	

	CString strTemp;
	strTemp.Format(_T("%d"),m_DCom_WT-1);
	::WritePrivateProfileString("ConfigInfo","com_WT",strTemp,".\\config_phonemessage.ini");
	
	strTemp.Format(_T("%d"),m_DCom_WT);
	::WritePrivateProfileString("ConfigInfo","com_r_WT",strTemp,".\\config_phonemessage.ini");
}

void CBeidouDlg::OnButtonClearNum() 
{
	// TODO: Add your control notification handler code here
	m_target_number="0";
	UpdateData(FALSE);
	
 	if((flag_PW_in_busy==1)||(flag_PW_out_busy==1)||(flag_3G_out_busy==1)||(flag_3G_out_busy==1)){//ͨ���У���ťʼ��ʹ��
 		GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
 	}else{//������
 		GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(FALSE);
 	}	
	GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(FALSE);
}

void CBeidouDlg::OnButtonCall() 
{
	// TODO: Add your control notification handler code here
	if((state_system[0]==0)&&(flag_3G_in_busy==0)&&(flag_ST_in_busy==0)){//����绰ʱ�����ߵ绰���� ����3Gû�д������ǵ绰û�д��룬�������ߵ绰
		if((flag_3G_out_busy==0)&&(flag_3G_in_busy==0)&&(flag_PW_out_busy==0)&&(flag_PW_in_busy==0)&&(m_target_number=="0")){//ֻ�в���绰ʱ(ȫ������)���ż���Ƿ��к�������
			AfxMessageBox("��������Է�����!");
			return;
	}
		
		if(WT_state==0){//ժ��			
			KillTimer(4);
			char lpOutBuffer[] = {'A','T','Z','\r','\n'};//�����ϴ�ATHָ����йһ�
			CByteArray Array;
			Array.RemoveAll();
			Array.SetSize(5);		
			
			for (int i=0; i<5; i++)
			{
				Array.SetAt(i,lpOutBuffer[i]);
			}
			if(m_comm_WT.GetPortOpen())
			{
				m_comm_WT.SetOutput(COleVariant(Array));//��������
			}

			if (flag_PW_in_busy==1)//���У������绰
			{
				WT_state=3;//�ùһ���
				GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
			} 
			else//�����绰
			{
				sound_switch(1);//���ߵ绰����������
				flag_PW_out_busy=1;//��������绰
				GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("���ڲ���...");
				GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(FALSE);//����绰ʱ���ر��˸�ť
				SetTimer(3,100,NULL);//100ms���ٵ��ñ��������в��ţ�ʹժ���������û�͸�����Ȳ���һ��ժ���ٵ���һ�Σ����ڲ���
				WT_state=2;//�ò�����
			}			
		}
		else if(WT_state==2)//����
		{
			CByteArray Array;
			Array.RemoveAll();
			int len=m_target_number.GetLength();
			Array.SetSize(len+5);
			char *lpOutBuffer = (char *)malloc((len+5)*sizeof(char));
			if(lpOutBuffer == NULL)
				MessageBox("Allocated failed!");
			
			memcpy(lpOutBuffer+3,m_target_number,len);
			lpOutBuffer[0] = 'A';
			lpOutBuffer[1] = 'T';
			lpOutBuffer[2] = 'D';
			lpOutBuffer[len+3] = '\r';
			lpOutBuffer[len+4] = '\n';
			
			for (int i=0; i<(len+5); i++)
			{
				Array.SetAt(i,lpOutBuffer[i]);
			}
			if(m_comm_WT.GetPortOpen())
			{
				m_comm_WT.SetOutput(COleVariant(Array));//��������
			}
			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
			GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(TRUE);//ʹ���˸��
			WT_state=3;//��һ���ùһ���
		}
		else if (WT_state==3)//�һ�
		{			
			char lpOutBuffer[] = {'A','T','H','\r','\n'};//�����ϴ�ATHָ����йһ�
			CByteArray Array;
			Array.RemoveAll();    
			Array.SetSize(5);		
			
			for (int i=0; i<5; i++)
			{
				Array.SetAt(i,lpOutBuffer[i]);
			}
			if(m_comm_WT.GetPortOpen())
			{
				m_comm_WT.SetOutput(COleVariant(Array));//��������
			}

			//�ر�¼��
			HWND hWnd = ::FindWindow(NULL, _T("record11"));
			if (NULL != hWnd) {
				::SendMessage(hWnd, WM_CLOSE, 0, 0);
			}//��¼�����

			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("����绰");
			WT_state=0;//�һ��������С���һ��ժ��
			flag_PW_out_busy=0;//�����һ�������
			flag_PW_in_busy=0;//�����һ�������
			bIsAutoResponseProcess = FALSE;
			nCountCheck = 0;
			SetTimer(4,(QUERY_INTERVAL+QUERY_WT),NULL);//�����һ��ͱ����һ����򿪶�ʱ��
			sound_switch(4);//������ΪĬ��
			applay_broadcast(2);//������ֹ�㲥
			SetTimer(19,3000,NULL);//ȫ��������·����ʱ�����ڽ����㲥
			KillTimer(15);//��ʱ�һ���رն���������
			m_StatBar->SetText("��ά��״̬������·�㲥��ֹ���·���δӦ��",4,0); 
		} 
		else if(WT_state==4)//����ժ��
		{
			KillTimer(34);//�ر�ժ����ʱ��ʱ��
			KillTimer(15);//ժ����رն���������
			sound_switch(1);//���ߵ绰����ժ������Ϊ����ģʽ
			ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);	//���У�����ժ��������¼��
			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
			WT_state=3;//ͨ����,��һ�����ǹһ���
		}//end of WT_state		
	}
	/*************************************3G********************************************/
	else if ((state_system[1]==0)&&(flag_ST_in_busy==0))//����绰ʱ�����ߵ绰������ʱ������3G�绰
	{
		UpdateData(TRUE);
		CString strToSend,str;
		this->GetDlgItemText(IDC_BUTTON_CALL,str);
		if (str == "����绰")
		{
			nCallFlags = 1;
		} 
		else if(str == "�һ�")
		{
			//�ر�¼��
			HWND hWnd = ::FindWindow(NULL, _T("record11"));
			if (NULL != hWnd) {
				::SendMessage(hWnd, WM_CLOSE, 0, 0);
			}//��¼�����
			nCallFlags = 3;
		}
		else if(str == "�����绰")
		{
			nCallFlags = 2;	
		}
		else if(str == "����ժ��")
		{
			nCallFlags = 4;	
		}
		int i;
		CString strTemp;
		int len = m_target_number.GetLength();	
		switch (nCallFlags)   //�ж�
		{
			case 0://�ݲ���ʲô
				break;
			case 1://���У�����绰
				char *chNum;
				chNum = (LPSTR)(LPCTSTR)m_target_number;
				if (len>=11) //�̻���������Ҳ�п�����11λ
				{
					i = checkPhoneNum(chNum,len);
					if (i==1)
					{
						strTemp =m_target_number;
					}else if (i==2)
					{
						strTemp ="+" + m_target_number;
					} 
					else if (i==3)
					{
						strTemp ="+86" + m_target_number;
					}
					else if(i==0)
					{
						AfxMessageBox("���볤�ȳ�������11λ���벻���ֻ����롣");
						return;
					}
				} 
				else{
					strTemp =m_target_number;   //ֱ�Ӳ���ȥ
				}

				bIsCaller = TRUE;//���б�־λ
				strToSend = "ATD"+ strTemp;
				strToSend += "I";//����CLIR
				strToSend += ";";
				sound_switch(2);//�����л���3G��������ģʽ
				SendAtCmd(strToSend,11);
				flag_3G_out_busy = 1;
				this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");			
				break;
			case 2://���У��Զ�Ӧ��
				//���Զ�ժ����֤�ɹ����ı���Ϊ     �����绰 ����Ҫ���Ҷϵ绰���Զ��������棬 
				SendAtCmd("ATA",12);//ժ��
				break;
			case 3://�һ�
				applay_broadcast(2);//������ֹ�㲥
				SetTimer(19,3000,NULL);//ȫ��������·����ʱ�����ڽ����㲥
				strToSend = "AT+CHUP";
				SendAtCmd(strToSend,13);//����Ƚ����⣬�ҶϵĻظ���TRUEorFALSE���п��ܣ���������ΪFALSE������
				flag_3G_out_busy = 0;
				flag_3G_in_busy = 0;
				bRadioIsReady = FALSE;
				nCountCheck = 0;
				SetTimer(10,(QUERY_INTERVAL+QUERY_3G),NULL);
				this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("����绰");
				m_FKXX += strToSend;
				UpdateData(FALSE);
				OnClearAll();
				nCallFlags = 0;
				bIsAutoResponseProcess = FALSE;
				break;
			case 4://����ժ��
				KillTimer(32);
				KillTimer(18);
				sound_switch(2);
				//����֪ͨ����   //��mic    //��ʽ��ʼ��ͨ�绰ͨ��
				bIsAutoResponseProcess = TRUE;
				ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);	//3G����¼��
				bIsRecord = TRUE;
				GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
				break;
			default:
				break;
		}
	}
	/*************************************���ǵ绰********************************************/
	else if (state_system[2]==0)//����绰ʱ�����ߵ绰��3G������ʱ���������ǵ绰
	{
		UpdateData(TRUE);
		int nCallSatelliteFlags = 0;
		CString str;
		this->GetDlgItemText(IDC_BUTTON_CALL,str);
		if (str == "����")
		{
			nCallSatelliteFlags = 1;
		} 
		else if(str == "�һ�")
		{
			nCallSatelliteFlags = 3;
		}
		else if(str == "����")
		{
			nCallSatelliteFlags = 2;	
		}
		//������ժ��
		//�ж�
		switch (nCallSatelliteFlags)
		{
		case 1:
			KillTimer(20);//ֹͣ���ڲ�ѯ
			bISCallProcess_WX = TRUE;
			bISCaller_WX = TRUE;
			flag_ST_out_busy=1;
			this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
			nATCmdFlags = 15;
			Send_AT_Command_ST("ATD"+m_target_number+";");//����
			break;
		case 2:	
			this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
			nATCmdFlags = 16;
			Send_AT_Command_ST("ATA");
			ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);	//����¼��
			bIsRecording = TRUE;
			break;
		case 3:
			SetTimer(20,(QUERY_ST+QUERY_INTERVAL),NULL);//�򿪲�ѯ��ʱ��
			bISCallProcess_WX = FALSE;
			flag_ST_out_busy=0;
			flag_ST_in_busy=0;
			this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("����");
			nATCmdFlags = 17;
			Send_AT_Command_ST("ATH");
			break;
			//		ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);	//����¼��
		default:
			break;
		}
	}
	else{
		AfxMessageBox("���޵绰��·���ã����Ե����ԣ�");
	}


}

void CBeidouDlg::OnChangeEditTargetnum() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if (m_target_number=="0")
	{
		GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(FALSE);
	}else{
		if(SerialPortOpenCloseFlag_WT||SerialPortOpenCloseFlag_3G)GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
	}
}

void CBeidouDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
//	applay_broadcast(1);

	if(modulereset==TRUE) 
	{
		module_reset(1);//��λģ�� 1�����ߵ绰��2�����ǵ绰��3��3Gģ�飻4������ģ�飻5���㲥�壻6��������
	}
	else if (soundswitch==TRUE)
	{
		sound_switch(1);
	}
	else{//���ߵ绰����
		chuanhao('1',ChuanHaoID);
	}
}

void CBeidouDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
//	applay_broadcast(2);

	if(modulereset==TRUE) 
	{
		module_reset(2);//��λģ�� 1�����ߵ绰��2�����ǵ绰��3��3Gģ�飻4������ģ�飻5���㲥�壻6��������
	}
	else if (soundswitch==TRUE)
	{
		sound_switch(2);
	}
	else{//���ߵ绰����
		chuanhao('2',ChuanHaoID);
	}
}

void CBeidouDlg::chuanhao(char num,CString ID)
{
	if (m_target_number=="0")//�����Ƴ�
	{
		m_target_number=m_target_number.Left(m_target_number.GetLength()-1);
	}
	if(SerialPortOpenCloseFlag_WT||SerialPortOpenCloseFlag_3G)GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(TRUE);
//	if(flag_PW_out_busy==0)GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(TRUE);//��������ʱ��ʧ���˸�ť
	
	m_target_number+=num;
	UpdateData(FALSE);

	if((flag_PW_in_busy==1)||(flag_PW_out_busy==1))
	{//���ߵ绰���������󣬲Ŵ���
		char lpOutBuffer[] = {'A','T','B','0','\r','\n'};//�����ϴ�ATHָ����йһ�
		lpOutBuffer[3]=num;
		CByteArray Array;
		Array.RemoveAll();
		Array.SetSize(6);		

		for (int i=0; i<6; i++)
		{
			Array.SetAt(i,lpOutBuffer[i]);
		}
		if(m_comm_WT.GetPortOpen())
		{
			m_comm_WT.SetOutput(COleVariant(Array));//��������
		}				
	}
	else if((flag_3G_in_busy==1)||(flag_3G_out_busy==1)){//3G�绰���������󣬲Ŵ���
		CString strSend;
		strSend = "AT^DTMF=";
		strSend += ID;
		strSend += ",";
		strSend += num;
		SendAtCmd(strSend,0);
	}
}

void CBeidouDlg::OnButton3() 
{
	// TODO: Add your control notification handler code here
	if(modulereset==TRUE) 
	{
		module_reset(3);//��λģ�� 1�����ߵ绰��2�����ǵ绰��3��3Gģ�飻4������ģ�飻5���㲥�壻6��������
	}
	else if (soundswitch==TRUE)
	{
		sound_switch(3);
	}
	else{//���ߵ绰����
		chuanhao('3',ChuanHaoID);
	}
}

void CBeidouDlg::OnButton4() 
{
	// TODO: Add your control notification handler code here
	if(modulereset==TRUE) 
	{
		module_reset(4);//��λģ�� 1�����ߵ绰��2�����ǵ绰��3��3Gģ�飻4������ģ�飻5���㲥�壻6��������
	}
	else if (soundswitch==TRUE)
	{
		sound_switch(4);
	}
	else{//���ߵ绰����
		chuanhao('4',ChuanHaoID);
	}
}

void CBeidouDlg::OnButton5() 
{
	// TODO: Add your control notification handler code here
	if(modulereset==TRUE) 
	{
		module_reset(5);//��λģ�� 1�����ߵ绰��2�����ǵ绰��3��3Gģ�飻4������ģ�飻5���㲥�壻6������壻7��12V���ţ�8��17V���ţ�
	}
	else if (soundswitch==TRUE)
	{
		sound_switch(5);
	}
	else{
		chuanhao('5',ChuanHaoID);
	}
}

void CBeidouDlg::OnButton6() 
{
	// TODO: Add your control notification handler code here
	if(modulereset==TRUE) 
	{
		module_reset(6);//��λģ�� 1�����ߵ绰��2�����ǵ绰��3��3Gģ�飻4������ģ�飻5���㲥�壻6������壻7��12V���ţ�8��17V���ţ�
	}
	else if (soundswitch==TRUE)
	{
		sound_switch(6);
	}
	else{
	chuanhao('6',ChuanHaoID);
	}
}

void CBeidouDlg::OnButton8() //��Ӧ����7����д����
{
	// TODO: Add your control notification handler code here
	if(modulereset==TRUE) 
	{
		module_reset(7);//��λģ�� 1�����ߵ绰��2�����ǵ绰��3��3Gģ�飻4������ģ�飻5���㲥�壻6������壻7��12V���ţ�8��17V���ţ�
	}
	else if (soundswitch==TRUE)
	{
		sound_switch(7);
	}
	else{
	chuanhao('7',ChuanHaoID);
	}
}

void CBeidouDlg::OnButton7() //��Ӧ����8����д����
{
	// TODO: Add your control notification handler code here
	if(modulereset==TRUE) 
	{
		module_reset(8);//��λģ�� 1�����ߵ绰��2�����ǵ绰��3��3Gģ�飻4������ģ�飻5���㲥�壻6������壻7��12V���ţ�8��17V���ţ�
	}
	else if (soundswitch==TRUE)
	{
//		sound_switch(8);
	}
	else{
	chuanhao('8',ChuanHaoID);
	}
}

void CBeidouDlg::OnButton9() 
{
	// TODO: Add your control notification handler code here
	if((modulereset==FALSE)&&(soundswitch==FALSE)){
	chuanhao('9',ChuanHaoID);
	}
}

void CBeidouDlg::OnButton10() 
{
	// TODO: Add your control notification handler code here
	if((modulereset==FALSE)&&(soundswitch==FALSE)){
	chuanhao('0',ChuanHaoID);
	}
}

void CBeidouDlg::OnButtonXing() 
{
	// TODO: Add your control notification handler code here
	if((modulereset==FALSE)&&(soundswitch==FALSE)){
	chuanhao('*',ChuanHaoID);
	}
}

void CBeidouDlg::OnButtonJing() 
{
	// TODO: Add your control notification handler code here
	if((modulereset==FALSE)&&(soundswitch==FALSE)){
	chuanhao('#',ChuanHaoID);
	}
}

void CBeidouDlg::OnButtonBack() 
{
	// TODO: Add your control notification handler code here
//	if(flag_PW_out_busy==1){//�����绰ʱ���˸���Դ��Ŷ����𲻵��κ����ã���ʱֱ�ӷ���
//		return;
//	}
	
	m_target_number=m_target_number.Left(m_target_number.GetLength()-1);
	if (m_target_number.GetLength()==0)
	{
		m_target_number+="0";
		if((flag_PW_in_busy==0)&&(flag_PW_out_busy==0)&&(flag_3G_in_busy==0)&&(flag_3G_out_busy==0)){
			GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(FALSE);
		}
		GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(FALSE);
	}
	UpdateData(FALSE);

}

void CBeidouDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	HANDLE hself = GetCurrentProcess();
 	TerminateProcess(hself, 0);
}

void CBeidouDlg::OnChangeEditFkxx() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_c_FKXX.LineScroll(m_c_FKXX.GetLineCount(),0);//�÷�����Ϣ����򱣳������һ��
}

void CBeidouDlg::OnComm_YW() 
{
	// TODO: Add your control notification handler code here
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len,k;
	BYTE rxdata[2048]; //����BYTE����
	CString strDisp="",strTmp="";
	int frequency_point=0;//Ƶ��ɨ����ܵ�Ƶ����
	double frequency_buf=0;//Ƶ�����
	unsigned char frame_receive[100]={0};//���ջ�����
	
	if((m_comm_YW.GetCommEvent()==2)) //�¼�ֵΪ2��ʾ���ջ����������ַ�
	{
		Sleep(100);//�ȴ�һ֡����ȫ���洢�ڻ�����������֡��ƴ�Ӳ������ӳ�
		variant_inp=m_comm_YW.GetInput(); //��������
		safearray_inp=variant_inp;  //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���
		len=safearray_inp.GetOneDimSize(); //�õ���Ч���ݳ���
		for(k=0;k<len;k++)
		{
			safearray_inp.GetElement(&k,rxdata+k);//ת��ΪBYTE������
		}
		
		//			AfxMessageBox("OK",MB_OK,0);
		//			frame=frame_len[frame_index_YW];
		//			frame_len[frame_index_YW]=0;
		
		frame_index_YW=0;
		for(k=0;k<len;k++)//������ת��ΪCString����
		{
			BYTE bt=*(char*)(rxdata+k);    //�ַ���
				if (rxdata[0]!='$')
				{
					return;//֡���ݴ�����
				}
			frame_receive[frame_index_YW]=bt;//frame_receive_YW
			frame_index_YW++;
// 			strTmp.Format("%c",bt);    //���ַ�������ʱ����strtemp���
// 			strDisp+=strTmp;  //������ձ༭���Ӧ�ַ���
			
		}
//		AfxMessageBox(strDisp,MB_OK,0);

	if (((flag_com_init_ack_YW==0)||(timer_board_disconnect_times_YW!=0))&&(frame_receive[1]=='r')&&(frame_receive[2]=='e')&&(frame_receive[3]=='y')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_wakeup_times)&&(frame_receive[9]==XOR(frame_receive,9)))//�״��������֣���λ���������ʱ�����ñ���$,\r,\n
	{
		flag_com_init_ack_YW=1;
		m_board_led_YW.SetIcon(m_hIconRed);
		GetDlgItem(IDC_STATIC_BOARDCONNECT_YW)->SetWindowText(" ��ά�������ӣ�"); 
		m_FKXX+="��ά�����Ӳ�ѯ������\r\n";
		timer_board_disconnect_times_YW=0;//�յ�����������

		int temperature=frame_receive[7]*256+frame_receive[8];
		m_temperature.Format("%.1f",(float)temperature/10);
		UpdateData(FALSE);
		
	}else if ((flag_com_init_ack_YW==1)&&(frame_receive[1]=='r')&&(frame_receive[2]=='s')&&(frame_receive[3]=='e')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_control_times)&&(frame_receive[8]==XOR(frame_receive,8)))//��λָ��ģ��
	{
	
		switch(frame_receive[7]-0x30){
		case 1:
			m_StatBar->SetText("��ά��״̬�����ߵ绰ģ�鸴λ���",4,0); 
			break;
		case 2:
			m_StatBar->SetText("��ά��״̬�����ǵ绰ģ�鸴λ���",4,0); 
			break;
		case 3:
			m_StatBar->SetText("��ά��״̬��3Gģ�鸴λ���",4,0); 
			break;
		case 4:
			m_StatBar->SetText("��ά��״̬������ģ�鸴λ���",4,0); 
			break;
		case 5:
			m_StatBar->SetText("��ά��״̬���㲥�帴λ���",4,0); 
			break;
		case 6:
			m_StatBar->SetText("��ά��״̬������帴λ���",4,0); 
			break;
		case 7:
			m_StatBar->SetText("��ά��״̬��12V���Ÿ�λ���",4,0); 
			break;
		case 8:
			m_StatBar->SetText("��ά��״̬��48V���Ÿ�λ���",4,0); 
			break;
		default:
			m_StatBar->SetText("��ά��״̬����λģ��ָ������",4,0); 
			break;
		}

	}else if ((flag_com_init_ack_YW==1)&&(frame_receive[1]=='s')&&(frame_receive[2]=='w')&&(frame_receive[3]=='h')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_control_times)&&(frame_receive[8]==XOR(frame_receive,8)))//�л���Ƶ��������֡
	{
//		if ((frame_receive[7]-0x30)!=5)sound_switch(5);//���Զ�Ӧ��ģʽ��,�л����Զ�Ӧ��

		if ((frame_receive[7]-0x30)==1)
		{
			m_StatBar->SetText("��ά��״̬���л������ߵ绰����",4,0); 
		} 
		else if ((frame_receive[7]-0x30)==2)
		{
			m_StatBar->SetText("��ά��״̬���л���3G����",4,0); 
		}
		else if ((frame_receive[7]-0x30)==3)
		{
			m_StatBar->SetText("��ά��״̬���л������ǵ绰����",4,0); 
		}
		else if ((frame_receive[7]-0x30)==4)
		{
			m_StatBar->SetText("��ά��״̬���л���PC���",4,0); 
		}else if ((frame_receive[7]-0x30)==5)
		{
			m_StatBar->SetText("��ά��״̬���л������ߵ绰����",4,0); 
		}else if ((frame_receive[7]-0x30)==6)
		{
			m_StatBar->SetText("��ά��״̬���л���3G����",4,0); 
		}else if ((frame_receive[7]-0x30)==7)
		{
			m_StatBar->SetText("��ά��״̬���л������ǵ绰����",4,0); 
		}
		


	}else if ((flag_com_init_ack_YW==1)&&(frame_receive[1]=='s')&&(frame_receive[2]=='c')&&(frame_receive[3]=='a')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_control_times)&&(frame_receive[7]==XOR(frame_receive,7)))//Ƶ��ɨ��֡������Ϣ
	{
		m_StatBar->SetText("��ά��״̬������ΪƵ��ɨ��ģʽ",4,0);
		
		
	}else if ((flag_com_init_ack_YW==1)&&(frame_receive[1]=='p')&&(frame_receive[2]=='p')&&(frame_receive[3]=='p')&&(frame_receive[4]=='_')
	&&(frame_receive[7]==XOR(frame_receive,7)))//����·����/�Ͽ�����ͨ��//&&(frame_receive[5]==index_wakeup_times)
	{
		if (frame_receive[6]==1)
		{
			KillTimer(14);//�رն���·���볬ʱ��ʱ��
			KillTimer(17);
			m_StatBar->SetText("��ά�壺����·�㲥����Ӧ��ɹ�",4,0);
			ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);//3G��ʼ¼��
			bIsRecord = TRUE;
			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
			PlaySound(".//Wav//PrepareSuccess.wav", NULL, SND_FILENAME|SND_ASYNC);//�㲥׼���ɹ����뿪ʼ�㲥
			GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
			broad_prepare=1;
		} 
		else if(frame_receive[6]==2)
		{
			m_StatBar->SetText("��ά�壺����·�㲥�Ͽ�Ӧ��ɹ�",4,0); 
			broad_prepare=2;
		}
		else if(frame_receive[6]==3)
		{
			KillTimer(14);//�رն���·���볬ʱ��ʱ��
			KillTimer(17);
			m_StatBar->SetText("��ά�壺����·�㲥����Ӧ��ɹ�",4,0);
			broad_prepare=1;
		}
		
	}else if ((flag_com_init_ack_YW==1)&&(frame_receive[1]=='r')&&(frame_receive[2]=='s')&&(frame_receive[3]=='t')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==0)&&(frame_receive[7]==0)&&(frame_receive[8]==XOR(frame_receive,8)))//�ش�֡
	{
		//	AfxMessageBox("wakaka",MB_OK,0);
		switch (index_resent_data_frame)
		{
		case 6://��ά�帴λ֡У����������ش�
	//		OnButtonBoardReset();
			break;
		case 7://Ƶ��ɨ�裬�̵�������֡У����������ش�
//			OnButtonScan();
			break;
		case 3://

			break;

		}
		m_StatBar->SetText("��ά��״̬����ά�������ش�",4,0);
	}  
	else
	{
		m_StatBar->SetText("��ά��״̬����ά��ش�֡�д���",4,0);
	}
	UpdateData(FALSE);
	}
}

void CBeidouDlg::OnButtonConnect_YW() 
{
	// TODO: Add your control notification handler code here
// 	char buff[2];
// 	CString string1="",string2="";
// 	buff[1]='\0';
// 	buff[0]=m_DParity;
// 	string1.Format(_T("%d"),m_DBaud);
// 	string1+=",";
// 	string2=buff;
// 	string1+=string2;
// 	string1+=",";
// 	string2.Format(_T("%d"),m_DDatabits); 
// 	string1+=string2;
// 	string1+=",";
// 	string2.Format(_T("%d"),m_DStopbits);
// 	string1+=string2;
	CString string1="115200,n,8,1";
/*
	CString   tmp;
	tmp.Format( "%d ",string1);
	MessageBox( "config:"+string1);
*/
	if(SerialPortOpenCloseFlag_YW==FALSE)
	{
		SerialPortOpenCloseFlag_YW=TRUE;

		//�����Ǵ��ڵĳ�ʼ������
		if(m_comm_YW.GetPortOpen())//�򿪶˿�ǰ�ļ�⣬�ȹأ��ٿ�
			MessageBox("�����޷��򿪣�");
//			m_comm.SetPortOpen(FALSE);	//	
		m_comm_YW.SetCommPort(m_DCom_YW); //ѡ��˿ڣ�Ĭ����com3
		m_comm_YW.SetSettings((LPSTR)(LPCTSTR)string1); //������115200����У�飬8������λ��1��ֹͣλ
		if(!m_comm_YW.GetPortOpen())
		{			
			m_comm_YW.SetPortOpen(TRUE);//�򿪴���
			GetDlgItem(IDC_BUTTON_CONNECTYUNWEI)->SetWindowText("�رմ���");
			m_StatBar->SetText("��ά��״̬�������Ѵ�",4,0); //��������

			m_ctrlIconOpenoff_YW.SetIcon(m_hIconRed);
			UpdateData();

			if (index_wakeup_times<200)
			{
				index_wakeup_times++;
				if ((index_wakeup_times==0x0d)||(index_wakeup_times==0x24))
				{
					index_wakeup_times++;
				}
			} 
			else
			{
				index_wakeup_times=0;
			}
			frame_board_check_YW[5]=index_wakeup_times;
			frame_board_check_YW[6]=XOR(frame_board_check_YW,6);
			if ((frame_board_check_YW[6]=='$')||(frame_board_check_YW[6]==0x0d))
			{
				frame_board_check_YW[6]++;//����������$��0x0d����ֵ��һ
			}
			frame_board_check_YW[7]='\r';
			frame_board_check_YW[8]='\n';
			CByteArray Array;
			Array.RemoveAll();
			Array.SetSize(7+2);
									
			for (int i=0;i<(7+2);i++)
			{
				Array.SetAt(i,frame_board_check_YW[i]);
			}

			if(m_comm_YW.GetPortOpen())
			{
				m_comm_YW.SetOutput(COleVariant(Array));//��������
			}
			index_resent_data_frame=0;//����֡��֧���ش�����
			SetTimer(6,(QUERY_INTERVAL+QUERY_YW),NULL);//û�д���Ƶ��ɨ��׶βŴ򿪶��ڲ�ѯ��10+3��(��㲥��Ĳ�ѯ��ʱ���ϴ�)��ѯһ���Ӱ��Ƿ񱣳����ӡ��ָ�Ӳ������ʱ�������Զ�����
		}
		else
			MessageBox("�޷�����ά�崮�ڣ������ԣ�");	 
	}
	else
	{
		SerialPortOpenCloseFlag_YW=FALSE;
		GetDlgItem(IDC_BUTTON_CONNECTYUNWEI)->SetWindowText("�򿪴���");
		m_StatBar->SetText("��ά��״̬�������ѹر�",4,0); //��������
		m_ctrlIconOpenoff_YW.SetIcon(m_hIconOff);
		m_board_led_YW.SetIcon(m_hIconOff);
		GetDlgItem(IDC_STATIC_BOARDCONNECT_YW)->SetWindowText(" ��ά���ѶϿ���");
		flag_com_init_ack_YW=0;//��ά��δ����
		m_comm_YW.SetPortOpen(FALSE);//�رմ���
		KillTimer(6);

 	}
}

void CBeidouDlg::OnSelendokComboComselectYw() 
{
	// TODO: Add your control notification handler code here
	m_DCom_YW=m_Com_YW.GetCurSel()+1;
	UpdateData();
	
	CString strTemp;
	strTemp.Format(_T("%d"),m_DCom_YW-1);
	::WritePrivateProfileString("ConfigInfo","com_YW",strTemp,".\\config_phonemessage.ini");
	
	strTemp.Format(_T("%d"),m_DCom_YW);
	::WritePrivateProfileString("ConfigInfo","com_r_YW",strTemp,".\\config_phonemessage.ini");
}

void CBeidouDlg::module_reset(int index)//��λָ��ģ��
{
	if(SerialPortOpenCloseFlag_YW==TRUE)//ֻ�е���ά�崮�ڴ��ˣ��ſ���ʹ��
	{
		if (index_control_times<200)
		{
			index_control_times++;
			if ((index_control_times==0x0d)||(index_control_times==0x24))
			{
				index_control_times++;
			}
		} 
		else
		{
			index_control_times=0;
		}
		frame_board_reset_YW[5]=index_control_times;
		frame_board_reset_YW[6]=index+0x30;//��λģ�� 1�����ߵ绰��2�����ǵ绰��3��3Gģ�飻4������ģ�飻5���㲥�壻6������壻7��12V���ţ�8��17V���ţ�
		frame_board_reset_YW[7]=XOR(frame_board_reset_YW,7);
		if ((frame_board_reset_YW[7]=='$')||(frame_board_reset_YW[7]==0x0d))
		{
			frame_board_reset_YW[7]++;//����������$��0x0d����ֵ��һ
		}
		frame_board_reset_YW[8]='\r';
		frame_board_reset_YW[9]='\n';
		CByteArray Array;
		Array.RemoveAll();
		Array.SetSize(8+2);
		
		for (int i=0;i<(8+2);i++)
		{
			Array.SetAt(i,frame_board_reset_YW[i]);
		}
		
		if(m_comm_YW.GetPortOpen())
		{
			m_comm_YW.SetOutput(COleVariant(Array));//��������
		}
		index_resent_data_frame=6;//6��ά�帴λ�ش�֡���
	}	
}

void CBeidouDlg::OnModuleReset() 
{
	// TODO: Add your control notification handler code here
	if (soundswitch==TRUE)
	{
		OnSoundSwitch();
	}
	 
	if (modulereset==FALSE)
	{
		modulereset=TRUE;
		GetDlgItem(IDC_MODULE_RESET)->SetWindowText("ȡ����λ");
		
		GetDlgItem(IDC_BUTTON_1)->SetWindowText("���ߵ绰");
		GetDlgItem(IDC_BUTTON_2)->SetWindowText("���ǵ绰");
		GetDlgItem(IDC_BUTTON_3)->SetWindowText("3G");
		GetDlgItem(IDC_BUTTON_4)->SetWindowText("����");

		GetDlgItem(IDC_BUTTON_5)->SetWindowText("�㲥��");
		GetDlgItem(IDC_BUTTON_6)->SetWindowText("�����");
		GetDlgItem(IDC_BUTTON_8)->SetWindowText("12V����");
		GetDlgItem(IDC_BUTTON_7)->SetWindowText("48V����");
	} 
	else
	{
		modulereset=FALSE;
		GetDlgItem(IDC_MODULE_RESET)->SetWindowText("�ֶ���λ");

		GetDlgItem(IDC_BUTTON_1)->SetWindowText("1");
		GetDlgItem(IDC_BUTTON_2)->SetWindowText("2");
		GetDlgItem(IDC_BUTTON_3)->SetWindowText("3");
		GetDlgItem(IDC_BUTTON_4)->SetWindowText("4");

		GetDlgItem(IDC_BUTTON_5)->SetWindowText("5");
		GetDlgItem(IDC_BUTTON_6)->SetWindowText("6");
		GetDlgItem(IDC_BUTTON_8)->SetWindowText("7");
		GetDlgItem(IDC_BUTTON_7)->SetWindowText("8");
	}
}

void CBeidouDlg::sound_switch(int index)
{
	if(SerialPortOpenCloseFlag_YW==TRUE)//ֻ�е���ά�崮�ڴ��ˣ��ſ���ʹ��
	{
		if (index_control_times<200)
		{
			index_control_times++;
			if ((index_control_times==0x0d)||(index_control_times==0x24))
			{
				index_control_times++;
			}
		} 
		else
		{
			index_control_times=0;
		}
		frame_board_sound_YW[5]=index_control_times;
		frame_board_sound_YW[6]=index+0x30;//��Ƶ�л� 1�����ߵ绰��2��3G��3�����ǵ绰��4��PC��Ƶ�����5���㲥�壻6������壻7��12V���ţ�8��48V���ţ�10��ʹ�ý������лؿ���ģʽ��
		frame_board_sound_YW[7]=XOR(frame_board_sound_YW,7);
		if ((frame_board_sound_YW[7]=='$')||(frame_board_sound_YW[7]==0x0d))
		{
			frame_board_sound_YW[7]++;//����������$��0x0d����ֵ��һ
		}
		frame_board_sound_YW[8]='\r';
		frame_board_sound_YW[9]='\n';
		CByteArray Array;
		Array.RemoveAll();
		Array.SetSize(8+2);
		
		for (int i=0;i<(8+2);i++)
		{
			Array.SetAt(i,frame_board_sound_YW[i]);
		}
		
		if(m_comm_YW.GetPortOpen())
		{
			m_comm_YW.SetOutput(COleVariant(Array));//��������
		}
		index_resent_data_frame=6;//6��ά�帴λ�ش�֡���
	}	
}

void CBeidouDlg::OnSoundSwitch() 
{
	// TODO: Add your control notification handler code here
	if (modulereset==TRUE)
	{
		OnModuleReset();//��Ƶ�л����ֶ���λֻ��һ������
	}
	if (soundswitch==FALSE)
	{
		soundswitch=TRUE;
		GetDlgItem(IDC_SOUND_SWITCH)->SetWindowText("�ر��л�");

		GetDlgItem(IDC_BUTTON_1)->SetWindowText("���ߵ绰����");
		GetDlgItem(IDC_BUTTON_2)->SetWindowText("3G����");
		GetDlgItem(IDC_BUTTON_3)->SetWindowText("���ǵ绰����");
		GetDlgItem(IDC_BUTTON_4)->SetWindowText("PC��Ƶ���");

		GetDlgItem(IDC_BUTTON_5)->SetWindowText("���ߵ绰����");
		GetDlgItem(IDC_BUTTON_6)->SetWindowText("3G����");
		GetDlgItem(IDC_BUTTON_8)->SetWindowText("���ǵ绰����");
	} 
	else
	{
		soundswitch=FALSE;
		GetDlgItem(IDC_SOUND_SWITCH)->SetWindowText("��Ƶ�л�");

		GetDlgItem(IDC_BUTTON_1)->SetWindowText("1");
		GetDlgItem(IDC_BUTTON_2)->SetWindowText("2");
		GetDlgItem(IDC_BUTTON_3)->SetWindowText("3");
		GetDlgItem(IDC_BUTTON_4)->SetWindowText("4");

		GetDlgItem(IDC_BUTTON_5)->SetWindowText("5");
		GetDlgItem(IDC_BUTTON_6)->SetWindowText("6");
		GetDlgItem(IDC_BUTTON_8)->SetWindowText("7");
	}
}

void CBeidouDlg::OnButtonClearmsg() 
{
	// TODO: Add your control notification handler code here
	m_FKXX="";
	UpdateData(FALSE);

}

void CBeidouDlg::OnChangeEditSendmsg() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}


void CBeidouDlg::OnButtonConnect_3G() 
{
	// TODO: Add your control notification handler code here
	CString string1="115200,n,8,1";
	
	if(SerialPortOpenCloseFlag_3G==FALSE)
	{
		SerialPortOpenCloseFlag_3G=TRUE;
		
		//�����Ǵ��ڵĳ�ʼ������
		if(m_comm_3G.GetPortOpen())//�򿪶˿�ǰ�ļ�⣬�ȹأ��ٿ�
			MessageBox("�����޷��򿪣�");
		m_comm_3G.SetCommPort(m_DCom_3G); //ѡ��˿ڣ�Ĭ����com3
		m_comm_3G.SetSettings((LPSTR)(LPCTSTR)string1); //������115200����У�飬8������λ��1��ֹͣλ
		if(!m_comm_3G.GetPortOpen())
		{			
			m_comm_3G.SetPortOpen(TRUE);//�򿪴���
			GetDlgItem(IDC_BUTTON_CONNECT3G)->SetWindowText("�رմ���");
			m_StatBar->SetText("3G״̬�������Ѵ�",2,0); //��������
			UpdateData();
			bIsINIProcess = TRUE;
			bINIFail = TRUE;
			SendAtCmd("ATE0",2);   //�������� 0��MS �����ʹ�TE ���յ��ַ�  1��MS ���ʹ�TE ���յ��ַ���
			SetTimer(49,5000,NULL);
			index_resent_data_frame=0;//����֡��֧���ش�����
		}
		else
			MessageBox("�޷���3G���ڣ������ԣ�");	 
	}
	else
	{
		nATCmdID=0;//ֹͣ��ʱ����˳�����
		SerialPortOpenCloseFlag_3G=FALSE;
		GetDlgItem(IDC_BUTTON_CONNECT3G)->SetWindowText("�򿪴���");
		m_StatBar->SetText("3G״̬�������ѹر�",2,0); //��������
		m_ctrlIconOpenoff_3G.SetIcon(m_hIconOff);
		m_board_led_3G.SetIcon(m_hIconOff);
		m_3g_m_r.SetIcon(m_hIconOff);
		m_3g_p_r.SetIcon(m_hIconOff);
		state_system[1]=1;//3G���ڹر�
		state_system[3]=1;
		GetDlgItem(IDC_STATIC_BOARDCONNECT_3G)->SetWindowText(" 3G�ѶϿ���");
		//		flag_com_init_ack_3G=0;//��ά��δ����
		m_comm_3G.SetPortOpen(FALSE);//�رմ���
		KillTimer(10);
 	}	
}

void CBeidouDlg::OnSelendokComboComselect3G() 
{
	// TODO: Add your control notification handler code here
	m_DCom_3G=m_Com_3G.GetCurSel()+1;
	UpdateData();
	
	CString strTemp;
	strTemp.Format(_T("%d"),m_DCom_3G-1);
	::WritePrivateProfileString("ConfigInfo","com_3G",strTemp,".\\config_phonemessage.ini");
	
	strTemp.Format(_T("%d"),m_DCom_3G);
	::WritePrivateProfileString("ConfigInfo","com_r_3G",strTemp,".\\config_phonemessage.ini");	
}

void CBeidouDlg::SendAtCmd(CString strAT, int nATID)
{
	nATCmdID = nATID;
	unsigned char uChar[1024];
	int len;
	memcpy(uChar,strAT,strAT.GetLength());
	len = strAT.GetLength() +2;
	uChar[len-2] = '\r';
	uChar[len-1] = '\n';
	
	CByteArray Array;
	Array.RemoveAll();
	Array.SetSize(len);
	
	for (int k=0;k<len;k++)
	{
		Array.SetAt(k,uChar[k]);
		m_FKXX += uChar[k];
	}
	if(m_comm_3G.GetPortOpen())
	{
		m_comm_3G.SetOutput(COleVariant(Array));//��������		
		UpdateData(FALSE);
	}
}

void CBeidouDlg::OnComm_3G() 
{
	// TODO: Add your control notification handler code here
		 VARIANT variant_inp;
     COleSafeArray safearray_inp;
     LONG len,k;
     BYTE rxdata[2048]; //����BYTE���� An 8-bit integerthat is not signed.
     CString strtemp;

	 int nIndex = 0;//����
     if(m_comm_3G.GetCommEvent()==2) //�¼�ֵΪ2��ʾ���ջ����������ַ�
     {////////��������Ը����Լ���ͨ��Э����봦�����
		 Sleep(100);
		 variant_inp=m_comm_3G.GetInput(); //��������
		 safearray_inp=variant_inp; //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���
		 len=safearray_inp.GetOneDimSize(); //�õ���Ч���ݳ���
		 m_comm_3G.GetInput();    //��Ԥ���������������������
		 int nTemp = strlen(cReceiveData);
		 for(k=0;k<len;k++)
			 safearray_inp.GetElement(&k,rxdata+k);//ת��ΪBYTE������
		 nIndex = nTemp;
		 for(k=0;k<len;k++) //������ת��ΪCstring�ͱ���
		 {
			 BYTE bt=*(char*)(rxdata+k); //�ַ���
			 cReceiveData[nIndex]=bt;
			 nIndex++;//�������ԣ�cReceiveDataǰ��������Ϊ0D0A ������index=2����ʼ
			 strtemp.Format("%c",bt); //���ַ�������ʱ����strtemp���
			 m_FKXX+=strtemp; //������ձ༭���Ӧ�ַ��� 
			 UpdateData(FALSE); //���±༭������
		 }
			cReceiveData[nIndex]= '\0';
			strtemp = cReceiveData;

		if(nATCmdID == 2)
		{
			if(strtemp.Find("OK")!=-1)
			{
				KillTimer(49);
				SetTimer(40,1000,NULL);	
				nATCmdID = 1;
			}
		}
		else if(nATCmdID == 40)
		{					
			if (strtemp.Find("+CMGL") != -1)
			{
				bRoughLongSMS = TRUE;
			}
			else
			{
				bIsNewSMS = FALSE;
				bReadsmsTimeFlag = FALSE;
			}
		}

		

		if ((cReceiveData[2+0]=='R')&&(cReceiveData[2+1]=='I')&&(cReceiveData[2+2]=='N')&&(cReceiveData[2+3]=='G'))   //��������
		{
			if (state_system[1]==1)
			{
				return;
			}

			flag_3G_in_busy = 1;
			KillTimer(10); 
			if (strtemp.Find("+CLIP") != -1)
			{
				if (call_in_number_3G.IsEmpty()!=0)
				{
					int	n=strtemp.FindOneOf("\"");
					strtemp.Delete(n,1);
					int	k=strtemp.FindOneOf("\"");
					call_in_number_3G = strtemp.Mid(n,k-n);
					UpdateData(false);
				}
			}	//���磬�Զ�Ӧ��
			
			bIsCaller = FALSE;//��ɱ���
			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�����绰");			
			UpdateData(FALSE);
			OnButtonCall(); //��ʵ��ͨ��
			sound_switch(6);//�л���3G����ģʽ
			SetTimer(31,AUTORESPONSE_TIME,NULL);//15s�ȴ�ʱ�䣨�ȴ��û����룬ʱ������˲���������ʱ�䣩
			PlaySound(".//Wav//Guide.wav", NULL, SND_FILENAME|SND_ASYNC);//Ӧ����̨ϵͳ���Զ��㲥�밴1��ԭʼͨ���밴2,#�Ž���
			
		}
		else if (strtemp.Find("+CLIP") != -1)//������ʾ
		{
			int	n=strtemp.FindOneOf("\"");
			strtemp.Delete(n,1);
			int	k=strtemp.FindOneOf("\"");
			call_in_number_3G = strtemp.Mid(n,k-n);
			m_target_number = call_in_number_3G;
			UpdateData(false);
		}
		else if(strtemp.Find("BEGIN")!=-1)//ժ���ظ�ָ��
		{			 //^CONN: BEGIN ͨ����ʼ	����¼��Ҫд���Զ�Ӧ������
			m_FKXX+="3Gժ���ɹ�\r\n";
			if (bIsCaller == TRUE)  //����
			{
				if(!bIsRecord)//¼����־λΪFALSE
				{
					bIsRecord = TRUE;
					ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);//3G��ʼ¼��
				}
				GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
				UpdateData(FALSE);
				bIsRecord=TRUE;
				SendAtCmd("AT+CLCC",0);
			}
		}
		else if(strtemp.Find("NO CARRIER")!=-1)//�Է��һ��ظ�ָ��
		{
			sound_switch(4);//3G����ͨ�����л�Ĭ�Ͽ���
			SetTimer(19,3000,NULL);//ȫ��������·����ʱ�����ڽ����㲥
			SetTimer(10,(QUERY_INTERVAL+QUERY_3G),NULL);
			KillTimer(17);
			KillTimer(31);
			KillTimer(32);
			applay_broadcast(2);//������ֹ�㲥
			KillTimer(18);//�Է��һ���رն���������
			m_StatBar->SetText("��ά��״̬������·�㲥��ֹ���·���δӦ��",4,0);
			ChuanHaoID = "";
			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("����绰");
			OnButtonClearNum();
			bRadioIsReady = FALSE;
			bIsAutoResponseProcess = FALSE;
			nCountCheck = 0;
			if (bIsRecord==TRUE)
			{	
			flag_3G_out_busy = 0;
			flag_3G_in_busy = 0;
			HWND hWnd = ::FindWindow(NULL, _T("record11"));//�ر�¼��
			if (NULL != hWnd) {
				::SendMessage(hWnd, WM_CLOSE, 0, 0);
			}	
			bIsCaller = TRUE;  
			bIsRecord=FALSE;
			}	
		}	 
		else if(strtemp.Find("CEND")!=-1)//ͬ��
		{			 // ^CEND: 000015�����ţ� ͨ������
			sound_switch(4);//3G����ͨ�����л�Ĭ�Ͽ���
			SetTimer(19,3000,NULL);//ȫ��������·����ʱ�����ڽ����㲥
			SetTimer(10,(QUERY_INTERVAL+QUERY_3G),NULL);
			flag_3G_out_busy = 0;
			flag_3G_in_busy = 0;
			bIsAutoResponseProcess = FALSE;
			nCountCheck = 0;
			KillTimer(17);
			KillTimer(31);
			KillTimer(32);
			OnButtonClearNum();
			applay_broadcast(2);//������ֹ�㲥
			KillTimer(18);//�Է��һ���رն���������
			m_StatBar->SetText("��ά��״̬������·�㲥��ֹ���·���δӦ��",4,0);
			ChuanHaoID = "";
			UpdateData(TRUE);			
			 if (bIsRecord==TRUE)
			 {	
			//	OnButtonCall();	
				HWND hWnd = ::FindWindow(NULL, _T("record11"));//�ر�¼��
				if (NULL != hWnd) {
					::SendMessage(hWnd, WM_CLOSE, 0, 0);
				}	
				bIsCaller = TRUE;  
				bIsRecord=FALSE;
			 }
			 bRadioIsReady = FALSE;
			 GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("����绰");
		}
		else if(strtemp.Find("+CLCC")!=-1)//��������
		{			 			
			int	n=strtemp.Find("+CLCC:");
			int k=strtemp.Find(",");				
			ChuanHaoID = strtemp.Mid(n+7,k-(n+7)); //��Ҫ��
		}
		else if(strtemp.Find("+RXDTMF:")!=-1) //����
		{			 //������ʾ	 +RXDTMF: 1  			
			if((bIsCaller == FALSE)&&(bIsAutoResponseProcess == FALSE)) //ֻ�б��кʹ����Զ�Ӧ������вŶԴ��Ž��в��������й㲥��������ժ����Ͳ������Զ�Ӧ�������
			{		
				send_3Gstring+=cReceiveData[11];
				if(send_3Gstring.Find("#")!=-1)
				{		
					CString stre = send_3Gstring.Mid(0,1);//ֻȡ��һλ
					send_3Gstring = stre;
					send_3Gstring += '#';//����#��
					
					m_FKXX+="3G����:";
					m_FKXX+=send_3Gstring;
					m_FKXX+="\r\n";
					UpdateData(FALSE);
					KillTimer(31);
					nCountCheck++;
					if (send_3Gstring == "1#")//�Զ��㲥
					{
						SetTimer(17,20000,NULL);
						bIsAutoResponseProcess = TRUE;
						applay_broadcast(1);//���뿪ʼ�㲥
						m_StatBar->SetText("��ά��״̬������·�㲥�������·���δӦ��",4,0); 
						//֪ͨ��ά���㲥����
						PlaySound(".//Wav//BroadPrepare.wav", NULL, SND_FILENAME|SND_ASYNC);//�㲥����׼���У����Ժ�   (4s)
					}     
					else if (send_3Gstring == "2#")//ԭ��ͨ��
					{
						PlaySound(".//Wav//RingConnect.wav", NULL, SND_FILENAME|SND_ASYNC);//ͨ�����������У����Ժ�   (5s)   //�������Ūһ�α�������(���ظ�����)
						SetTimer(32,NOPICKUP_TIME ,NULL);//�Զ�������ȴ�
						bIsAutoResponseProcess = TRUE;
						//���̨��Ա֪ͨ���� 
						SetTimer(18,4000,NULL);//ÿ��4s����һ��ring������
						//����OnButtonCall �������һ��״̬��־��������ժ��
						GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
						GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("����ժ��");
					}
					else
					{
						if (nCountCheck < 3)
						{
							PlaySound(".//Wav//ErrorCmd.wav", NULL, SND_FILENAME|SND_ASYNC);//ָ���������������
							SetTimer(31,AUTORESPONSE_TIME,NULL);//15s�ȴ�ʱ�䣨�ȴ��û����룬ʱ������˲���������ʱ�䣩
						} 
						else
						{										
							GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
							if((flag_3G_in_busy==1)||(flag_3G_out_busy==1)){
								GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
								OnButtonCall();
							}
						}
						
					}
					
					send_3Gstring="";
					UpdateData(FALSE);
				}	
			}
		}
					else if(strtemp.Find("+CMGL:")!=-1)//���Ų�ѯ��������
					{		
						KillTimer(10);
							CString stra;
							int t = 0;
							int	n=strtemp.Find("+CMGL:");
							int k=strtemp.Find(",");				
							stra = strtemp.Mid(n+7,k-(n+7)); //��Ҫ��
							nSMSCount++;
							t=atoi(stra);
							SMSIndex[t] = stra;
							strtemp.Delete(n,30);							
							while (strtemp.Find("+CMGL:")!=-1)
							{
								n=strtemp.Find("+CMGL:");
								k=strtemp.Find(",");
					//			stra = strtemp.Mid(n+7,k+2-(n+7));
								nSMSCount++;
								stra = strtemp.Mid(n+7,k-(n+7));
								t=0;
								t=atoi(stra);
								SMSIndex[t] = stra;
								strtemp.Delete(n,30);
							}
							if (strtemp.Find("OK")!=-1)
								SetTimer(43,1000,NULL);	
							else
								SetTimer(43,4000,NULL); //��ֹ(5������)�¶��ŵĵ���	
					 }
					else if(strtemp.Find("+CMGR:")!=-1)//��ȡ��������
					 {
						bool bSMSToDecode = FALSE;//�ֲ���������ʾ�Ƿ���н���
						if (bReadsmsTimeFlag == FALSE)//��ʱ��û����
						{	
							if (strtemp.Find("050003")!=-1)
							{		
								SetTimer(42,12000,NULL);  //���Ų�ѯ�ڶ�ʱ��42���� 
							bReadsmsTimeFlag = TRUE;
								bRoughLongSMS = TRUE;
								for(int t=0; t<SMSSIZE_10; t++)
								{
									SMSIndex[t] = "";
									strLongSMSTextStore[t] = "";
									strLongSMSIDStore[t] = "";
								}										
						/*		int k = strtemp.Find("050003");
								CString strd;
								strd = strtemp.Mid(k+8,2);
								k = atoi(strd);*/

								if (nSMSCount > 0) //������Ϊ�˴洢���г����ţ�����AT+CMGL=4��nSMSCount>0���п������ϴν��룬û��ȫ��
								{
									bSMSToDecode = TRUE;
								} 
								else //��һ��������
								{
									bSMSToDecode = FALSE;
								}								
							}
							else if (strtemp.Find("060804")!=-1)
							{
								SetTimer(42,12000,NULL);  //���Ų�ѯ�ڶ�ʱ��42���� 
							bReadsmsTimeFlag = TRUE;
								bRoughLongSMS = TRUE;
								for(int t=0; t<SMSSIZE_10; t++)
								{
									SMSIndex[t] = "";
									strLongSMSTextStore[t] = "";
									strLongSMSIDStore[t] = "";
								}									
						/*		int x = strtemp.Find("060804");
								CString strd;
								strd = strtemp.Mid(x+10,2);
								x = atoi(strd);*/
								
								if (nSMSCount > 0) //��������
								{
									bSMSToDecode = TRUE;
								} 
								else //��һ��������
								{
									bSMSToDecode = FALSE;
								}								
							}
							else
							{
								bSMSToDecode = TRUE;
							}
						}
						else
						{
							if ((strtemp.Find("050003")!=-1)||(strtemp.Find("060804")!=-1))
							{
								bRoughLongSMS = TRUE;
								bSMSToDecode = TRUE;
							}
							else
							{
								AfxMessageBox("bReadsmsTimeFlag ����");
							}

						}

						if (bSMSToDecode)//����
						{
							CString stra;
							CString str2;
							int	n=strtemp.Find(":");
							int k=strtemp.Find("OK");
							if (k==-1)
								m_StatBar->SetText("3G״̬: �������ݽ�β��OK",2,0); //��������
							else
							{
								str2 = strtemp.Mid(n+2,1);
								int h=strtemp.Find("\r\n",n);
								stra = strtemp.Mid(h+2,k-4-(h+2));//�Ѿ�ȡ����ֻ����char������
								n=0;
								n=atoi(str2);
								char *pSrc = (LPSTR)(LPCTSTR)stra;					
								sms = ReadSMS(pSrc,sms,n);
								if(bTimer48OnOff == FALSE)
								{
									bTimer48OnOff = TRUE;
									SetTimer(48,120*1000,NULL);//���Ž���������û�أ�����CSQ
								}
								SetTimer(41,500,NULL);
							}
						}
					 }
					else if(strtemp.Find("+CMTI:")!=-1)//�¶���
					{
						KillTimer(10);						
						if ((bIsNewSMS == FALSE)&&(bReadsmsTimeFlag == FALSE)) //���¶��ţ��Ҷ�ʱ��û����
						{
							bIsNewSMS = TRUE;

							m_StatBar->SetText("3G״̬: �¶���",2,0); //��������
							int k=strtemp.Find(",");				
							strShortSMSIndex = strtemp.Mid(k+1,1); //��Ҫ��							
							SendAtCmd("AT+CMGR="+strShortSMSIndex,32);//��ȡָ������   AT+CMGR 32
						}
						else
						{
							//�����κ�����
						}							
					}
					else if(strtemp.Find(">")!=-1) //���Ͷ��ţ�PDU���ݵķ����ڶ�ʱ��41����
					{
					//	SetTimer(40,1000,NULL);
						nATCmdID = 23;			
					}	
					
					else if(strtemp.Find("CSQ")!=-1)//��ѯ�ź�ǿ��
					{			 //AT+CSQ
						timer_board_disconnect_times_3G=0;//���3G����
						if((tel_manual_select==0)||(m_tel_manual_select.GetCurSel()==2)){//�绰���֣��Զ���ѡ��3Gʱ�����޸�
							state_system[1]=0;//3G��ʱ�����3G����
						}
						if((msg_manual_select==0)||(m_msg_manual_select.GetCurSel()==1)){//���Ų��֣��Զ���ѡ��3Gʱ�����޸�
							state_system[3]=0;
						}
						m_board_led_3G.SetIcon(m_hIconRed);
						m_3g_m_r.SetIcon(m_hIconRed);
						m_3g_p_r.SetIcon(m_hIconRed);
						GetDlgItem(IDC_STATIC_BOARDCONNECT_3G)->SetWindowText(" 3G�����ӣ�");
						m_FKXX+="3G�źŲ�ѯ������\r\n";
						UpdateData(FALSE);

						CString str3;
						int	n=strtemp.FindOneOf(":");
						int	k=strtemp.FindOneOf(",");
						str3 = strtemp.Mid(n+2,k-(n+2));
						int j = 0;
						j = atoi(str3);
						if ((j<=8)&&(j>0))
						{
							this->GetDlgItem(IDC_RSSI)->SetWindowText("�ź�ǿ��: ��");
						} 
						else if ((j>=9)&&(j<=14))
						{
							this->GetDlgItem(IDC_RSSI)->SetWindowText("�ź�ǿ��: ��");
						}
						else
						{
							if (strtemp.Find("99,")!=-1)
							{
								this->GetDlgItem(IDC_RSSI)->SetWindowText("�ź�δ֪");
							}
							else
							{
								this->GetDlgItem(IDC_RSSI)->SetWindowText("�ź�ǿ��: ��");
							}
						}
					//	str3 = strtemp.Mid(k-2,5);
						UpdateData(false);
					}
					else if((strtemp.Find("ERROR")!=-1)&&(bIsINIProcess == TRUE))//��ʼ�����ش���
					{			 
						bINIFail = FALSE;
					}
					else if((strtemp.Find("OK")!=-1)&&(41 == nATCmdID))
					{
						nATCmdID = 0;
						bIsINIProcess = FALSE;
					}
					else if(strtemp.Find("CMS ERROR")!=-1) //���Ŵ��󷵻�
					{			 //CMS
						CString str4;
						int	n=strtemp.FindOneOf("CMS ERROR");
						str4 = strtemp.Mid(n,14);
						m_FKXX+=str4;
						UpdateData(false);
						//ȡ���֣�Ȼ�������Ӧ��ȡֵ����
						if (strtemp.Find("3")!=-1)
						{
							n=strtemp.Find("3");
							str4 = strtemp.Mid(n,3);
							CMSError(str4);
						} 
						else if(strtemp.Find("5")!=-1 )
						{
							n=strtemp.Find("5");
							str4 = strtemp.Mid(n,3);
							CMSError(str4);
						}
					}
					else if(strtemp.Find("+CMGS")!=-1) //���ŷ��ͳɹ������ж��ű���  
					{
						CString str,strDateTime;
						CTime m_Time;
						m_Time = CTime::GetCurrentTime();
						strDateTime = m_Time.Format(_T("%Y/%m/%d %H:%M:%S %A"));
						UpdateData(TRUE);
						str = "�ռ���:";
						str += m_target_number;
						str += "\r\n";
						char pSrc[1500]; 
						strncpy(pSrc,(LPCTSTR)str,str.GetLength());
						CFile m_File;
						m_File.Open(".//SendSMS.txt",CFile::modeCreate|CFile::modeReadWrite|CFile::modeNoTruncate|CFile::typeBinary|CFile::shareDenyNone,NULL);  
						m_File.SeekToEnd();
						m_File.Write(pSrc,sizeof(char)*str.GetLength());
						str = "ʱ��:";
						str += strDateTime;
						str += "\r\n";
						strncpy(pSrc,(LPCTSTR)str,str.GetLength());
						m_File.SeekToEnd();
						m_File.Write(pSrc,sizeof(char)*str.GetLength());
						str = "����:";
						str += m_sendmsg;
						str += "\r\n";
						strncpy(pSrc,(LPCTSTR)str,str.GetLength());
						m_File.SeekToEnd();
						m_File.Write(pSrc,sizeof(char)*str.GetLength());
						m_File.Flush();
						m_File.Close();
						if (nLongSMSSendCount == 0)
						{
							this->GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
							UpdateData(FALSE);
							m_StatBar->SetText("3G״̬: ���ŷ��ͳɹ�",2,0); //��������
						} 
						else
						{
							SetTimer(47,100,NULL);
						}
					}
					
							
		memset(cReceiveData,0,2048);
		strtemp = "";
		UpdateData(false);
  }
}

SMSInfoALL CBeidouDlg::ReadSMS(char *pSrc, SMSInfoALL smsb, int nTxRxFlag)
{
	SM_PARAM *pDst;
	pDst = &smsb.SMSInfo;
	smsb = nDecodePdu(pSrc,smsb,nTxRxFlag);//���Ž���
	int num = 0;
	m_StatBar->SetText("3G״̬: ��ȡ������,���Ժ�",2,0); //��������
	CTime curtime = CTime::GetCurrentTime();
	CString strTime = curtime.Format("%Y-%m-%d %H:%M:%S");
	
	if (nTxRxFlag==0||nTxRxFlag==1)
	{
//		GetDlgItem(IDC_TIME)->EnableWindow(TRUE);
//		m_strTime = pDst->TP_SCTS;
	} 
	else if (nTxRxFlag==2||nTxRxFlag==3)
	{
//		this->GetDlgItem(IDC_TIME)->SetWindowText("");
//		this->GetDlgItem(IDC_TIME)->EnableWindow(FALSE);
	}
//	m_target_number = pDst->SCA;
//	m_target_number = pDst->TPA;
//	call_in_number_3G = pDst->TPA;
	if (smsb.bIsLongSMS)
	{
		strLongSMSText = "";
		bool bIsEqual = FALSE;
		strLongSMSTextStore[pDst->index - 1] = pDst->TP_UD;//�ӵ�һ����ʼ�������0��ʼ��
		strLongSMSIDStore[pDst->index - 1] = smsb.strLongSMSID;
		
		
		for(int n=0; n<SMSSIZE_10; n++)
		{
			if(strLongSMSIDStore[n] != "")
			{
				
				if (strLongSMSIDStore[n] == smsb.strLongSMSID)
				{
					num++;
					bIsEqual = TRUE;
				} 
				else
				{
					bIsEqual = FALSE;
					break;
				}
				
			}
			if(n>smsb.nSMSTotalNum)
			{
				break;
			}
		}
		if (!bIsEqual)
		{
			for (int i=0;i<SMSSIZE_10;i++)
			{							
				strLongSMSTextStore[i] = "";
				strLongSMSIDStore[i]= "";
			}
		}

		//	strLongSMSText += pDst->TP_UD;
//		strLongSMSText +=	 "\r\n";
		for (int i=0;i<SMSSIZE_10;i++)
		{
			if(i>smsb.nSMSTotalNum)
			{
				break;
			}
			else
			{
				strLongSMSText += strLongSMSTextStore[i];
			//	strLongSMSText +=	 "\r\n";
			}
		}
		//��ʼ���볤���ź������ӣ��Զ�������ѯCSQ

			if(smsb.nSMSTotalNum == num)
			{//tts�������tts����
		//		TTSModule(strLongSMSText);
		//		m_StatBar->SetText("3G״̬������������",2,0); //��������
				strLongSMSText += "\r\n";
				bTimer48OnOff = FALSE;
				KillTimer(48);
				SetTimer(10,(QUERY_INTERVAL+QUERY_3G),NULL);//���Ŷ����ˣ��ٴ�3G��ѯ��ʱ��
				
				m_showmsg+=strTime;
				m_showmsg+="��";
				m_showmsg += pDst->TPA;
				m_showmsg+="�յ���Ϣ��\r\n";
				m_showmsg += strLongSMSText;
				UpdateData(FALSE);
//				TTSModule(strLongSMSText);
				ttsStruct.nPlayTimers = 1; //��ʱ��������
				ttsStruct.strTransmit = (LPCSTR)strLongSMSText;
//				pThread=AfxBeginThread(ThreadProc, &ttsStruct);
				pThread=AfxBeginThread(ThreadProc, this);
				m_StatBar->SetText("3G״̬������������",2,0); //��������
			}			
	} 
	else
	{
		strLongSMSText = pDst->TP_UD;
		strLongSMSText += "\r\n";
		bTimer48OnOff = FALSE;
		KillTimer(48);
		SetTimer(10,(QUERY_INTERVAL+QUERY_3G),NULL);//���Ŷ����ˣ��ٴ�3G��ѯ��ʱ��

		
		m_showmsg+=strTime;
		m_showmsg+="��";
		m_showmsg += pDst->TPA;
		m_showmsg+="�յ���Ϣ��\r\n";
		m_showmsg += strLongSMSText;
		UpdateData(FALSE);
//		TTSModule(strLongSMSText);
		ttsStruct.nPlayTimers = 1; //��ʱ��������
		ttsStruct.strTransmit = (LPCSTR)strLongSMSText;
//		pThread=AfxBeginThread(ThreadProc, &ttsStruct);
		pThread=AfxBeginThread(ThreadProc, this);
		m_StatBar->SetText("3G״̬������������",2,0); //��������
	}	


	CString strToSave;
	char pH[1200];
	CFile m_File;
	strToSave = "������:";
	strToSave += call_in_number_3G + "\r\n";	 
	strncpy(pH,(LPCTSTR)strToSave,strToSave.GetLength());	
	m_File.Open(".//ReceiveSMS.txt",CFile::modeCreate|CFile::modeReadWrite|CFile::modeNoTruncate|CFile::typeBinary|CFile::shareDenyNone,NULL); 
	m_File.SeekToEnd();
	m_File.Write(pH,sizeof(char)*strToSave.GetLength());
	if (!smsb.bIsLongSMS)
	{
		strToSave = "ʱ��:";
		strToSave += strTime + "\r\n";
		strncpy(pH,(LPCTSTR)strToSave,strToSave.GetLength());
		m_File.SeekToEnd();
		m_File.Write(pH,sizeof(char)*strToSave.GetLength());
	}	
	strToSave = "����:";
	strToSave += strLongSMSText;
	strncpy(pH,(LPCTSTR)strToSave,strToSave.GetLength());
	pH[strToSave.GetLength()-1] = '\0';
	m_File.SeekToEnd();
	m_File.Write(pH,sizeof(char)*strToSave.GetLength());
	m_File.Flush();
	m_File.Close();
	return smsb;
}

void CBeidouDlg::OnClearAll()
{
	if (!sms.bIsLongSMS || nSMSCount == 0)
		strLongSMSText =  "";
	bIsNewSMS = FALSE;
	nSMSCount = 0;
	bTimer48OnOff = FALSE;
	ChuanHaoID = "";
}

void CBeidouDlg::OnSelendokComboTelManualSelect() 
{
	// TODO: Add your control notification handler code here
	if((flag_3G_in_busy==0)&&(flag_3G_out_busy==0)&&(flag_PW_in_busy==0)&&(flag_PW_in_busy==0)){
		if (m_tel_manual_select.GetCurSel()==0)//�Զ�
		{
			tel_manual_select=0;//�Զ�ģʽ
			/*��ѯ���ߵ绰*/
			char lpOutBuffer[] = {'A','T','H','\r','\n'};//�����ϴ�ATHָ����йһ�
			CByteArray Array;
			Array.RemoveAll();    
			Array.SetSize(5);		
			
			for (int i=0; i<5; i++)
			{
				Array.SetAt(i,lpOutBuffer[i]);
			}
			if(m_comm_WT.GetPortOpen())
			{
				m_comm_WT.SetOutput(COleVariant(Array));//��������
			}
			/*��ѯ3G*/
			SendAtCmd("AT+CSQ",6);      //��ѯCSQ
			/*��ѯ���ǵ绰*/
			if (SerialPortOpenCloseFlag_ST==TRUE)//ֻ���ڴ����Ѿ���ʱ�Ų�ѯ
			{
				CString msg = "AT+RINFO\r\n";
				m_Port_ST.WriteToPort((LPCTSTR)msg);	//��������
			}

		} 
		else if (m_tel_manual_select.GetCurSel()==1)//���ߵ绰ģʽ�����3G���Ǿ������ã������ߵ绰��������ܷ��ѯ������ͬ��
		{
			char lpOutBuffer[] = {'A','T','H','\r','\n'};//�����ϴ�ATHָ����йһ�
			CByteArray Array;
			Array.RemoveAll();    
			Array.SetSize(5);		
			
			for (int i=0; i<5; i++)
			{
				Array.SetAt(i,lpOutBuffer[i]);
			}
			if(m_comm_WT.GetPortOpen())
			{
				m_comm_WT.SetOutput(COleVariant(Array));//��������
			}

//			KillTimer(4);
			tel_manual_select=1;//�ֶ�ģʽ
//			state_system[0]=0;
			state_system[1]=1;
			state_system[2]=1;

		}else if (m_tel_manual_select.GetCurSel()==2)//3G�绰
		{
			SendAtCmd("AT+CSQ",6);      //��ѯCSQ

//			KillTimer(10);
			tel_manual_select=1;//�ֶ�ģʽ
			state_system[0]=1;
//			state_system[1]=0;
			state_system[2]=1;

		}else if (m_tel_manual_select.GetCurSel()==3)//���ǵ绰
		{
			if (SerialPortOpenCloseFlag_ST==TRUE)//ֻ���ڴ����Ѿ���ʱ�Ų�ѯ
			{
				CString msg = "AT+RINFO\r\n";
				m_Port_ST.WriteToPort((LPCTSTR)msg);	//��ѯ���ǵ绰�Ķ�λ
			}

//			KillTimer(12);
			tel_manual_select=1;//�ֶ�ģʽ
			state_system[0]=1;
			state_system[1]=1;
	//		state_system[2]=0;

		}
		tel_mode_saved=m_tel_manual_select.GetCurSel();//������ȷ��ѡ��
	}else{
		m_tel_manual_select.SetCurSel(tel_mode_saved);
	}
}

void CBeidouDlg::SendLongSMS(strLongSMS  *structUse)
{
	int nChNum = 0;
	int nTotalNum = 0;
	char ch;	
	int n=0;
	n=structUse->nCount;
	n -= nLongSMSSendCount;
	CString strPart = "";//���ڽ�ȡ�ַ���
	CString order ="";
	CString totalOrder = "";
	funReturn d = {FALSE,0};//Ĭ��ΪFALSE,0�ַ�
	order.Format("%d",n+1);
	totalOrder.Format("%d",structUse->nCount);//���Ϊ9�� ������һ���Ϊ5
	if (1==nLongSMSSendCount)
	{
//		strPart = m_sendmsg.Mid(structUse->nStrSendLoc,m_sendmsg.GetLength()-nTotalNum);
		strPart = m_sendmsg.Mid(structUse->nStrSendLoc,m_sendmsg.GetLength()-structUse->nStrSendLoc);
		strCount countNum = Statistic(strPart);//�ַ�ͳ��
		nLongSMSSendCount --;
		d = cEncodeLongPDU(structUse->strDstNum,strPart,chPDU,structUse->bChineseFlag,countNum.nTotalLenth,order,totalOrder,structUse->nLongSMSID);//�ռ��ˣ�����  //Ĭ��������
		if (d.bToReturn)
		{
			if(d.nLenthToReturn <=200)
			{
				CString strTempc = "";
				SendAtCmd("AT+CMGF=0",21); //PDUģʽ
				strTempc.Format("%d",d.nLenthToReturn);
				SendAtCmd("AT+CMGS="+strTempc,22);
				nATCmdID = 23;
				SetTimer(46,1000,NULL);
				SetTimer(10,(QUERY_INTERVAL+QUERY_3G),NULL);
				longSMSStructINI();
			}
			else
			{
				longSMSStructINI();
				m_StatBar->SetText("3G״̬: ���ŷ��ͳ���",2,0); //��������
			}
			
		}
		else
		{
			longSMSStructINI();
			m_StatBar->SetText("3G״̬: ���ŷ���ʧ��,��������",2,0); //��������
		}
	}
	else
	{
			while ((nChNum<=65) && (nLongSMSSendCount != 1))
			{
				ch = m_sendmsg.GetAt(structUse->nStrSendLoc + nTotalNum);
				if(ch<0||ch>255)
				{
					nChNum++;
					nTotalNum+=2;
				}
				else 
				{
					nChNum++;
					nTotalNum++;
				}
			}
		
  		strPart = m_sendmsg.Mid(structUse->nStrSendLoc,nTotalNum);							
		structUse->nStrSendLoc += nTotalNum;
		strCount countNum = {2*nChNum-nTotalNum,nTotalNum-nChNum,nChNum};
	//	strCount countNum = Statistic(strPart);//�ַ�ͳ��
		nLongSMSSendCount --;
		//	d = cEncodeLongPDU(structUse.strDstNum,strPart,chPDU,structUse.bChineseFlag,countNum.nTotalLenth,order,totalOrder);//�ռ��ˣ�����  //Ĭ��������
		d = cEncodeLongPDU(structUse->strDstNum,strPart,chPDU,structUse->bChineseFlag,countNum.nTotalLenth,order,totalOrder,structUse->nLongSMSID);//�ռ��ˣ�����  //Ĭ��������
		if (d.bToReturn)
		{
			if (d.nLenthToReturn <= 200)
			{
				CString strTempd = "";
				SendAtCmd("AT+CMGF=0",21); //PDUģʽ
				strTempd.Format("%d",d.nLenthToReturn);
				SendAtCmd("AT+CMGS="+strTempd,22);
				nATCmdID = 23;
				SetTimer(46,1000,NULL);
				//��ʱ��47�ڴ��ڷ��ͳɹ�����
			} 
			else
			{
				longSMSStructINI();
				m_StatBar->SetText("3G״̬: ���ŷ��ͳ���",2,0); //��������
			}
		}
		else
		{
			longSMSStructINI();
			m_StatBar->SetText("3G״̬: ���ŷ���ʧ�ܣ���������",2,0); //��������
		}
	}
}

void CBeidouDlg::OnSelendokComboMsgManualSelect() 
{
	// TODO: Add your control notification handler code here
	if (m_msg_manual_select.GetCurSel()==0)//�Զ�
	{
		/*��ѯ3G*/
		SendAtCmd("AT+CSQ",6);      //��ѯCSQ
		/*��ѯ����*/
		CByteArray Array;//ѡ�񱱶�ģʽ�����̲�ѯ�����Ƿ����
		Array.RemoveAll();
		Array.SetSize(12);
		for (int i=0; i<12; i++)
		{
			Array.SetAt(i,frame_IC_check[i]);
		}		
		if(m_comm.GetPortOpen())
		{
			m_comm.SetOutput(COleVariant(Array));//��������
		}


		msg_manual_select=0;
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
	} 
	else if(m_msg_manual_select.GetCurSel()==1)//3G���ţ�������������·����Ϊ�����ã�����3G���ſ�����񣬿����ڲ�ѯ�����
	{
		SendAtCmd("AT+CSQ",6);      //��ѯCSQ
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);

		msg_manual_select=1;//�ֶ�
		state_system[4]=1;
	}
	else if(m_msg_manual_select.GetCurSel()==2)//��������
	{
		CByteArray Array;//ѡ�񱱶�ģʽ�����̲�ѯ�����Ƿ����
		Array.RemoveAll();
		Array.SetSize(12);
		for (int i=0; i<12; i++)
		{
			Array.SetAt(i,frame_IC_check[i]);
		}		
		if(m_comm.GetPortOpen())
		{
			m_comm.SetOutput(COleVariant(Array));//��������
		}

		msg_manual_select=1;//�ֶ�
		state_system[3]=1;
		if (flag_bd_is_busy==1)//������ʱ������ť�ٴ�ʧ�ܡ����ڣ�����������ͣ��л���3G���лر����������
		{
			GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
		}
	}
}

void CBeidouDlg::longSMSStructINI()
{
	longSMSStruct.strDstNum = "";
	longSMSStruct.strSMSText = "";
	longSMSStruct.bChineseFlag = FALSE;
	longSMSStruct.nTextLenth = 0;
	longSMSStruct.nCount = 0;
	longSMSStruct.nStrSendLoc = 0;
	longSMSStruct.nLongSMSID = 0;
}
	
BOOL TTSModule(CString str)
{

	BOOL bFlag = NULL;
	BOOL bToRadio = CheckString(str);//����ַ������Ƿ��������ַ�˫����
	if (bToRadio) //Ϊ��㲥
	{		//�Զ��㲥��������ʱֻ��һ����������
		bFlag = ttsSpeak(str);//tts,�������
	//	TTS_Realease();	
	}
	else
	{	
		bFlag = FALSE;
	}			
	return bFlag;
}

void CBeidouDlg::On123() 
{
	// TODO: Add your control notification handler code here
	strForTTS = "@@@@����Ӧ���㲥ϵͳ��Ϊʡ�����Լ��ֵ�����ְ�ܲ�����Ӧ���¼���ʵ��Ӧ��ָ�ӵ��Ⱥ�Ӧ���㲥��������ṩ����֧�֣����������û��ṩ���ߵ�㲥ģʽ���ֺ�Ԥ�����¼��澯����Ԯָ�������������ȹ㲥����";
//	strForTTS = "@@@@�򹷹����򹷵ÿ����˰����ǰɡ�С���ˣ�����������ʲ����ǿڶ��������������ֵܡ�";
	//	TTSModule(strTTS);	
	ttsStruct.nPlayTimers = 1; //��ʱ��������
	ttsStruct.strTransmit = (LPCSTR)strForTTS;
//	pThread=AfxBeginThread(ThreadProc, &ttsStruct);
	if (bIsTTSPlay == FALSE) //δ��ʼ����TTS
	{
		bIsTTSPlay = TRUE;
		pThread=AfxBeginThread(ThreadProc, this);
	} 
	else //TTS������
	{
		AfxMessageBox("TTS���ڽ����У����Ժ�����");
	}
	
}

UINT ThreadProc(LPVOID lpParameter)
{
	CBeidouDlg *pBeiDouDlg = (CBeidouDlg*)lpParameter;
/*	structForTTS Param;
	MoveMemory(&Param,lpParameter,sizeof(Param));	
	while ((Param.nPlayTimers != 0)&&(Param.strTransmit != ""))
	{
		CString str(Param.strTransmit);
		TTSModule(str);
		Param.nPlayTimers--;
		if (0==Param.nPlayTimers)
		{
			Param.strTransmit = "";
		}
	}*/
	while ((pBeiDouDlg->ttsStruct.nPlayTimers != 0)&&(pBeiDouDlg->ttsStruct.strTransmit != ""))
	{
		pBeiDouDlg->applay_broadcast(3);//TTS����򿪹㲥
		pBeiDouDlg->SetTimer(23,2000,NULL);//ÿ�������ѯTTS�Ƿ񲥷���ϣ����δ�꣬������ѯ���������������
		TTSModule(pBeiDouDlg->ttsStruct.strTransmit);
		pBeiDouDlg->ttsStruct.nPlayTimers--;
		if (0==pBeiDouDlg->ttsStruct.nPlayTimers)
		{
			pBeiDouDlg->ttsStruct.strTransmit = "";
			pBeiDouDlg->bIsTTSPlay = FALSE;
		}
	}
	return 0;
}

void CBeidouDlg::applay_broadcast(int n)//1:����㲥��2��ֹͣ�㲥��3:TTS����㲥;
{
	static bool is_busy=0;
	if(is_busy==0){
		is_busy=1;
	if (index_wakeup_times<200)
	{
		index_wakeup_times++;
		if ((index_wakeup_times==0x0d)||(index_wakeup_times==0x24))
		{
			index_wakeup_times++;
		}
	} 
	else
	{
		index_wakeup_times=0;
	}
	frame_board_ppp[5]=index_wakeup_times;
	frame_board_ppp[6]=n;
	frame_board_ppp[7]=XOR(frame_board_ppp,7);
	if ((frame_board_ppp[7]=='$')||(frame_board_ppp[7]==0x0d))
	{
		frame_board_ppp[7]++;//����������$��0x0d����ֵ��һ
	}
	frame_board_ppp[8]='\r';
	frame_board_ppp[9]='\n';
	CByteArray Array;
	Array.RemoveAll();
	Array.SetSize(8+2);
	
	for (int i=0;i<(8+2);i++)
	{
		Array.SetAt(i,frame_board_ppp[i]);
	}
	
	if(m_comm_YW.GetPortOpen())
	{
		m_comm_YW.SetOutput(COleVariant(Array));//��������
	}
	is_busy=0;
	}
}

void CBeidouDlg::OnButtonConnect_ST() 
{
	// TODO: Add your control notification handler code here
	if(SerialPortOpenCloseFlag_ST==FALSE){
		if(m_Port_ST.InitPort(this, m_DCom_ST, 9600,'N',8,1,EV_RXFLAG | EV_RXCHAR,512)){
			m_Port_ST.StartMonitoring();
			SerialPortOpenCloseFlag_ST=TRUE;
			m_ctrlIconOpenoff_ST.SetIcon(m_hIconRed);
			GetDlgItem(IDC_BUTTON_CONNECTST)->SetWindowText("�رմ���");
			m_StatBar->SetText("���ǵ绰״̬�������Ѵ�",1,0); //��������
			UpdateData();
			
			//���ǵ绰��ʼ��
			nATCmdFlags = 100;
			BISInitial_WX = TRUE;
			BINIfail_WX = TRUE;
			Send_AT_Command_ST("AT");
			SetTimer(59,5000,NULL);
			index_resent_data_frame=0;//����֡��֧���ش�����		
		}else{
			MessageBox("�޷������ǵ绰���ڣ������ԣ�");
			SerialPortOpenCloseFlag_ST=FALSE;
		}
	}else{
			m_Port_ST.ClosePort();//�رմ���
			
			SerialPortOpenCloseFlag_ST=FALSE;
			GetDlgItem(IDC_BUTTON_CONNECTST)->SetWindowText("�򿪴���");
			GetDlgItem(IDC_STATIC_BOARDCONNECT_ST)->SetWindowText(" ���ǵ绰�ѹرգ�");
			UpdateData();

			m_ctrlIconOpenoff_ST.SetIcon(m_hIconOff);
			m_board_led_ST.SetIcon(m_hIconOff);
			m_st_r.SetIcon(m_hIconOff);
			state_system[2]=1;//���ǵ绰���ڹرգ�״̬��Ϊ������
			KillTimer(20);//ֹͣ���ڲ�ѯ
			KillTimer(55);//����errorҲֹͣ��ʼ��
			KillTimer(59);//ֹͣ��ʼ��
			KillTimer(50);//ֹͣ��ʼ��
	}
}

void CBeidouDlg::OnSelendokComboComselect_ST() 
{
	// TODO: Add your control notification handler code here
	m_DCom_ST=m_Com_ST.GetCurSel()+1;
	UpdateData();
	
	CString strTemp;
	strTemp.Format(_T("%d"),m_DCom_ST-1);
	::WritePrivateProfileString("ConfigInfo","com_ST",strTemp,".\\config_phonemessage.ini");
	
	strTemp.Format(_T("%d"),m_DCom_ST);
	::WritePrivateProfileString("ConfigInfo","com_r_ST",strTemp,".\\config_phonemessage.ini");	
}

LONG CBeidouDlg::OnCommunication(WPARAM ch, LPARAM port)
{ 
	UpdateData(TRUE);
    COleSafeArray safearray_inp;
	int nIndex = 0;//����
	static bool ST_CSQ_avilable=0;//���ǵ绰CSQ�Ƿ����źš�0�����źţ�1�����źţ�
    CString strtemp;
	strtemp.Format("%c",ch);
		 
	m_FKXX+=strtemp;		 
	strdata += strtemp;

	if(nATCmdFlags == 100)
	{
		if(strdata.Find("OK")!=-1)
		{
			KillTimer(59);
			SetTimer(50,1000,NULL);	
			nATCmdFlags = 1;
			strdata = "";
		}
	}

	if ((strdata.Find("OK")!=-1)&&(strdata.GetLength()<=3))
	{
		if ((bISCallProcess_WX == TRUE) && (bISCaller_WX == TRUE))
		{
			ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);//��ʼ¼��
			bIsRecording = TRUE;
		}
			strdata="";
	}
	else if (strdata.Find("RING")!=-1) //����ò�����
	{	
		KillTimer(20);//ֹͣ���ڲ�ѯ
		flag_ST_in_busy=1;
		int i = strdata.Find("RING");
		strdata = strdata.Mid(i,strdata.GetLength()-i);
		//i = strdata.Find("OK");
		if (i!=-1)
		{
			bISCaller_WX = FALSE;
			if (bISCallProcess_WX == FALSE)
			{
				bISCallProcess_WX = TRUE;
				this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("����");	
			}
			strdata = "";
		}
	}
	else if (strdata.Find("CLIP:") != -1)//��������
	{
		KillTimer(20);//ֹͣ���ڲ�ѯ
		int i = strtemp.Find("\"");
		strdata = strdata.Mid(i,strdata.GetLength()-i);
		//i = strdata.Find("OK");
		if (i!=-1&&comingflag==0)
		{
			m_target_number="";
			comingflag=1;
		}
		else if(i!=-1&&comingflag==1)
		{	
			comingflag=0;
			UpdateData(FALSE);
			strdata="";
		}
		if (comingflag==1&&i==-1)
		{
			 UpdateData(TRUE);
			 m_target_number += strtemp;
			 UpdateData(FALSE);
		}
	}
	else if (strdata.Find("CSQ")!=-1)
	{
		CString str3;
		int	n=strtemp.FindOneOf(":");
		int	k=strtemp.FindOneOf(",");
		str3 = strtemp.Mid(n+2,k-(n+2));
		int j = 0;
		j = atoi(str3);	
			if ((j<=24)&&(j>0))
			{
				GetDlgItem(IDC_ST_RSSI)->SetWindowText("���ǵ绰�ź�: ��");
				ST_CSQ_avilable=1;//����
			} 
			else if ((j>=24)&&(j<=28))
			{
				GetDlgItem(IDC_ST_RSSI)->SetWindowText("���ǵ绰�ź�: ��");
				ST_CSQ_avilable=1;//����
			}
			else
			{
				if (strtemp.Find("99,")!=-1)
				{
					GetDlgItem(IDC_ST_RSSI)->SetWindowText("���ǵ绰�ź�δ֪��");
					ST_CSQ_avilable=0;//������
				}
				else
				{
					GetDlgItem(IDC_ST_RSSI)->SetWindowText("���ǵ绰�ź�: ��");
					ST_CSQ_avilable=1;//����
				}
			}
		strdata="";
		UpdateData(false);
	}
	else if (strdata.Find("RINFO")!=-1)
	{
		int i = strdata.Find("RINFO");
		strdata = strdata.Mid(i,strdata.GetLength()-i);
		i = strdata.Find("2");
		if (i!=-1)
		{
			timer_board_disconnect_times_ST=0;
			GetDlgItem(IDC_ST_POSITION)->SetWindowText("���ǵ绰��λ���ɹ�");
			if (ST_CSQ_avilable==1)//CSQ���ź�ʱ
			{
				state_system[2]=0;//������ǵ绰����
			}else{
				state_system[2]=1;//CSQ���ź�ʱ��������ǵ绰������
			}
			
		} 
		else
		{
			GetDlgItem(IDC_ST_POSITION)->SetWindowText("���ǵ绰��λ��ʧ��");
			state_system[2]=1;//������ǵ绰������
		}
		strdata="";
	}
	else if (strdata.Find("+GPSTRACKD")!=-1)
	{ //�Ȼ�OK
		int i = strdata.Find("78");
		int j = strdata.Find("48");
		int k = strdata.Find("79");
		if (i!=-1)
		{
			m_StatBar->SetText("���ǵ绰״̬��GPS�Ѷ�λ",2,0);
			strdata=""; 
		}
		else if (k!=-1)
		{
			m_StatBar->SetText("���ǵ绰״̬��GPS�Ѷ�λ",2,0);
			strdata="";
		}
		else if(j!=-1)
		{
			m_StatBar->SetText("���ǵ绰״̬��GPSδ��λ",2,0);
			strdata="";	
		}
	}
	else if (strdata.Find("+RCIPH:")!=-1)
	{//����״̬�����Ѳ���
		m_StatBar->SetText("���ǵ绰״̬��������......",2,0);
		strdata="";
	}
	else if(strdata.Find("ERROR")!=-1)//��ʼ�����ش���
	{	
		if (BISInitial_WX == TRUE)
		{
			BINIfail_WX = FALSE;
			strdata = "";
		} 
		else
		{
			int i = strdata.Find("OK");
			if (i!=-1)
			{
				CString stra;
				stra.Format("%d",nATCmdFlags);
				AfxMessageBox("AT command "+stra+" error.");
				strdata="";
			}
		}
	}
	else if((strdata.Find("OK")!=-1)&&(10 == nATCmdFlags))
	{
		nATCmdFlags = 0;
		BISInitial_WX = FALSE;
		BINIfail_WX = FALSE;
		strdata = "";
	}
	else if (strdata.Find("CME ERROR")!=-1)//���ﻹҪ�ģ���������������������������
	{
		CString str;
		this->GetDlgItemText(IDC_BUTTON_CALL,str);
		if (str=="�һ�")
		{
			if((flag_ST_in_busy==1)||(flag_ST_out_busy==1))OnButtonCall();
		}
		m_FKXX += strdata + "\r\n";
		strdata ="";
	}
	else if((strdata.Find("NO CARRIER")!=-1)||(strdata.Find("BUSY")!=-1)||(strdata.Find("NO ANSWER")!=-1))//�һ�
	{
		CString str;
		this->GetDlgItemText(IDC_BUTTON_CALL,str);
		if (str=="�һ�")
		{
			if((flag_ST_in_busy==1)||(flag_ST_out_busy==1))OnButtonCall();
		}

		if (bIsRecording)//δ�رվ͹ر�
		{
			bIsRecording = FALSE;
			//�ر�¼��
			HWND hWnd = ::FindWindow(NULL, _T("record11"));//Ҫ�ģ�����record11
			if (NULL != hWnd) {
				::SendMessage(hWnd, WM_CLOSE, 0, 0);
			}
		}
		strdata="";	
		SetTimer(20,(QUERY_INTERVAL+QUERY_ST),NULL);
	}
	UpdateData(FALSE); //���±༭������	
	return 0;
}

void CBeidouDlg::OnGPS() 
{
	// TODO: Add your control notification handler code here
	if (SerialPortOpenCloseFlag_ST==TRUE)//ֻ���ڴ����Ѿ���ʱ�Ų�ѯ
	{
		CString msg = "AT+GPSTRACK=1\r\n";
		m_Port_ST.WriteToPort((LPCTSTR)msg);	//��������
	}
}

void CBeidouDlg::On_CSQ_ST() 
{
	// TODO: Add your control notification handler code here
	if (SerialPortOpenCloseFlag_ST==TRUE)//ֻ���ڴ����Ѿ���ʱ�Ų�ѯ
	{
		CString msg = "AT+CSQ\r\n";
		m_Port_ST.WriteToPort((LPCTSTR)msg);	//��������
	}
}

void CBeidouDlg::On_Position_ST() 
{
	// TODO: Add your control notification handler code here
	if (SerialPortOpenCloseFlag_ST==TRUE)//ֻ���ڴ����Ѿ���ʱ�Ų�ѯ
	{
		CString msg = "AT+RINFO\r\n";
		m_Port_ST.WriteToPort((LPCTSTR)msg);	//��������
	}
}

void CBeidouDlg::Send_AT_Command_ST(CString msg)
{
	if (SerialPortOpenCloseFlag_ST==TRUE)
	{
		msg+="\r\n";
		m_FKXX += msg;
		int nLentx = 0;
		m_Port_ST.WriteToPort((LPCTSTR)msg);	//��������
		UpdateData(FALSE);
	}
	
}
