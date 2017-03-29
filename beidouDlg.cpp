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
TIMER1,2 使能发送消息按钮及刷新北斗进度条
TIMER3 有线电话摘机拨号对用户透明

TIMER4,5 有线电话
TIMER6,7 运维
TIMER8,9 北斗
TIMER10,11 3G
TIMER20,21,22 卫星电话
TIMER14 定期检查反馈信息的文本长度，超出时清屏
TIMER33,34 自动应答
*/
/*************北斗******************************/
unsigned char frame_IC_check[12]={0x24,0x49,0x43,0x4A,0x43,0x00,0x0C,0x00,0x00,0x00,0x01,0x2A};//IC查询
unsigned char frame_SYS_check[11]={0x24,0x53,0x74,0x73,0x5F,0x00,0x0B};//系统检测
unsigned char frame_POWER_check[12]={0x24,0x47,0x4C,0x4A,0x43,0x00,0x0C,0x00,0x00,0x00,0x01,0x2B};//功率查询
unsigned char frame_MSG_check[250]={0x24,0x54,0x58,0x53,0x51};//通信申请

unsigned char frame_receive[received_frame_size][2000]={0};//北斗接收帧缓冲

/*************有线电话******************************/
unsigned char frame_receive_WT[2000]={0};

/***********运维板***************/
unsigned char frame_board_check_YW[7+2]={'$','r','e','y','_'};//运维连接检测帧
unsigned char frame_board_reset_YW[8+2]={'$','r','s','e','_'};//运维复位帧
unsigned char frame_board_sound_YW[8+2]={'$','s','w','h','_'};//运维切换音频开关申请帧
unsigned char frame_board_connect_YW[7+2]={'$','p','p','p','_'};//运维上位机两个软件相互查询帧
unsigned char frame_board_scan_YW[7+2]={'$','s','c','a','_'};//运维频谱扫描，继电器控制帧
unsigned char frame_board_ppp[8+2]={'$','p','p','p','_'};//运维打开/关闭广播申请帧帧
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
	for (int i=0;i<(sizeof(state_system)/sizeof(state_system[0]));i++)//间接获取数组长度，增加健壮性
	{
		state_system[i]=1;//初始化时，默认全部不可用。系统功能模块可用状态标志位。
	}
	tel_manual_select=0;//初始化
	msg_manual_select=0;//初始化

	m_hIconRed  = AfxGetApp()->LoadIcon(IDI_ICON_RED);
	m_hIconOff	= AfxGetApp()->LoadIcon(IDI_ICON_OFF);
	GetDlgItem(IDC_COMBO_STOPBITS2)->SetWindowText(_T("代码"));

	GetDlgItem(IDC_BUTTON_ICCHECK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3_POWERCHECK)->EnableWindow(FALSE);
//	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
//	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(FALSE);

//	GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(FALSE);
/****************************状态栏**************************************/
	m_StatBar=new CStatusBarCtrl;//状态栏
	RECT m_Rect; 
	GetClientRect(&m_Rect); //获取对话框的矩形区域
	m_Rect.top=m_Rect.bottom-20; //设置状态栏的矩形区域
	m_StatBar->Create(WS_BORDER|WS_VISIBLE|CBRS_BOTTOM,m_Rect,this,3); 
	
	int nParts[5]= {220,440,660,880,-1}; //分割尺寸385
	m_StatBar->SetParts(5, nParts); //分割状态栏
	m_StatBar->SetText("有线电话：未连接",0,0); //第一个分栏加入"这是第一个指示器"
	m_StatBar->SetText("卫星电话：未连接",1,0); //以下类似
	m_StatBar->SetText("3G：未连接",2,0); //以下类似
										/*也可使用以下方式加入指示器文字
										m_StatBar.SetPaneText(0,"这是第一个指示器",0);
										其他操作：m_StatBar->SetIcon(3,SetIcon(AfxGetApp()->LoadIcon(IDI_ICON3),FALSE));
										//在第四个分栏中加入ID为IDI_ICON3的图标*/
//	m_StatBar->SetIcon(2,SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_OFF),FALSE));
	m_StatBar->SetText("北斗：未连接",3,0); 
	m_StatBar->SetText("运维板：未连接",4,0); 
	m_StatBar->ShowWindow(SW_SHOW);
	
/********************1、北斗串口配置***********************************/	
	m_comm.SetCommPort(1); //选择com1
	m_comm.SetInputMode(1); //输入方式为二进制方式
	m_comm.SetInBufferSize(1024); //设置输入缓冲区大小
	m_comm.SetOutBufferSize(10240); //设置输出缓冲区大小
	m_comm.SetSettings("115200,n,8,1"); //波特率115200，无校验，8个数据位，1个停止位	 
	m_comm.SetRThreshold(1); //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件
	m_comm.SetInputLen(0); //设置当前接收区数据长度为0
	//	 m_comm.GetInput();    //先预读缓冲区以清除残留数据
	flag_bd_is_busy=0;//初始化
/********************2、有线电话串口配置***********************************/	
	m_comm_WT.SetCommPort(2); //选择com2
	m_comm_WT.SetInputMode(1); //输入方式为二进制方式
	m_comm_WT.SetInBufferSize(1024); //设置输入缓冲区大小
	m_comm_WT.SetOutBufferSize(10240); //设置输出缓冲区大小
	m_comm_WT.SetSettings("1200,n,8,1"); //波特率1200，无校验，8个数据位，1个停止位	 
	m_comm_WT.SetRThreshold(1); //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件
	m_comm_WT.SetInputLen(0); //设置当前接收区数据长度为0
	//	 m_comm_WT.GetInput();    //先预读缓冲区以清除残留数据
	GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(FALSE);//~1~

	send_string="";//传号字符串清空，准备接收传号
/********************3、运维串口配置***********************************/	
	m_comm_YW.SetCommPort(3); //选择com3
	m_comm_YW.SetInputMode(1); //输入方式为二进制方式
	m_comm_YW.SetInBufferSize(1024); //设置输入缓冲区大小
	m_comm_YW.SetOutBufferSize(10240); //设置输出缓冲区大小
	m_comm_YW.SetSettings("115200,n,8,1"); //波特率1200，无校验，8个数据位，1个停止位	 
	m_comm_YW.SetRThreshold(1); //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件
	m_comm_YW.SetInputLen(0); //设置当前接收区数据长度为0
	//	 m_comm_WT.GetInput();    //先预读缓冲区以清除残留数据
/********************4、3G串口配置***********************************/	
	m_comm_3G.SetCommPort(4); //选择com4
	m_comm_3G.SetInputMode(1); //输入方式为二进制方式
	m_comm_3G.SetInBufferSize(1024); //设置输入缓冲区大小
	m_comm_3G.SetOutBufferSize(10240); //设置输出缓冲区大小
	m_comm_3G.SetSettings("115200,n,8,1"); //波特率1200，无校验，8个数据位，1个停止位	 
	m_comm_3G.SetRThreshold(1); //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件
	m_comm_3G.SetInputLen(0); //设置当前接收区数据长度为0
	//	 m_comm_3G.GetInput();    //先预读缓冲区以清除残留数据
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
	/******************5、卫星电话串口***************************/
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
		frame_flag[j]=0;//没有帧等待处理
		frame_len[j]=0;//缓冲帧的下标
	}
	frame_index=0;//优先使用第零帧
	framelen=0;
	comm_init=0;//等待初始化

	m_sata1.SetRange(0,4);
	m_sata2.SetRange(0,4);
	m_sata3.SetRange(0,4);
	m_sata1.SetStep(1);//设置进度条的当前位置
    m_sata1.SetPos(0);
	m_sata2.SetStep(1);//设置进度条的当前位置
    m_sata2.SetPos(0);
	m_sata3.SetStep(1);//设置进度条的当前位置
	/****************************缩短界面**********************************/
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

	/**************************INI配置*******************************/
	CFileFind finder;   //查找是否存在ini文件，若不存在，则生成一个新的默认设置的ini文件，这样就保证了我们更改后的设置每次都可用
	BOOL ifFind = finder.FindFile(_T(".\\config_phonemessage.ini"));
	if(!ifFind)
	{
		/**********串口配置**********************/
		::WritePrivateProfileString("ConfigInfo","com","4",".\\config_phonemessage.ini");//串口配置选项组
		::WritePrivateProfileString("ConfigInfo","com_WT","1",".\\config_phonemessage.ini");//串口配置选项组
		::WritePrivateProfileString("ConfigInfo","com_YW","2",".\\config_phonemessage.ini");//串口配置选项组
		::WritePrivateProfileString("ConfigInfo","com_3G","3",".\\config_phonemessage.ini");//串口配置选项组
		::WritePrivateProfileString("ConfigInfo","com_ST","0",".\\config_phonemessage.ini");//串口配置选项组
// 		::WritePrivateProfileString("ConfigInfo","parity","0",".\\config_phonemessage.ini");
// 		::WritePrivateProfileString("ConfigInfo","databits","0",".\\config_phonemessage.ini");
// 		::WritePrivateProfileString("ConfigInfo","speed","5",".\\config_phonemessage.ini");
// 		::WritePrivateProfileString("ConfigInfo","stopbits","0",".\\config_phonemessage.ini");
		
		::WritePrivateProfileString("ConfigInfo","com_r","5",".\\config_phonemessage.ini");//串口配置数值组
		::WritePrivateProfileString("ConfigInfo","com_r_WT","2",".\\config_phonemessage.ini");//串口配置选项组
		::WritePrivateProfileString("ConfigInfo","com_r_YW","3",".\\config_phonemessage.ini");//串口配置选项组
		::WritePrivateProfileString("ConfigInfo","com_r_3G","4",".\\config_phonemessage.ini");//串口配置选项组
		::WritePrivateProfileString("ConfigInfo","com_r_ST","1",".\\config_phonemessage.ini");//串口配置选项组
// 		::WritePrivateProfileString("ConfigInfo","parity_r","N",".\\config_phonemessage.ini");
// 		::WritePrivateProfileString("ConfigInfo","databits_r","8",".\\config_phonemessage.ini");
// 		::WritePrivateProfileString("ConfigInfo","speed_r","115200",".\\config_phonemessage.ini");
// 		::WritePrivateProfileString("ConfigInfo","stopbits_r","1",".\\config_phonemessage.ini");		
	}

	/**********串口配置**********************/
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
	((CComboBox*)GetDlgItem(IDC_COMBO_COMSELECT))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//设置第n行内容为显示的内容。
	
	GetPrivateProfileString("ConfigInfo","com_WT","1",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_COMSELECT_WT))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//设置第n行内容为显示的内容。

	GetPrivateProfileString("ConfigInfo","com_YW","2",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_COMSELECT_YW))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//设置第n行内容为显示的内容。

	GetPrivateProfileString("ConfigInfo","com_3G","3",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_COMSELECT_3G))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//设置第n行内容为显示的内容。

	GetPrivateProfileString("ConfigInfo","com_ST","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_COMSELECT_ST))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//设置第n行内容为显示的内容。

	//电话、短信选路模式
	((CComboBox*)GetDlgItem(IDC_COMBO_TEL_MANUAL_SELECT))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_MSG_MANUAL_SELECT))->SetCurSel(0);
	tel_mode_saved=0;

// 	GetPrivateProfileString("ConfigInfo","parity","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
// 	strBufferReadConfig.ReleaseBuffer();
// 	strtmpReadConfig+=","+strBufferReadConfig;
// 	((CComboBox*)GetDlgItem(IDC_COMBO_PARITY))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//设置第n行内容为显示的内容。
// 	
// 	GetPrivateProfileString("ConfigInfo","databits","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
// 	strBufferReadConfig.ReleaseBuffer();
// 	strtmpReadConfig+=","+strBufferReadConfig;
// 	((CComboBox*)GetDlgItem(IDC_COMBO_DATABITS))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//设置第n行内容为显示的内容。
// 	
// 	GetPrivateProfileString("ConfigInfo","speed","5",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
// 	strBufferReadConfig.ReleaseBuffer();
// 	strtmpReadConfig+=","+strBufferReadConfig;
// 	((CComboBox*)GetDlgItem(IDC_COMBO_SPEED))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//设置第n行内容为显示的内容。
// 	
// 	GetPrivateProfileString("ConfigInfo","stopbits","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
// 	strBufferReadConfig.ReleaseBuffer();
// 	strtmpReadConfig+=","+strBufferReadConfig;
// 	((CComboBox*)GetDlgItem(IDC_COMBO_STOPBITS))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//设置第n行内容为显示的内容。
	
	UpdateData(FALSE);
	/*********************************************************************/
	switch_state=0;//打电话
	WT_state=0;//电话机状态，初始化置为空闲
	flag_PW_in_busy=0;//初始值
	flag_PW_out_busy=0;//初始值
	timer_board_disconnect_times_WT=0;
	timer_board_disconnect_times_YW=0;
	timer_board_disconnect_times_BD=0;
	timer_board_disconnect_times_3G=0;

	SerialPortOpenCloseFlag=FALSE;//默认关闭串口
	SerialPortOpenCloseFlag_WT=FALSE;//默认关闭有线电话的串口
	SerialPortOpenCloseFlag_YW=FALSE;//默认关闭运维的串口
	SerialPortOpenCloseFlag_3G=FALSE;//默认关闭3G电话的串口
	SerialPortOpenCloseFlag_ST=FALSE;//默认关闭卫星电话的串口

	SetTimer(12,5000,NULL);//定期截断反馈信息编辑框中的内容
	
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
	BYTE rxdata[2048]; //设置BYTE数组
	static bool frameplus=0;//0:$为第一位；1：$在串的中间
	CString strDisp="",strTmp="";

	if((m_comm.GetCommEvent()==2)) //事件值为2表示接收缓冲区内有字符
	{	
		Sleep(100);//一帧较长，为避免接收半帧就触发响应函数，造成未收完的判断而添加大量保护代码。
		//此处可用一种变通的方式，方法一，在触发事件时，延迟一下再读数据，保证每帧都是完整的。I'm very smart，哈哈
		//方法二：下面的多帧缓冲区方式，不管几帧来，都能收下，并以$区分开存储，收完完整一帧再处理，这种方法保证不会漏帧

		variant_inp=m_comm.GetInput(); //读缓冲区
		safearray_inp=variant_inp;  //VARIANT型变量转换为ColeSafeArray型变量
		len=safearray_inp.GetOneDimSize(); //得到有效数据长度
		for(k=0;k<len;k++)
		{
			safearray_inp.GetElement(&k,rxdata+k);//转换为BYTE型数组
		}

//			AfxMessageBox("OK",MB_OK,0);
//			frame=frame_len[frame_index];
//			frame_len[frame_index]=0;


		for(k=0;k<len;k++)//将数组转化为CString类型
		{
			BYTE bt=*(char*)(rxdata+k);    //字符型
			if (bt=='$')
			{
				if (rxdata[0]=='$')
				{
					frameplus=0;//0:'$'为第一位；1：'$'在串的中间
				} 
				else//'$'出现在流的中间位置
				{
					frameplus=1;
					frame_index++;//上一条已接收，需要处理
				}

				if (frame_index!=0)
				{
	//				frame_index++;//新的一帧到来
					if(frameplus==1)//只有一次来两帧及以上时才进来，并且将确定是完整收到的帧进行处理。
					{
						frame_flag[frame_index-1]=1;//上一个缓冲帧标记为待处理状态。
						frame_len[frame_index-1]=framelen;//上一个缓冲帧长度记录下来。
						framelen=0;
						for (short i=0;i<received_frame_size;i++)
						{
							if (frame_flag[i]==1)
							{
								decodeheads (frame_receive[i]);
								frame_flag[i]=0;//标记为未使用
							}
						}
						
					}
					if(frame_index==received_frame_size)frame_index=0;//缓存数组，用完一遍后，再从第一条开始用。默认不会一次把缓冲全部用完
				}
				
			}
			frame_receive[frame_index][framelen]=bt;
			framelen++;
//			strTmp.Format("%02x ",bt);    //将字符送入临时变量strtemp存放
//			strDisp+=strTmp;  //加入接收编辑框对应字符串
			
		}//本次缓冲区中的内容全部取出，并存储起来

		if (framelen==(unsigned char)(frame_receive[frame_index][5]*256+frame_receive[frame_index][6]))//对本次接收的缓冲帧的最后一条（只有一条或多条中的最后一条），如果接收完毕，进行处理
		{
			//帧接收完成
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
					frame_flag[i]=0;//标记为未使用
				}
			}
			if(frame_index==received_frame_size) frame_index=0;
			
		}
//		m_showmsg+=strDisp;
//		m_showmsg+="\r\n";
		UpdateData(FALSE);
	}//end of 串口事件	
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

		//以下是串口的初始化配置
		if(m_comm.GetPortOpen())//打开端口前的检测，先关，再开
			MessageBox("串口无法打开");
//			m_comm.SetPortOpen(FALSE);	//	
		m_comm.SetCommPort(m_DCom); //选择端口，默认是com1
		m_comm.SetSettings((LPSTR)(LPCTSTR)string1); //波特率256000，无校验，8个数据位，1个停止位
		if(!m_comm.GetPortOpen())
		{			
			m_comm.SetPortOpen(TRUE);//打开串口
			GetDlgItem(IDC_OPENCLOSEPORT)->SetWindowText("关闭串口");
			m_ctrlIconOpenoff.SetIcon(m_hIconRed);
			m_StatBar->SetText("北斗：串口已连接",3,0);
			GetDlgItem(IDC_BUTTON_ICCHECK)->EnableWindow(TRUE);
//			GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(TRUE);
//			GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON3_POWERCHECK)->EnableWindow(TRUE);
			
			Sleep(20);//等待串口稳定，再发送北斗的查询消息
			//打开串口后进行IC查询，取出卡号
			CByteArray Array;
			Array.RemoveAll();
            Array.SetSize(12);
			for (int i=0; i<12; i++)
			{
				Array.SetAt(i,frame_IC_check[i]);
			}
			
			
			if(m_comm.GetPortOpen())
			{
				m_comm.SetOutput(COleVariant(Array));//发送数据
			}
			SetTimer(8,(QUERY_INTERVAL+QUERY_BD),NULL);//自动模式下才打开定时器
		}
		else
			MessageBox("can not open serial port");	 
	}
	else
	{
		SerialPortOpenCloseFlag=FALSE;
		GetDlgItem(IDC_OPENCLOSEPORT)->SetWindowText("打开串口");
		m_ctrlIconOpenoff.SetIcon(m_hIconOff);
		m_comm.SetPortOpen(FALSE);//关闭串口
		m_StatBar->SetText("北斗：串口已断开",3,0);
		state_system[4]=1;//北斗串口关闭
		m_board_led_BD.SetIcon(m_hIconOff);
		m_bd_r.SetIcon(m_hIconOff);
		GetDlgItem(IDC_STATIC_BOARDCONNECT_BD)->SetWindowText(" 北斗已断开！");
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
// 	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //生成指向应用程序类的指针
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
// 	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //生成指向应用程序类的指针
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
// 	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //生成指向应用程序类的指针
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
// 	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //生成指向应用程序类的指针
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
	if (state_system[3]==0)//发短信时:3G可用，先用3G
	{
		if(bTimer48OnOff == FALSE)
		{
			bTimer48OnOff = TRUE;
			SetTimer(48,120*1000,NULL);//短信解码两分钟没回，开启CSQ
		}
		KillTimer(10);
		if((flag_3G_in_busy==1)||(flag_3G_out_busy==1))this->GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(FALSE);
		UpdateData(TRUE);  		
		BOOL bIsSendSMS = FALSE;	
		BOOL bChinese = FALSE;
		CString stra = "";
		bool bIsSendLongSMS = FALSE;
		
		strCount countNum;
		countNum = Statistic(m_sendmsg);//字符统计
		nEnglishLenth = countNum.nEnglishLenth;
		nChineseLenth = countNum.nChineseLenth;
		nTotalLenth = countNum.nTotalLenth;
		stra.Format("英: %d,中: %d\r\n共计: %d",nEnglishLenth , nChineseLenth , nTotalLenth);
		m_StatBar->SetText("3G状态: "+stra,2,0); //以下类似
		if (nChineseLenth == 0)
			bChinese = FALSE;	
		else
			bChinese = TRUE;
		
		if (bChinese)
		{	//判断中文字符是否超出 16bit最多字符数70个  //还得计算中英文字符的个数		
			if((nChineseLenth+nEnglishLenth) <= 70 )
				bIsSendSMS = TRUE;
			else if (((nChineseLenth+nEnglishLenth) > 70)&&((nChineseLenth+nEnglishLenth) <= 330 ) ) //先实现3条
			{
				bIsSendLongSMS = TRUE;
				nLongSMSSendCount = nTotalLenth/66 + 1;
			}
		} 
		else
		{	//判断英文字符是否超出 7bit最多字符数160个
			if(nEnglishLenth <= 160 )
				bIsSendSMS = TRUE;
		}
		
		if ((bIsSendSMS == TRUE)||(bIsSendLongSMS == TRUE))
		{	
			funReturn d = {FALSE,0};//默认为FALSE,0字符
			memset(chPDU,0,512);
			if (bIsSendSMS == TRUE)
			{
		//		d = cEncodePDU(m_strSendPhoneNum,m_strReceicePhoneNum,m_strSMSText,chPDU,bChinese,nTotalLenth);//发件人，收件人，内容
				d = cEncodePDU(m_target_number,m_sendmsg,chPDU,bChinese,nTotalLenth);//00，收件人，内容  //默认是中文
				if (d.bToReturn)
				{
					if(d.nLenthToReturn <=200)
					{
						CString strTemp = "";
						SendAtCmd("AT+CMGF=0",21); //PDU模式
						strTemp.Format("%d",d.nLenthToReturn);
						SendAtCmd("AT+CMGS="+strTemp,22);
						nATCmdID = 23;
						SetTimer(46,1000,NULL);
						bTimer48OnOff = FALSE;
						KillTimer(48);
						SetTimer(10,(QUERY_INTERVAL+QUERY_3G),NULL);//短信读完了，再打开3G查询定时器
					}
					else
					{
						m_StatBar->SetText("3G状态: 短信发送出错",2,0); //以下类似
					}
				}
				else
					m_StatBar->SetText("3G状态: 短信发送失败，检查后重试",2,0); //以下类似
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
				SetTimer(48,60*1000,NULL);// 防止发短信出错，卡在等PDU数据
			}
		} 
		else
			AfxMessageBox("短信字符数过多!字符数中文输入最多330，英文输入最多160");	
	}
	else if(state_system[4]==0){//发短信时:3G不可用，再用北斗
		KillTimer(8);
		UpdateData();
		if (m_target_number.GetLength()!=6)//(m_otherID==0)
		{
			AfxMessageBox("现在使用北斗通信，对方卡号请输入6位数字。");
			return;
		}
		if(m_sendmsg!="")
		{
			int timer_range=(m_category>1)?m_category:60;//只有当北斗的服务频度获取到（>0）时，才根据其设置进度条，否则使用默认值60	
			m_timer.SetRange(0,timer_range);
			m_timer.SetPos(0);
			m_timer.SetStep(1);//设置进度条的当前位置

			unsigned char hexdata[250];
			int len=strHex(m_sendmsg,hexdata,250);
			if (len>72)
			{	
				CString temp;
				temp.Format("%d",len-72);
				temp="北斗报文内容超出"+temp;
				temp+="个字符，请修改后重试！";
				AfxMessageBox(temp);
				return;
			}
			CByteArray Array;
			Array.RemoveAll();
			int total_len=len+19;
			Array.SetSize(total_len);

			frame_MSG_check[5]=total_len/256;//数据包总长度
			frame_MSG_check[6]=total_len%256;
			
			frame_MSG_check[10]=0x46;//默认使用“混编”方式发送数据

			int target_id=_ttoi(m_target_number);
			frame_MSG_check[11]=(unsigned char)(target_id/pow(2, 16));//对方ID
			frame_MSG_check[12]=(unsigned char)(target_id/pow(2, 8));
			frame_MSG_check[13]=(unsigned char)(target_id%256);

			frame_MSG_check[14]=8*len/256;//电文长度
			frame_MSG_check[15]=8*len%256+8;

			frame_MSG_check[16]=0;//是否应答
			frame_MSG_check[17]=0x4A;//混编时此处需要填入0x4A

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
				m_comm.SetOutput(COleVariant(Array));//发送数据
			}
			SetTimer(1,timer_range*1000,NULL);//61秒后使能
			SetTimer(2,1000,NULL);//每秒中断一次，设置一次进度条的值
			GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
			flag_bd_is_busy=1;//北斗发送中，忙碌啊
		}else{
			AfxMessageBox("请输入短信内容。");
		} 
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
		SetTimer(8,(QUERY_INTERVAL+QUERY_BD),NULL);
	}else{
		AfxMessageBox("暂无短信链路可用，请稍等再试！");
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
//DEL // 		m_comm.SetOutput(COleVariant(Array));//发送数据
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
		m_comm.SetOutput(COleVariant(Array));//发送数据
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
		m_comm.SetOutput(COleVariant(Array));//发送数据
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
	
	state_system[4]=0;//北斗应答：标记北斗可用
	timer_board_disconnect_times_BD=0;//查询计数器归零
	m_board_led_BD.SetIcon(m_hIconRed);
	m_bd_r.SetIcon(m_hIconRed);
	GetDlgItem(IDC_STATIC_BOARDCONNECT_BD)->SetWindowText(" 北斗已连接！");
	m_FKXX+="北斗连接查询：正常\r\n";
	UpdateData(FALSE);

	if ((comm_init==0)&&(_Useraddr!=0))//初次初始化串口，将卡号提出，完成各帧
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
	m_cardstate ="正常";
	m_basestate="正常";
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
		if(strncmp((const char *)BUFF, "$GLZK",5)==0) command_type= Sig;//功率检测		
		else if (strncmp((const char *)BUFF, "$TXXX",5)==0) command_type= Msg;//通信申请		
		else if (strncmp((const char *)BUFF, "$ICXX",5)==0) command_type= Icc;//IC查询
		else if (strncmp((const char *)BUFF, "$XTZJ",5)==0) command_type= Sts;//系统自检
		else if (strncmp((const char *)BUFF, "$FKXX",5)==0) command_type= FKXX;//反馈信息
		else if (strncmp((const char *)BUFF, "$Zst_",5)==0) command_type= Zst;//
		else if (strncmp((const char *)BUFF, "$Zrd_",5)==0) command_type= Zrd;//
		else if (strncmp((const char *)BUFF, "$Tim_",5)==0) command_type= Tim;//
		else if (strncmp((const char *)BUFF, "$Bst_",5)==0) command_type= Bst;//
		else if (strncmp((const char *)BUFF, "$DWSQ",5)==0) command_type= Pos;//

		
		
		switch(command_type)
		{
		case Sig://功率检测
			DeSig(BUFF);
			break;
		case Msg://通信申请
			DeMsg(BUFF);
			break;
		case FKXX://反馈信息
			DeFKXX(BUFF);
			break;
		case Icc://IC查询
			DeIcc(BUFF);
			break;
		case Sts://系统自检
			DeSts(BUFF);
			break;
		case Bst://串口输出
			break;
		case Zst://零值设置
			break;
		case Zrd://零值读取
			break;
		case Tim://时间输出
			break;
		case Pos://定位申请
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
		m_cardstate ="异常";
	else
		m_cardstate ="正常";

	_Useraddr=((long)BUFF[11])*pow(2, 16)+((long)BUFF[12])*pow(2, 8)+((long)BUFF[13]);
	m_cardnumber=_Useraddr;

//	if(BUFF[14]&0x01)
//		m_basestate="抑制";
//	else
//		m_basestate="非抑制";

	if(BUFF[14]&0x02)
		m_basestate="正常";
	else
		m_basestate="静默";
	
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
	unsigned char rec_text[650]={0};//最多支持接收600个字符
 
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
		strTmp.Format("%02x ",BUFF[j]);    //将字符送入临时变量strtemp存放
		strDisp+=strTmp;  //加入接收编辑框对应字符串
	}
	
	CTime curtime = CTime::GetCurrentTime();
	CString strTime = curtime.Format("%Y-%m-%d %H:%M:%S");

	m_showmsg+=strTime;
	m_showmsg+="从";
	strTmp.Format("%d",Recvaddr);
	m_showmsg+=strTmp;
	m_showmsg+="收到信息：\r\n";
	m_showmsg+=rec_text;
//	m_showmsg+="\t\t数据包：";
//	m_showmsg+=strDisp;
	m_showmsg+="\r\n";
	strTmp = "";
	strTmp = (char*)rec_text;
	strForTTS = strTmp;
	ttsStruct.nPlayTimers = 1; //暂时播放两次
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
		m_FKXX="北斗消息，发送成功！";
		break;
	case 0x01:
		m_FKXX="北斗消息，发送失败！";
		break;
	case 0x02:
		m_FKXX="北斗的信号为锁定！";
		break;
	case 0x03:
		m_FKXX="北斗的电量不足！";
		break;
	case 0x04:
		m_FKXX="北斗的服务频度未到！\r\n请"+temp+"秒后再试。";
		break;
	case 0x05:
		m_FKXX="北斗的加解密错误！";
		break;
	case 0x06:
		m_FKXX="北斗的CRC错误！";
		break;
	default:
		m_FKXX="北斗的扩展功能。";
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
		if (data[i]>128)    //((0<h)&&(h<128))     //判断此字符是否为汉字
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
	if (nIDEvent==1)//北斗发送消息，按钮失能60s
	{
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		flag_bd_is_busy=0;//发送完成，标记北斗空闲中
		KillTimer(1);
	} 
	else if(nIDEvent==2)//每秒中断一次，设置一次进度条的值
	{
		m_timer.StepIt();//每秒中断一次，设置一次进度条的值
		int timer_range=(m_category>1)?m_category:60;//只有当北斗的服务频度获取到（>0）时，才根据其设置进度条，否则使用默认值60
	if(m_timer.GetPos()==timer_range){
		m_timer.SetPos(0);
		KillTimer(2);	
       }
		UpdateData(FALSE);
	}else if (nIDEvent==3)
	{
		OnButtonCall();//使摘机动作对用户透明，先操作一次摘机再调用一次，用于拨号
		KillTimer(3);
	}else if(nIDEvent==4){//有线电话定期轮检
		if((tel_manual_select==0)||(m_tel_manual_select.GetCurSel()==1)){//自动模式,或者指定用有线电话时才查询
		char lpOutBuffer[] = {'A','T','H','\r','\n'};//接着上传ATH指令进行挂机
		CByteArray Array;
		Array.RemoveAll();    
		Array.SetSize(5);		
		
		for (int i=0; i<5; i++)
		{
			Array.SetAt(i,lpOutBuffer[i]);
		}
		if(m_comm_WT.GetPortOpen())
		{
			m_comm_WT.SetOutput(COleVariant(Array));//发送数据
		}
		if(timer_board_disconnect_times_WT==0)timer_board_disconnect_times_WT++;
		SetTimer(5,2000,NULL);//定时器4发出轮检查询帧后，打开定时器5，3次超时timer_board_disconnect_times_WT未被清零，则标记故障
		}
	}else if(nIDEvent==5){
		if (timer_board_disconnect_times_WT!=0)
		{
			timer_board_disconnect_times_WT++;
		}
		if (timer_board_disconnect_times_WT>=QUERY_TOLERATE_TIMES)
		{
			state_system[0]=1;//有线电话超时：标记有线电话故障
			m_FKXX+="有线电话连接查询：故障\r\n";
			UpdateData(FALSE);
			timer_board_disconnect_times_WT=0;
			m_board_led_WT.SetIcon(m_hIconOff);
			m_wt_r.SetIcon(m_hIconOff);
			GetDlgItem(IDC_STATIC_BOARDCONNECT_WT)->SetWindowText("有线电话连接丢失!");
		}
		KillTimer(5);
	}
	//////////////////////以上用于有线电话/////////////////////////////////
	else if (nIDEvent==6)//运维板轮检，类似于定时器4
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
			frame_board_check_YW[6]++;//如果异或结果是$或0x0d，则值加一
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
			m_comm_YW.SetOutput(COleVariant(Array));//发送数据
		}
		
		if(timer_board_disconnect_times_YW==0)timer_board_disconnect_times_YW++;
		SetTimer(7,1000,NULL);//定时器6发出轮检查询帧后，打开定时器7，3次超时timer_board_disconnect_times_YW未被清零，则标记故障
		
	}else if(nIDEvent==7){//运维板查询不通，次数统计.类似于定时器
		if (timer_board_disconnect_times_YW!=0)
		{
			timer_board_disconnect_times_YW++;
		}
		if (timer_board_disconnect_times_YW>=QUERY_TOLERATE_TIMES)
		{
			m_FKXX+="运维板连接查询：故障\r\n";
			UpdateData(FALSE);
			timer_board_disconnect_times_YW=0;
			m_board_led_YW.SetIcon(m_hIconOff);
			GetDlgItem(IDC_STATIC_BOARDCONNECT_YW)->SetWindowText("运维板连接丢失!");
		}
		KillTimer(7);
	}
	///////////////////////以上用于运维板定期查询///////////////////////////
	else if(nIDEvent==8){//北斗定期轮检
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
			m_comm.SetOutput(COleVariant(Array));//发送数据
		}
		if(timer_board_disconnect_times_BD==0)timer_board_disconnect_times_BD++;
		SetTimer(9,1500,NULL);//定时器8发出轮检查询帧后，打开定时器9，3次超时timer_board_disconnect_times_BD未被清零，则标记故障
		}
	}else if(nIDEvent==9){
		if (timer_board_disconnect_times_BD!=0)
		{
			timer_board_disconnect_times_BD++;
		}
		if (timer_board_disconnect_times_BD>=QUERY_TOLERATE_TIMES)
		{
			state_system[4]=1;//北斗超时：标记北斗故障
			m_FKXX+="北斗连接查询：故障\r\n";
			UpdateData(FALSE);
			timer_board_disconnect_times_BD=0;
			m_board_led_BD.SetIcon(m_hIconOff);
			m_bd_r.SetIcon(m_hIconOff);
			GetDlgItem(IDC_STATIC_BOARDCONNECT_BD)->SetWindowText("北斗连接丢失!");
		}
		KillTimer(9);
	}
	//////////////////////以上用于北斗/////////////////////////////////
	else if (nIDEvent == 10)
	{
		m_StatBar->SetText("3G状态：串口已开启",2,0); //以下类似
		if((tel_manual_select==0)||(m_tel_manual_select.GetCurSel()==2)){//自动模式,或者指定用3G电话时才查询
		SendAtCmd("AT+CSQ",6);      //查询CSQ
		if(timer_board_disconnect_times_3G==0)timer_board_disconnect_times_3G++;
		SetTimer(11,1500,NULL);//定时器10发出轮检查询帧后，打开定时器11，3次超时timer_board_disconnect_times_3G未被清零，则标记故障
		}
	}else if(nIDEvent==11){
		if (timer_board_disconnect_times_3G!=0)
		{
			timer_board_disconnect_times_3G++;
		}
		if (timer_board_disconnect_times_3G>=QUERY_TOLERATE_TIMES)
		{
			if((tel_manual_select==0)||(m_tel_manual_select.GetCurSel()==2)){//电话部分，自动或选中3G时允许修改
				state_system[1]=1;//3G超时：标记3G故障
			}
			if((msg_manual_select==0)||(m_msg_manual_select.GetCurSel()==1)){//短信部分，自动或选中3G时允许修改
				state_system[3]=1;
			}
			m_FKXX+="3G信号查询：故障\r\n";
			UpdateData(FALSE);
			timer_board_disconnect_times_3G=0;
			m_board_led_3G.SetIcon(m_hIconOff);
			m_3g_m_r.SetIcon(m_hIconOff);
			m_3g_p_r.SetIcon(m_hIconOff);
			GetDlgItem(IDC_STATIC_BOARDCONNECT_3G)->SetWindowText("3G连接丢失!");
		}
		KillTimer(11);
	}
	else if(nIDEvent==12){//反馈信息文本定时清空
		CEdit* pEdit = ((CEdit*)GetDlgItem(IDC_EDIT_FKXX));
		int len = pEdit->GetLineCount(); 
		if (len >= 18) //自动截取文本，CEdit的限制为64K字节 
		{ 
			OnButtonClearmsg();
		}
	}else if (nIDEvent==13) {//有线电话自动应答，操作超时
		KillTimer(13);
		if((flag_PW_in_busy==1)||(flag_PW_out_busy==1)){
			WT_state=3;
			OnButtonCall();//对方输入操作超时，挂机
		}
	}
	else if (nIDEvent==14) {//有线电话多链路接入广播超时
		KillTimer(14);
		m_StatBar->SetText("运维板状态：多链路广播申请超时",4,0); 
		SetTimer(13,4000,NULL);//播放操作超时音
		PlaySound(".//Wav//PrepareFailed.wav", NULL, SND_FILENAME|SND_ASYNC);//广播准备失败，请稍后重试 (4s)
	}
	else if (nIDEvent==15) {//有线电话循环播放振铃音
		sound_switch(8);//切换到虚拟振铃模式
		PlaySound(".//Wav//ring.wav", NULL, SND_FILENAME|SND_ASYNC);
	}

	else if (nIDEvent==16) {//3G电话自动应答，操作超时
		KillTimer(16);
		if((flag_3G_in_busy==1)||(flag_3G_out_busy==1)){ 
			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
			OnButtonCall();//对方输入操作超时，挂机
		}
	}
	else if (nIDEvent==17) {//3G电话多链路接入广播超时
		KillTimer(17);
		m_StatBar->SetText("运维板状态：多链路广播申请超时",4,0); 
		SetTimer(16,4000,NULL);//播放操作超时音
		PlaySound(".//Wav//PrepareFailed.wav", NULL, SND_FILENAME|SND_ASYNC);//广播准备失败，请稍后重试 (4s)
	}
	else if (nIDEvent==18) {//有线电话循环播放振铃音
		sound_switch(9);//切换到虚拟振铃模式
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
	//////////////////////以上用于3G/////////////////////////////////
	else if (nIDEvent == 20)
	{
		m_StatBar->SetText("卫星电话状态：串口已开启",1,0); //以下类似
		if((tel_manual_select==0)||(m_tel_manual_select.GetCurSel()==3)){//自动模式,或者指定用卫星电话时才查询
			if ((SerialPortOpenCloseFlag_ST==TRUE)&&(bISCallProcess_WX==FALSE))//只有在串口已经打开并且不是在通话中时才查询
			{
				Send_AT_Command_ST("AT+CSQ");//查询信号质量
				SetTimer(22,1000,NULL);
			}      
			if(timer_board_disconnect_times_ST==0)timer_board_disconnect_times_ST++;
			SetTimer(21,2000,NULL);//定时器20发出轮检查询帧后，打开定时器21，3次超时timer_board_disconnect_times_ST未被清零，则标记故障
		}
	}else if(nIDEvent==21){
		if (timer_board_disconnect_times_ST!=0)
		{
			timer_board_disconnect_times_ST++;
		}
		if (timer_board_disconnect_times_ST>=QUERY_TOLERATE_TIMES)
		{
			if((tel_manual_select==0)||(m_tel_manual_select.GetCurSel()==3)){
				state_system[2]=1;//卫星电话超时：标记卫星电话故障
			}

			m_FKXX+="卫星电话定位信号查询：故障\r\n";
			UpdateData(FALSE);
			timer_board_disconnect_times_ST=0;
			m_board_led_ST.SetIcon(m_hIconOff);
			m_st_r.SetIcon(m_hIconOff);
			GetDlgItem(IDC_STATIC_BOARDCONNECT_ST)->SetWindowText("卫星电话连接丢失!");
		}
		KillTimer(21);
	}else if(nIDEvent==22)//查询RINFO
	{
		KillTimer(22);
		Send_AT_Command_ST("AT+RINFO");//区域信息	
	}else if(nIDEvent==23){//定时查询TTS是否发送完毕
		if (bIsTTSPlay==0)
		{
			applay_broadcast(2);//申请结束广播
			KillTimer(23);
		}
	}
	//////////////////////////以上用于卫星电话////////////////////////////////
	//-----------------3G自动应答：5 S----------------------
	else if (nIDEvent==31)  
	{
		KillTimer(31);
		this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
		UpdateData(FALSE);
		m_StatBar->SetText("运维板状态：自动应答输入超时",4,0); 
		SetTimer(16,3000,NULL);//播放操作超时音
		PlaySound(".//Wav//TimeOut.wav", NULL, SND_FILENAME|SND_ASYNC);//对不起，您的操作已超时   (3s)
		
	}else if (nIDEvent==32)
	{
		KillTimer(32);
		this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
		UpdateData(FALSE);
		m_StatBar->SetText("运维板状态：本机无人接听超时",4,0);
		SetTimer(16,4000,NULL);//播放操作超时音
		PlaySound(".//Wav//NoResponse.wav", NULL, SND_FILENAME|SND_ASYNC);//对不起，无人应答，请稍后再试		
	}
	//-----------------3G自动应答：5 E----------------------
	///////////////////////以上用于3G自动应答/////////////////////////////////
	//-----------------有线电话自动应答：5 S----------------------
	else if (nIDEvent==33)  
	{
		KillTimer(33);
		this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
		UpdateData(FALSE);
		m_StatBar->SetText("运维板状态：自动应答输入超时",4,0); 
		SetTimer(13,3000,NULL);//播放操作超时音
		PlaySound(".//Wav//TimeOut.wav", NULL, SND_FILENAME|SND_ASYNC);//对不起，您的操作已超时   (3s)
	}
	else if (nIDEvent==34)
	{
		KillTimer(34);
		this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
		UpdateData(FALSE);
		m_StatBar->SetText("运维板状态：本机无人接听超时",4,0);
		SetTimer(13,4000,NULL);//播放操作超时音
		PlaySound(".//Wav//NoResponse.wav", NULL, SND_FILENAME|SND_ASYNC);//对不起，无人应答，请稍后再试		
	}
	//-----------------有线电话自动应答：5 E----------------------
	///////////////////////以上用于自动应答/////////////////////////////////
	else if (nIDEvent==40)  
	{
		if(bINIFail == TRUE) //继续初始化
		{
				CString strTemp;
				switch (nATCmdID)
				{
				case 1:			
					KillTimer(40);
					SendAtCmd("AT+CMEE=1",1);//返回错误代号     ok or error
					nATCmdID = 3;
					SetTimer(40,1000,NULL);
					break;
				case 3:			
					KillTimer(40);
					SendAtCmd("AT+CLIP=1",3);//来电显示  
					nATCmdID = 5;
					SetTimer(40,1000,NULL);
					break;	
				case 5:
					KillTimer(40);
					SendAtCmd("AT+CLVL=4",5);      //耳机音量设置为4
					nATCmdID = 6;
					SetTimer(40,1000,NULL);
					break;
				case 6:
					KillTimer(40);
					SendAtCmd("AT+CSQ",6);      //查询CSQ
					nATCmdID = 25;
					SetTimer(40,1000,NULL);
					break;
		/*		case 23:       //发送短信
					KillTimer(40);
					strTemp = chPDU;
					SendAtCmd(strTemp,23);
					nATCmdID = 0;
		//			SetTimer(40,1000,NULL);
					break;*/
				case 25:			
					KillTimer(40);
					SendAtCmd("AT+CNMI=2,1,0,0,0",25);      //新短信通知
					nATCmdID = 26;
					SetTimer(40,1000,NULL);
					break;
				case 26:			
					KillTimer(40);
					SendAtCmd("AT+CPMS=\"ME\",\"ME\",\"ME\"",25);      //新短信通知
					nATCmdID = 40;
					SetTimer(40,2000,NULL);
					break;
				case 40:
					KillTimer(40);
					if(SerialPortOpenCloseFlag_3G==TRUE)m_ctrlIconOpenoff_3G.SetIcon(m_hIconRed);//避免关闭串口与点灯不同步
					SetTimer(10,(QUERY_INTERVAL+QUERY_3G),NULL);//查询下CSQ。被移动到初始化最后一个定时器中
					SetTimer(42,2000,NULL);//短信查询 在定时器42里面，2s不能少
					//将初始化过程关闭在ok里面
					nATCmdID = 41; //设置为41,只是最后短信列表查询的判断
					break;
				default:
					nATCmdID = 0;
					break;
				}
		}
		else   //重新开始进行初始化
		{
			KillTimer(40);
			SetTimer(45,10000,NULL);//调用初始化函数
		}
	}
	else if (nIDEvent == 41)
	{
		KillTimer(41);
		if (bRoughLongSMS)
		{
			SendAtCmd("AT+CMGD="+strSMSIndex+","+"0",31);     //删除短信。
			strShortSMSIndex = "";
		} 
		else
		{
			SendAtCmd("AT+CMGD="+strShortSMSIndex+","+"0",31);     //删除短信。	
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
		SendAtCmd("AT+CMGL=4",40);      //查询短信
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
			SendAtCmd("AT+CMGR="+strSMSIndex,32);//读取指定短信   AT+CMGR 32
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
			HWND hWnd = ::FindWindow(NULL, _T("record11"));//关闭录音
			if (NULL != hWnd) {
				::SendMessage(hWnd, WM_CLOSE, 0, 0);
							}
			SendAtCmd("ATE0",2);   //回显命令 0：MS 不回送从TE 接收的字符  1：MS 回送从TE 接收的字符。			
	}
	////////////////////以下定时器用于卫星电话/////////////////////////////////////////
	else if (nIDEvent==50)
	{
		if(BINIfail_WX == TRUE) //继续初始化
		{
					switch(nATCmdFlags)
					{
						case 1:
							KillTimer(50);
							Send_AT_Command_ST("ATE0");  //关回显
							nATCmdFlags = 2;
							SetTimer(50,1000,NULL);
							break;
						case 2:
							KillTimer(50);
							Send_AT_Command_ST("AT+CLIP=1");  //开来电显示
							nATCmdFlags = 3;
							SetTimer(50,1000,NULL);
							break;
						case 3:
							KillTimer(50);
							Send_AT_Command_ST("AT+GPSSERVICE=1");//开GPS				
							nATCmdFlags = 4;
							SetTimer(50,1000,NULL);
							break;
						case 4:
							KillTimer(50);
							Send_AT_Command_ST("AT+CLVL=4");  //设置loudspeaker声音为4，最大
							nATCmdFlags = 5;
							SetTimer(50,1000,NULL);
							break;
						case 5:
							KillTimer(50);
							Send_AT_Command_ST("AT+CMEE=1");  //错误返回设置：设为返回错误代号
							nATCmdFlags = 6;
							SetTimer(50,1000,NULL);
							break;
						case 6:
							KillTimer(50);
							Send_AT_Command_ST("AT+CSQ");//查询信号质量
							nATCmdFlags = 7;
							SetTimer(50,1000,NULL);
							break;
						case 7:
							KillTimer(50);
							Send_AT_Command_ST("AT+CREG=1");  //查询注册状态的设置，返回状态代号
							nATCmdFlags = 8;
							SetTimer(50,1000,NULL);
							break;
						case 8:
							KillTimer(50);
							Send_AT_Command_ST("AT+GPSTRACK=1");//GPS追踪				
							nATCmdFlags = 9;
							SetTimer(50,1000,NULL);
							break;
						case 9:
							KillTimer(50);
							Send_AT_Command_ST("AT+RINFO");//区域信息
							SetTimer(20,(QUERY_INTERVAL+QUERY_ST),NULL);//开始定期检测CSQ和RINFO
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
	else if (nIDEvent == 55)//初始化返回error，重新初始化
	{
		KillTimer(55);
		bISCallProcess_WX = TRUE;
		BINIfail_WX = TRUE;
		nATCmdFlags = 100;
		SetTimer(50,1000,NULL);	
	}
	else if (nIDEvent == 59)//初始化未应答，重新初始化
	{
		BISInitial_WX = TRUE;
		BINIfail_WX = TRUE;
		HWND hWnd = ::FindWindow(NULL, _T("record11"));//关闭录音
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
	//获得按钮文本
	GetDlgItemText(IDC_BUTTON_SET,str);
	if(str=="完成")
	{
		//设置按钮文本
		SetDlgItemText(IDC_BUTTON_SET,"配置");
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_SET,"完成");
	}
	if(str=="完成")
	{
		//显示"简化版"对话框
		if (switch_state==0)//打电话
		{
			SetWindowPos(NULL,0,0,rectSmall.Width(),rectSmall.Height(),SWP_NOMOVE|SWP_NOZORDER);
		}else if(switch_state==1){//发短信
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
	switch_state=0;//打电话
 	SetWindowPos(NULL,0,0,rectSmall.Width(),rectSmall.Height(),SWP_NOMOVE|SWP_NOZORDER);
 	SetDlgItemText(IDC_BUTTON_SET,"配置");
}

void CBeidouDlg::OnButtonMessage() 
{
	// TODO: Add your control notification handler code here
	switch_state=1;//发短信
	SetWindowPos(NULL,0,0,rectMiddle.Width(),rectMiddle.Height(),SWP_NOMOVE|SWP_NOZORDER);
	SetDlgItemText(IDC_BUTTON_SET,"配置");
}
 
void CBeidouDlg::OnComm_WT() 
{
	// TODO: Add your control notification handler code here
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len,k;
	BYTE rxdata[2048]; //设置BYTE数组
	CString strDisp="",strTmp="";
	int frequency_point=0;//频率扫描的总的频点数
	double frequency_buf=0;//频点计算
	int frame_index_WT;//帧的索引	
	
	if((m_comm_WT.GetCommEvent()==2)) //事件值为2表示接收缓冲区内有字符
	{
		Sleep(160);
		variant_inp=m_comm_WT.GetInput(); //读缓冲区
		safearray_inp=variant_inp;  //VARIANT型变量转换为ColeSafeArray型变量
		len=safearray_inp.GetOneDimSize(); //得到有效数据长度
		for(k=0;k<len;k++)
		{
			safearray_inp.GetElement(&k,rxdata+k);//转换为BYTE型数组
		}
		frame_index_WT=0;
		for(k=0;k<len;k++)//将数组转化为CString类型
		{
			BYTE bt=*(char*)(rxdata+k);    //字符型
			frame_receive_WT[frame_index_WT]=bt;
			frame_index_WT++;			
		}
	
		
	if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='N'))//检测是否为DTMF信号
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
				send_string+='#';//加上#号
				m_FKXX+="有线电话传号:";
				m_FKXX+=send_string;
				m_FKXX+="\r\n";
				UpdateData(FALSE);
				KillTimer(33);//停掉ring指令中打开的定时器33
				nCountCheck++;
				if (send_string == "1#")//多链路接入广播
				{
					SetTimer(14,20000,NULL);//超时时，播放广播准备失败音
					applay_broadcast(1);//申请开始广播
					m_StatBar->SetText("运维板状态：多链路广播申请已下发，未应答",4,0); 
					PlaySound(".//Wav//BroadPrepare.wav", NULL, SND_FILENAME|SND_ASYNC);//广播正在准备中，请稍候   (3s)
					bIsAutoResponseProcess = TRUE;
				}     
				else if (send_string == "2#")//本地通话
				{
					SetTimer(34,NOPICKUP_TIME ,NULL);//自定义振铃等待
					PlaySound(".//Wav//RingConnect.wav", NULL, SND_FILENAME|SND_ASYNC);//通话正在连接中，请稍候   (4s)   //这里可以弄一段背景音乐(可重复播放)
					bIsAutoResponseProcess = TRUE;
					//向电台人员通知来电,本地播放振铃音   
					SetTimer(15,4000,NULL);//每隔4s播放一次ring振铃音
					WT_state=4;//等待虚拟摘机
					GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("接听电话");
					GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
				}
				else
				{
					if (nCountCheck < 3)
					{
						PlaySound(".//Wav//ErrorCmd.wav", NULL, SND_FILENAME|SND_ASYNC);//指令出错，请重新输入
						SetTimer(33,AUTORESPONSE_TIME,NULL);//15s等待时间（等待用户输入，时间包括了播放语音的时间）
					} 
					else
					{
						WT_state=3;
						GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
						if((flag_PW_in_busy==1)||(flag_PW_out_busy==1))OnButtonCall();
					}
					
				}
				send_string="";
				UpdateData(FALSE);
			}	
		}
		
	}
	else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='C')&&(frame_receive_WT[3]=='I')&&(frame_receive_WT[4]=='D'))//检测是否为来电提示
	{
		strDisp=frame_receive_WT+5;
		call_in_number=strDisp;//开始拼接来电号码

	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='Z'))//检测是否摘机成功
	{
		if (flag_PW_out_busy == 1)
		{
			ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);	//主动呼出，启动录音
		}		
		m_FKXX+="摘机成功";
		m_FKXX+="\r\n";
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='D'))//检测是否拨号成功
	{
		m_FKXX+="拨号成功";
		m_FKXX+="\r\n";
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='H'))//检测是否挂机成功
	{
		m_board_led_WT.SetIcon(m_hIconRed);
		m_wt_r.SetIcon(m_hIconRed);
		GetDlgItem(IDC_STATIC_BOARDCONNECT_WT)->SetWindowText(" 有线电话已连接！"); 
		timer_board_disconnect_times_WT=0;//查询计数器归零
		state_system[0]=0;//有线电话应答：标记有线电话可用
		if((flag_PW_in_busy==1)||(flag_PW_out_busy==1))	m_FKXX+="挂机成功\r\n";
		else m_FKXX+="有线电话连接查询：正常\r\n";
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='B'))//检测是否传号成功
	{
		m_FKXX+="传号成功";
		m_FKXX+="\r\n";
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='S')&&(frame_receive_WT[3]=='2'))//检测是否为挂机提示
	{
		applay_broadcast(2);//申请终止广播
		SetTimer(19,3000,NULL);//全部语音链路空闲时，定期结束广播
		KillTimer(34);//对方挂机，关闭摘机超时计时器
		KillTimer(15);//对方挂机则关闭定期振铃音
		KillTimer(14);
		m_StatBar->SetText("运维板状态：多链路广播终止已下发，未应答",4,0); 
		m_FKXX+="对方挂机";
		m_FKXX+="\r\n";
		if((flag_PW_in_busy==1)||(flag_PW_out_busy==1)){
			WT_state=3;
			OnButtonCall();//对方挂机后，我也挂机，电话状态设置为空闲。避免出现对方先挂机，我之后立即挂机，此处出现重播的问题
		}
		char lpOutBuffer[] = {'A','T','H','\r','\n'};//接着上传ATH指令进行挂机
		CByteArray Array;
		Array.RemoveAll();
		Array.SetSize(5);		
		
		for (int i=0; i<5; i++)
		{
			Array.SetAt(i,lpOutBuffer[i]);
		}
		if(m_comm_WT.GetPortOpen())
		{
			m_comm_WT.SetOutput(COleVariant(Array));//发送数据
		}
		GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("拨打电话");
		sound_switch(4);//开关切为默认
		SetTimer(4,(QUERY_INTERVAL+QUERY_WT),NULL);//主动挂机和被动挂机都打开定时器//此处可删除
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='R')&&(frame_receive_WT[3]=='i')&&(frame_receive_WT[4]=='n')&&(frame_receive_WT[5]=='g'))//检测是否为来电提示音
	{
		if (state_system[0]==1)
		{
			return;
		}
		KillTimer(4);
		m_FKXX+="Ring!  ";
		flag_PW_in_busy=1;
		send_string="";//传号字符串清空，准备接收传号
		//----------自动应答：2 S-----------
		OnButtonCall();//摘机，自动播放第一段语音
		sound_switch(5);//切换到有线电话被叫模式，进入自动应答
		SetTimer(33,AUTORESPONSE_TIME,NULL);//20s等待时间（等待用户输入，时间包括了播放语音的时间）
		PlaySound(".//Wav//Guide.wav", NULL, SND_FILENAME|SND_ASYNC);//应急电台系统，自动广播请按1，原始通话请按2,#号结束
		//----------自动应答：2 E-----------
		
	}else if (rxdata[0] >='0' && rxdata[0]<='9')//检测是否是电话号码
	{
		strDisp=frame_receive_WT;
		call_in_number+=strDisp;
		m_FKXX+="\r\n对方号码是:";
		m_FKXX+=call_in_number;
	} 
	else
	{
	//	AfxMessageBox("下位机帧有错误！",MB_OK,0);
		m_FKXX+=frame_receive_WT;
		m_FKXX+="电话板帧有错误，检察线是否连好\r\n";
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

		//以下是串口的初始化配置
		if(m_comm_WT.GetPortOpen())//打开端口前的检测，先关，再开
			MessageBox("串口无法打开");
//			m_comm.SetPortOpen(FALSE);	//	
		m_comm_WT.SetCommPort(m_DCom_WT); //选择端口，默认是com2
		m_comm_WT.SetSettings((LPSTR)(LPCTSTR)string1); //波特率1200，无校验，8个数据位，1个停止位
		if(!m_comm_WT.GetPortOpen())
		{			
			m_comm_WT.SetPortOpen(TRUE);//打开串口
			GetDlgItem(IDC_OPENCLOSEPORT_WT)->SetWindowText("关闭串口");
			m_StatBar->SetText("有线电话：串口已连接",0,0);
			m_openoff_WT.SetIcon(m_hIconRed);
			
			Sleep(70);//等待串口稳定，再发送有线电话的查询消息
			char lpOutBuffer[] = {'A','T','H','\r','\n'};//接着上传ATH指令进行挂机
			CByteArray Array;
			Array.RemoveAll();    
			Array.SetSize(5);		
			
			for (int i=0; i<5; i++)
			{
				Array.SetAt(i,lpOutBuffer[i]);
			}
			if(m_comm_WT.GetPortOpen())
			{
				m_comm_WT.SetOutput(COleVariant(Array));//发送数据
			}

//			GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
			SetTimer(4,(QUERY_INTERVAL+QUERY_WT),NULL);//自动模式下才打开定时器
		}
		else
			MessageBox("串口无法打开.");	 
	}
	else
	{
		SerialPortOpenCloseFlag_WT=FALSE;
		GetDlgItem(IDC_OPENCLOSEPORT_WT)->SetWindowText("打开串口");
		m_openoff_WT.SetIcon(m_hIconOff);
		m_comm_WT.SetPortOpen(FALSE);//关闭串口
		state_system[0]=1;//有线电话串口关闭
		m_board_led_WT.SetIcon(m_hIconOff);
		m_wt_r.SetIcon(m_hIconOff);
		GetDlgItem(IDC_STATIC_BOARDCONNECT_WT)->SetWindowText(" 有线电话已断开！"); 
//		GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(FALSE);
		m_StatBar->SetText("有线电话：串口已断开",0,0);
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
	
 	if((flag_PW_in_busy==1)||(flag_PW_out_busy==1)||(flag_3G_out_busy==1)||(flag_3G_out_busy==1)){//通信中，按钮始终使能
 		GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
 	}else{//空闲中
 		GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(FALSE);
 	}	
	GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(FALSE);
}

void CBeidouDlg::OnButtonCall() 
{
	// TODO: Add your control notification handler code here
	if((state_system[0]==0)&&(flag_3G_in_busy==0)&&(flag_ST_in_busy==0)){//拨打电话时：有线电话可用 并且3G没有打入卫星电话没有打入，先用有线电话
		if((flag_3G_out_busy==0)&&(flag_3G_in_busy==0)&&(flag_PW_out_busy==0)&&(flag_PW_in_busy==0)&&(m_target_number=="0")){//只有拨打电话时(全部空闲)，才检测是否有号码输入
			AfxMessageBox("请先输入对方号码!");
			return;
	}
		
		if(WT_state==0){//摘机			
			KillTimer(4);
			char lpOutBuffer[] = {'A','T','Z','\r','\n'};//接着上传ATH指令进行挂机
			CByteArray Array;
			Array.RemoveAll();
			Array.SetSize(5);		
			
			for (int i=0; i<5; i++)
			{
				Array.SetAt(i,lpOutBuffer[i]);
			}
			if(m_comm_WT.GetPortOpen())
			{
				m_comm_WT.SetOutput(COleVariant(Array));//发送数据
			}

			if (flag_PW_in_busy==1)//被叫，接听电话
			{
				WT_state=3;//该挂机啦
				GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
			} 
			else//向外打电话
			{
				sound_switch(1);//有线电话，主动呼出
				flag_PW_out_busy=1;//主动拨打电话
				GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("正在拨号...");
				GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(FALSE);//拨打电话时，关闭退格按钮
				SetTimer(3,100,NULL);//100ms后再调用本函数进行拨号，使摘机动作对用户透明，先操作一次摘机再调用一次，用于拨号
				WT_state=2;//该拨号啦
			}			
		}
		else if(WT_state==2)//拨号
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
				m_comm_WT.SetOutput(COleVariant(Array));//发送数据
			}
			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
			GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(TRUE);//使能退格键
			WT_state=3;//下一步该挂机了
		}
		else if (WT_state==3)//挂机
		{			
			char lpOutBuffer[] = {'A','T','H','\r','\n'};//接着上传ATH指令进行挂机
			CByteArray Array;
			Array.RemoveAll();    
			Array.SetSize(5);		
			
			for (int i=0; i<5; i++)
			{
				Array.SetAt(i,lpOutBuffer[i]);
			}
			if(m_comm_WT.GetPortOpen())
			{
				m_comm_WT.SetOutput(COleVariant(Array));//发送数据
			}

			//关闭录音
			HWND hWnd = ::FindWindow(NULL, _T("record11"));
			if (NULL != hWnd) {
				::SendMessage(hWnd, WM_CLOSE, 0, 0);
			}//关录音完成

			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("拨打电话");
			WT_state=0;//挂机，空闲中。下一步摘机
			flag_PW_out_busy=0;//主动挂机，清零
			flag_PW_in_busy=0;//主动挂机，清零
			bIsAutoResponseProcess = FALSE;
			nCountCheck = 0;
			SetTimer(4,(QUERY_INTERVAL+QUERY_WT),NULL);//主动挂机和被动挂机都打开定时器
			sound_switch(4);//开关切为默认
			applay_broadcast(2);//申请终止广播
			SetTimer(19,3000,NULL);//全部语音链路空闲时，定期结束广播
			KillTimer(15);//超时挂机则关闭定期振铃音
			m_StatBar->SetText("运维板状态：多链路广播终止已下发，未应答",4,0); 
		} 
		else if(WT_state==4)//虚拟摘机
		{
			KillTimer(34);//关闭摘机超时计时器
			KillTimer(15);//摘机则关闭定期振铃音
			sound_switch(1);//有线电话虚拟摘机，变为主动模式
			ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);	//被叫，虚拟摘机，启动录音
			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
			WT_state=3;//通话中,下一步就是挂机了
		}//end of WT_state		
	}
	/*************************************3G********************************************/
	else if ((state_system[1]==0)&&(flag_ST_in_busy==0))//拨打电话时：有线电话不可用时，再用3G电话
	{
		UpdateData(TRUE);
		CString strToSend,str;
		this->GetDlgItemText(IDC_BUTTON_CALL,str);
		if (str == "拨打电话")
		{
			nCallFlags = 1;
		} 
		else if(str == "挂机")
		{
			//关闭录音
			HWND hWnd = ::FindWindow(NULL, _T("record11"));
			if (NULL != hWnd) {
				::SendMessage(hWnd, WM_CLOSE, 0, 0);
			}//关录音完成
			nCallFlags = 3;
		}
		else if(str == "接听电话")
		{
			nCallFlags = 2;	
		}
		else if(str == "虚拟摘机")
		{
			nCallFlags = 4;	
		}
		int i;
		CString strTemp;
		int len = m_target_number.GetLength();	
		switch (nCallFlags)   //判断
		{
			case 0://暂不做什么
				break;
			case 1://主叫，拨打电话
				char *chNum;
				chNum = (LPSTR)(LPCTSTR)m_target_number;
				if (len>=11) //固话加上区号也有可能是11位
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
						AfxMessageBox("号码长度出错，或者11位号码不是手机号码。");
						return;
					}
				} 
				else{
					strTemp =m_target_number;   //直接拨出去
				}

				bIsCaller = TRUE;//主叫标志位
				strToSend = "ATD"+ strTemp;
				strToSend += "I";//启动CLIR
				strToSend += ";";
				sound_switch(2);//开关切换到3G主动呼出模式
				SendAtCmd(strToSend,11);
				flag_3G_out_busy = 1;
				this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");			
				break;
			case 2://被叫，自动应答
				//在自动摘机验证成功后将文本设为     接听电话 （重要）挂断电话在自动接听里面， 
				SendAtCmd("ATA",12);//摘机
				break;
			case 3://挂机
				applay_broadcast(2);//申请终止广播
				SetTimer(19,3000,NULL);//全部语音链路空闲时，定期结束广播
				strToSend = "AT+CHUP";
				SendAtCmd(strToSend,13);//情况比较特殊，挂断的回复，TRUEorFALSE均有可能，现在暂设为FALSE，不管
				flag_3G_out_busy = 0;
				flag_3G_in_busy = 0;
				bRadioIsReady = FALSE;
				nCountCheck = 0;
				SetTimer(10,(QUERY_INTERVAL+QUERY_3G),NULL);
				this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("拨打电话");
				m_FKXX += strToSend;
				UpdateData(FALSE);
				OnClearAll();
				nCallFlags = 0;
				bIsAutoResponseProcess = FALSE;
				break;
			case 4://虚拟摘机
				KillTimer(32);
				KillTimer(18);
				sound_switch(2);
				//放音通知来电   //开mic    //正式开始普通电话通话
				bIsAutoResponseProcess = TRUE;
				ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);	//3G启动录音
				bIsRecord = TRUE;
				GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
				break;
			default:
				break;
		}
	}
	/*************************************卫星电话********************************************/
	else if (state_system[2]==0)//拨打电话时：有线电话、3G不可用时，再用卫星电话
	{
		UpdateData(TRUE);
		int nCallSatelliteFlags = 0;
		CString str;
		this->GetDlgItemText(IDC_BUTTON_CALL,str);
		if (str == "拨号")
		{
			nCallSatelliteFlags = 1;
		} 
		else if(str == "挂机")
		{
			nCallSatelliteFlags = 3;
		}
		else if(str == "接听")
		{
			nCallSatelliteFlags = 2;	
		}
		//无虚拟摘机
		//判断
		switch (nCallSatelliteFlags)
		{
		case 1:
			KillTimer(20);//停止定期查询
			bISCallProcess_WX = TRUE;
			bISCaller_WX = TRUE;
			flag_ST_out_busy=1;
			this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
			nATCmdFlags = 15;
			Send_AT_Command_ST("ATD"+m_target_number+";");//拨号
			break;
		case 2:	
			this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
			nATCmdFlags = 16;
			Send_AT_Command_ST("ATA");
			ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);	//启动录音
			bIsRecording = TRUE;
			break;
		case 3:
			SetTimer(20,(QUERY_ST+QUERY_INTERVAL),NULL);//打开查询定时器
			bISCallProcess_WX = FALSE;
			flag_ST_out_busy=0;
			flag_ST_in_busy=0;
			this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("拨号");
			nATCmdFlags = 17;
			Send_AT_Command_ST("ATH");
			break;
			//		ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);	//启动录音
		default:
			break;
		}
	}
	else{
		AfxMessageBox("暂无电话链路可用，请稍等再试！");
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
		module_reset(1);//复位模块 1：有线电话；2：卫星电话；3：3G模块；4：北斗模块；5：广播板；6：其他；
	}
	else if (soundswitch==TRUE)
	{
		sound_switch(1);
	}
	else{//有线电话传号
		chuanhao('1',ChuanHaoID);
	}
}

void CBeidouDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
//	applay_broadcast(2);

	if(modulereset==TRUE) 
	{
		module_reset(2);//复位模块 1：有线电话；2：卫星电话；3：3G模块；4：北斗模块；5：广播板；6：其他；
	}
	else if (soundswitch==TRUE)
	{
		sound_switch(2);
	}
	else{//有线电话传号
		chuanhao('2',ChuanHaoID);
	}
}

void CBeidouDlg::chuanhao(char num,CString ID)
{
	if (m_target_number=="0")//把零移除
	{
		m_target_number=m_target_number.Left(m_target_number.GetLength()-1);
	}
	if(SerialPortOpenCloseFlag_WT||SerialPortOpenCloseFlag_3G)GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(TRUE);
//	if(flag_PW_out_busy==0)GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(TRUE);//主动呼出时，失能退格按钮
	
	m_target_number+=num;
	UpdateData(FALSE);

	if((flag_PW_in_busy==1)||(flag_PW_out_busy==1))
	{//有线电话呼入或呼出后，才传号
		char lpOutBuffer[] = {'A','T','B','0','\r','\n'};//接着上传ATH指令进行挂机
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
			m_comm_WT.SetOutput(COleVariant(Array));//发送数据
		}				
	}
	else if((flag_3G_in_busy==1)||(flag_3G_out_busy==1)){//3G电话呼入或呼出后，才传号
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
		module_reset(3);//复位模块 1：有线电话；2：卫星电话；3：3G模块；4：北斗模块；5：广播板；6：其他；
	}
	else if (soundswitch==TRUE)
	{
		sound_switch(3);
	}
	else{//有线电话传号
		chuanhao('3',ChuanHaoID);
	}
}

void CBeidouDlg::OnButton4() 
{
	// TODO: Add your control notification handler code here
	if(modulereset==TRUE) 
	{
		module_reset(4);//复位模块 1：有线电话；2：卫星电话；3：3G模块；4：北斗模块；5：广播板；6：其他；
	}
	else if (soundswitch==TRUE)
	{
		sound_switch(4);
	}
	else{//有线电话传号
		chuanhao('4',ChuanHaoID);
	}
}

void CBeidouDlg::OnButton5() 
{
	// TODO: Add your control notification handler code here
	if(modulereset==TRUE) 
	{
		module_reset(5);//复位模块 1：有线电话；2：卫星电话；3：3G模块；4：北斗模块；5：广播板；6：调理板；7：12V功放；8：17V功放；
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
		module_reset(6);//复位模块 1：有线电话；2：卫星电话；3：3G模块；4：北斗模块；5：广播板；6：调理板；7：12V功放；8：17V功放；
	}
	else if (soundswitch==TRUE)
	{
		sound_switch(6);
	}
	else{
	chuanhao('6',ChuanHaoID);
	}
}

void CBeidouDlg::OnButton8() //对应数字7键，写反了
{
	// TODO: Add your control notification handler code here
	if(modulereset==TRUE) 
	{
		module_reset(7);//复位模块 1：有线电话；2：卫星电话；3：3G模块；4：北斗模块；5：广播板；6：调理板；7：12V功放；8：17V功放；
	}
	else if (soundswitch==TRUE)
	{
		sound_switch(7);
	}
	else{
	chuanhao('7',ChuanHaoID);
	}
}

void CBeidouDlg::OnButton7() //对应数字8键，写反了
{
	// TODO: Add your control notification handler code here
	if(modulereset==TRUE) 
	{
		module_reset(8);//复位模块 1：有线电话；2：卫星电话；3：3G模块；4：北斗模块；5：广播板；6：调理板；7：12V功放；8：17V功放；
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
//	if(flag_PW_out_busy==1){//拨出电话时，退格键对传号而言起不到任何作用，此时直接返回
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
	m_c_FKXX.LineScroll(m_c_FKXX.GetLineCount(),0);//让反馈信息输入框保持在最后一行
}

void CBeidouDlg::OnComm_YW() 
{
	// TODO: Add your control notification handler code here
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len,k;
	BYTE rxdata[2048]; //设置BYTE数组
	CString strDisp="",strTmp="";
	int frequency_point=0;//频率扫描的总的频点数
	double frequency_buf=0;//频点计算
	unsigned char frame_receive[100]={0};//接收缓冲区
	
	if((m_comm_YW.GetCommEvent()==2)) //事件值为2表示接收缓冲区内有字符
	{
		Sleep(100);//等待一帧数据全部存储在缓冲区，避免帧的拼接产生的延迟
		variant_inp=m_comm_YW.GetInput(); //读缓冲区
		safearray_inp=variant_inp;  //VARIANT型变量转换为ColeSafeArray型变量
		len=safearray_inp.GetOneDimSize(); //得到有效数据长度
		for(k=0;k<len;k++)
		{
			safearray_inp.GetElement(&k,rxdata+k);//转换为BYTE型数组
		}
		
		//			AfxMessageBox("OK",MB_OK,0);
		//			frame=frame_len[frame_index_YW];
		//			frame_len[frame_index_YW]=0;
		
		frame_index_YW=0;
		for(k=0;k<len;k++)//将数组转化为CString类型
		{
			BYTE bt=*(char*)(rxdata+k);    //字符型
				if (rxdata[0]!='$')
				{
					return;//帧数据串错误
				}
			frame_receive[frame_index_YW]=bt;//frame_receive_YW
			frame_index_YW++;
// 			strTmp.Format("%c",bt);    //将字符送入临时变量strtemp存放
// 			strDisp+=strTmp;  //加入接收编辑框对应字符串
			
		}
//		AfxMessageBox(strDisp,MB_OK,0);

	if (((flag_com_init_ack_YW==0)||(timer_board_disconnect_times_YW!=0))&&(frame_receive[1]=='r')&&(frame_receive[2]=='e')&&(frame_receive[3]=='y')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_wakeup_times)&&(frame_receive[9]==XOR(frame_receive,9)))//首次连接握手，上位机软件接收时，不用避免$,\r,\n
	{
		flag_com_init_ack_YW=1;
		m_board_led_YW.SetIcon(m_hIconRed);
		GetDlgItem(IDC_STATIC_BOARDCONNECT_YW)->SetWindowText(" 运维板已连接！"); 
		m_FKXX+="运维板连接查询：正常\r\n";
		timer_board_disconnect_times_YW=0;//收到反馈则清零

		int temperature=frame_receive[7]*256+frame_receive[8];
		m_temperature.Format("%.1f",(float)temperature/10);
		UpdateData(FALSE);
		
	}else if ((flag_com_init_ack_YW==1)&&(frame_receive[1]=='r')&&(frame_receive[2]=='s')&&(frame_receive[3]=='e')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_control_times)&&(frame_receive[8]==XOR(frame_receive,8)))//复位指定模块
	{
	
		switch(frame_receive[7]-0x30){
		case 1:
			m_StatBar->SetText("运维板状态：有线电话模块复位完毕",4,0); 
			break;
		case 2:
			m_StatBar->SetText("运维板状态：卫星电话模块复位完毕",4,0); 
			break;
		case 3:
			m_StatBar->SetText("运维板状态：3G模块复位完毕",4,0); 
			break;
		case 4:
			m_StatBar->SetText("运维板状态：北斗模块复位完毕",4,0); 
			break;
		case 5:
			m_StatBar->SetText("运维板状态：广播板复位完毕",4,0); 
			break;
		case 6:
			m_StatBar->SetText("运维板状态：调理板复位完毕",4,0); 
			break;
		case 7:
			m_StatBar->SetText("运维板状态：12V功放复位完毕",4,0); 
			break;
		case 8:
			m_StatBar->SetText("运维板状态：48V功放复位完毕",4,0); 
			break;
		default:
			m_StatBar->SetText("运维板状态：复位模块指定出错",4,0); 
			break;
		}

	}else if ((flag_com_init_ack_YW==1)&&(frame_receive[1]=='s')&&(frame_receive[2]=='w')&&(frame_receive[3]=='h')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_control_times)&&(frame_receive[8]==XOR(frame_receive,8)))//切换音频开关申请帧
	{
//		if ((frame_receive[7]-0x30)!=5)sound_switch(5);//非自动应答模式下,切换到自动应答

		if ((frame_receive[7]-0x30)==1)
		{
			m_StatBar->SetText("运维板状态：切换到有线电话主叫",4,0); 
		} 
		else if ((frame_receive[7]-0x30)==2)
		{
			m_StatBar->SetText("运维板状态：切换到3G主叫",4,0); 
		}
		else if ((frame_receive[7]-0x30)==3)
		{
			m_StatBar->SetText("运维板状态：切换到卫星电话主叫",4,0); 
		}
		else if ((frame_receive[7]-0x30)==4)
		{
			m_StatBar->SetText("运维板状态：切换到PC输出",4,0); 
		}else if ((frame_receive[7]-0x30)==5)
		{
			m_StatBar->SetText("运维板状态：切换到有线电话被叫",4,0); 
		}else if ((frame_receive[7]-0x30)==6)
		{
			m_StatBar->SetText("运维板状态：切换到3G被叫",4,0); 
		}else if ((frame_receive[7]-0x30)==7)
		{
			m_StatBar->SetText("运维板状态：切换到卫星电话被叫",4,0); 
		}
		


	}else if ((flag_com_init_ack_YW==1)&&(frame_receive[1]=='s')&&(frame_receive[2]=='c')&&(frame_receive[3]=='a')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_control_times)&&(frame_receive[7]==XOR(frame_receive,7)))//频谱扫描帧反馈信息
	{
		m_StatBar->SetText("运维板状态：配置为频谱扫描模式",4,0);
		
		
	}else if ((flag_com_init_ack_YW==1)&&(frame_receive[1]=='p')&&(frame_receive[2]=='p')&&(frame_receive[3]=='p')&&(frame_receive[4]=='_')
	&&(frame_receive[7]==XOR(frame_receive,7)))//多链路接入/断开申请通过//&&(frame_receive[5]==index_wakeup_times)
	{
		if (frame_receive[6]==1)
		{
			KillTimer(14);//关闭多链路申请超时定时器
			KillTimer(17);
			m_StatBar->SetText("运维板：多链路广播申请应答成功",4,0);
			ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);//3G开始录音
			bIsRecord = TRUE;
			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
			PlaySound(".//Wav//PrepareSuccess.wav", NULL, SND_FILENAME|SND_ASYNC);//广播准备成功，请开始广播
			GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
			broad_prepare=1;
		} 
		else if(frame_receive[6]==2)
		{
			m_StatBar->SetText("运维板：多链路广播断开应答成功",4,0); 
			broad_prepare=2;
		}
		else if(frame_receive[6]==3)
		{
			KillTimer(14);//关闭多链路申请超时定时器
			KillTimer(17);
			m_StatBar->SetText("运维板：多链路广播申请应答成功",4,0);
			broad_prepare=1;
		}
		
	}else if ((flag_com_init_ack_YW==1)&&(frame_receive[1]=='r')&&(frame_receive[2]=='s')&&(frame_receive[3]=='t')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==0)&&(frame_receive[7]==0)&&(frame_receive[8]==XOR(frame_receive,8)))//重传帧
	{
		//	AfxMessageBox("wakaka",MB_OK,0);
		switch (index_resent_data_frame)
		{
		case 6://运维板复位帧校验出错，请求重传
	//		OnButtonBoardReset();
			break;
		case 7://频谱扫描，继电器控制帧校验出错，请求重传
//			OnButtonScan();
			break;
		case 3://

			break;

		}
		m_StatBar->SetText("运维板状态：运维板请求重传",4,0);
	}  
	else
	{
		m_StatBar->SetText("运维板状态：运维板回传帧有错误！",4,0);
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

		//以下是串口的初始化配置
		if(m_comm_YW.GetPortOpen())//打开端口前的检测，先关，再开
			MessageBox("串口无法打开！");
//			m_comm.SetPortOpen(FALSE);	//	
		m_comm_YW.SetCommPort(m_DCom_YW); //选择端口，默认是com3
		m_comm_YW.SetSettings((LPSTR)(LPCTSTR)string1); //波特率115200，无校验，8个数据位，1个停止位
		if(!m_comm_YW.GetPortOpen())
		{			
			m_comm_YW.SetPortOpen(TRUE);//打开串口
			GetDlgItem(IDC_BUTTON_CONNECTYUNWEI)->SetWindowText("关闭串口");
			m_StatBar->SetText("运维板状态：串口已打开",4,0); //以下类似

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
				frame_board_check_YW[6]++;//如果异或结果是$或0x0d，则值加一
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
				m_comm_YW.SetOutput(COleVariant(Array));//发送数据
			}
			index_resent_data_frame=0;//连接帧不支持重传机制
			SetTimer(6,(QUERY_INTERVAL+QUERY_YW),NULL);//没有处在频谱扫描阶段才打开定期查询，10+3秒(与广播板的查询在时间上错开)查询一次子板是否保持连接。恢复硬件连接时，可以自动连接
		}
		else
			MessageBox("无法打开运维板串口，请重试！");	 
	}
	else
	{
		SerialPortOpenCloseFlag_YW=FALSE;
		GetDlgItem(IDC_BUTTON_CONNECTYUNWEI)->SetWindowText("打开串口");
		m_StatBar->SetText("运维板状态：串口已关闭",4,0); //以下类似
		m_ctrlIconOpenoff_YW.SetIcon(m_hIconOff);
		m_board_led_YW.SetIcon(m_hIconOff);
		GetDlgItem(IDC_STATIC_BOARDCONNECT_YW)->SetWindowText(" 运维板已断开！");
		flag_com_init_ack_YW=0;//运维板未连接
		m_comm_YW.SetPortOpen(FALSE);//关闭串口
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

void CBeidouDlg::module_reset(int index)//复位指定模块
{
	if(SerialPortOpenCloseFlag_YW==TRUE)//只有当运维板串口打开了，才可以使用
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
		frame_board_reset_YW[6]=index+0x30;//复位模块 1：有线电话；2：卫星电话；3：3G模块；4：北斗模块；5：广播板；6：调理板；7：12V功放；8：17V功放；
		frame_board_reset_YW[7]=XOR(frame_board_reset_YW,7);
		if ((frame_board_reset_YW[7]=='$')||(frame_board_reset_YW[7]==0x0d))
		{
			frame_board_reset_YW[7]++;//如果异或结果是$或0x0d，则值加一
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
			m_comm_YW.SetOutput(COleVariant(Array));//发送数据
		}
		index_resent_data_frame=6;//6运维板复位重传帧编号
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
		GetDlgItem(IDC_MODULE_RESET)->SetWindowText("取消复位");
		
		GetDlgItem(IDC_BUTTON_1)->SetWindowText("有线电话");
		GetDlgItem(IDC_BUTTON_2)->SetWindowText("卫星电话");
		GetDlgItem(IDC_BUTTON_3)->SetWindowText("3G");
		GetDlgItem(IDC_BUTTON_4)->SetWindowText("北斗");

		GetDlgItem(IDC_BUTTON_5)->SetWindowText("广播板");
		GetDlgItem(IDC_BUTTON_6)->SetWindowText("调理板");
		GetDlgItem(IDC_BUTTON_8)->SetWindowText("12V功放");
		GetDlgItem(IDC_BUTTON_7)->SetWindowText("48V功放");
	} 
	else
	{
		modulereset=FALSE;
		GetDlgItem(IDC_MODULE_RESET)->SetWindowText("手动复位");

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
	if(SerialPortOpenCloseFlag_YW==TRUE)//只有当运维板串口打开了，才可以使用
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
		frame_board_sound_YW[6]=index+0x30;//音频切换 1：有线电话；2：3G；3：卫星电话；4：PC音频输出；5：广播板；6：调理板；7：12V功放；8：48V功放；10：使用结束，切回空闲模式；
		frame_board_sound_YW[7]=XOR(frame_board_sound_YW,7);
		if ((frame_board_sound_YW[7]=='$')||(frame_board_sound_YW[7]==0x0d))
		{
			frame_board_sound_YW[7]++;//如果异或结果是$或0x0d，则值加一
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
			m_comm_YW.SetOutput(COleVariant(Array));//发送数据
		}
		index_resent_data_frame=6;//6运维板复位重传帧编号
	}	
}

void CBeidouDlg::OnSoundSwitch() 
{
	// TODO: Add your control notification handler code here
	if (modulereset==TRUE)
	{
		OnModuleReset();//音频切换与手动复位只有一个可用
	}
	if (soundswitch==FALSE)
	{
		soundswitch=TRUE;
		GetDlgItem(IDC_SOUND_SWITCH)->SetWindowText("关闭切换");

		GetDlgItem(IDC_BUTTON_1)->SetWindowText("有线电话主叫");
		GetDlgItem(IDC_BUTTON_2)->SetWindowText("3G主叫");
		GetDlgItem(IDC_BUTTON_3)->SetWindowText("卫星电话主叫");
		GetDlgItem(IDC_BUTTON_4)->SetWindowText("PC音频输出");

		GetDlgItem(IDC_BUTTON_5)->SetWindowText("有线电话被叫");
		GetDlgItem(IDC_BUTTON_6)->SetWindowText("3G被叫");
		GetDlgItem(IDC_BUTTON_8)->SetWindowText("卫星电话被叫");
	} 
	else
	{
		soundswitch=FALSE;
		GetDlgItem(IDC_SOUND_SWITCH)->SetWindowText("音频切换");

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
		
		//以下是串口的初始化配置
		if(m_comm_3G.GetPortOpen())//打开端口前的检测，先关，再开
			MessageBox("串口无法打开！");
		m_comm_3G.SetCommPort(m_DCom_3G); //选择端口，默认是com3
		m_comm_3G.SetSettings((LPSTR)(LPCTSTR)string1); //波特率115200，无校验，8个数据位，1个停止位
		if(!m_comm_3G.GetPortOpen())
		{			
			m_comm_3G.SetPortOpen(TRUE);//打开串口
			GetDlgItem(IDC_BUTTON_CONNECT3G)->SetWindowText("关闭串口");
			m_StatBar->SetText("3G状态：串口已打开",2,0); //以下类似
			UpdateData();
			bIsINIProcess = TRUE;
			bINIFail = TRUE;
			SendAtCmd("ATE0",2);   //回显命令 0：MS 不回送从TE 接收的字符  1：MS 回送从TE 接收的字符。
			SetTimer(49,5000,NULL);
			index_resent_data_frame=0;//连接帧不支持重传机制
		}
		else
			MessageBox("无法打开3G串口，请重试！");	 
	}
	else
	{
		nATCmdID=0;//停止定时器的顺序操作
		SerialPortOpenCloseFlag_3G=FALSE;
		GetDlgItem(IDC_BUTTON_CONNECT3G)->SetWindowText("打开串口");
		m_StatBar->SetText("3G状态：串口已关闭",2,0); //以下类似
		m_ctrlIconOpenoff_3G.SetIcon(m_hIconOff);
		m_board_led_3G.SetIcon(m_hIconOff);
		m_3g_m_r.SetIcon(m_hIconOff);
		m_3g_p_r.SetIcon(m_hIconOff);
		state_system[1]=1;//3G串口关闭
		state_system[3]=1;
		GetDlgItem(IDC_STATIC_BOARDCONNECT_3G)->SetWindowText(" 3G已断开！");
		//		flag_com_init_ack_3G=0;//运维板未连接
		m_comm_3G.SetPortOpen(FALSE);//关闭串口
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
		m_comm_3G.SetOutput(COleVariant(Array));//发送数据		
		UpdateData(FALSE);
	}
}

void CBeidouDlg::OnComm_3G() 
{
	// TODO: Add your control notification handler code here
		 VARIANT variant_inp;
     COleSafeArray safearray_inp;
     LONG len,k;
     BYTE rxdata[2048]; //设置BYTE数组 An 8-bit integerthat is not signed.
     CString strtemp;

	 int nIndex = 0;//索引
     if(m_comm_3G.GetCommEvent()==2) //事件值为2表示接收缓冲区内有字符
     {////////以下你可以根据自己的通信协议加入处理代码
		 Sleep(100);
		 variant_inp=m_comm_3G.GetInput(); //读缓冲区
		 safearray_inp=variant_inp; //VARIANT型变量转换为ColeSafeArray型变量
		 len=safearray_inp.GetOneDimSize(); //得到有效数据长度
		 m_comm_3G.GetInput();    //先预读缓冲区以清除残留数据
		 int nTemp = strlen(cReceiveData);
		 for(k=0;k<len;k++)
			 safearray_inp.GetElement(&k,rxdata+k);//转换为BYTE型数组
		 nIndex = nTemp;
		 for(k=0;k<len;k++) //将数组转换为Cstring型变量
		 {
			 BYTE bt=*(char*)(rxdata+k); //字符型
			 cReceiveData[nIndex]=bt;
			 nIndex++;//经过调试，cReceiveData前两个数据为0D0A 数据在index=2处开始
			 strtemp.Format("%c",bt); //将字符送入临时变量strtemp存放
			 m_FKXX+=strtemp; //加入接收编辑框对应字符串 
			 UpdateData(FALSE); //更新编辑框内容
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

		

		if ((cReceiveData[2+0]=='R')&&(cReceiveData[2+1]=='I')&&(cReceiveData[2+2]=='N')&&(cReceiveData[2+3]=='G'))   //来电振铃
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
			}	//来电，自动应答，
			
			bIsCaller = FALSE;//变成被叫
			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("接听电话");			
			UpdateData(FALSE);
			OnButtonCall(); //先实现通话
			sound_switch(6);//切换到3G被叫模式
			SetTimer(31,AUTORESPONSE_TIME,NULL);//15s等待时间（等待用户输入，时间包括了播放语音的时间）
			PlaySound(".//Wav//Guide.wav", NULL, SND_FILENAME|SND_ASYNC);//应急电台系统，自动广播请按1，原始通话请按2,#号结束
			
		}
		else if (strtemp.Find("+CLIP") != -1)//来电显示
		{
			int	n=strtemp.FindOneOf("\"");
			strtemp.Delete(n,1);
			int	k=strtemp.FindOneOf("\"");
			call_in_number_3G = strtemp.Mid(n,k-n);
			m_target_number = call_in_number_3G;
			UpdateData(false);
		}
		else if(strtemp.Find("BEGIN")!=-1)//摘机回复指令
		{			 //^CONN: BEGIN 通话开始	主叫录音要写在自动应答里面
			m_FKXX+="3G摘机成功\r\n";
			if (bIsCaller == TRUE)  //主叫
			{
				if(!bIsRecord)//录音标志位为FALSE
				{
					bIsRecord = TRUE;
					ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);//3G开始录音
				}
				GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
				UpdateData(FALSE);
				bIsRecord=TRUE;
				SendAtCmd("AT+CLCC",0);
			}
		}
		else if(strtemp.Find("NO CARRIER")!=-1)//对方挂机回复指令
		{
			sound_switch(4);//3G结束通话，切回默认开关
			SetTimer(19,3000,NULL);//全部语音链路空闲时，定期结束广播
			SetTimer(10,(QUERY_INTERVAL+QUERY_3G),NULL);
			KillTimer(17);
			KillTimer(31);
			KillTimer(32);
			applay_broadcast(2);//申请终止广播
			KillTimer(18);//对方挂机则关闭定期振铃音
			m_StatBar->SetText("运维板状态：多链路广播终止已下发，未应答",4,0);
			ChuanHaoID = "";
			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("拨打电话");
			OnButtonClearNum();
			bRadioIsReady = FALSE;
			bIsAutoResponseProcess = FALSE;
			nCountCheck = 0;
			if (bIsRecord==TRUE)
			{	
			flag_3G_out_busy = 0;
			flag_3G_in_busy = 0;
			HWND hWnd = ::FindWindow(NULL, _T("record11"));//关闭录音
			if (NULL != hWnd) {
				::SendMessage(hWnd, WM_CLOSE, 0, 0);
			}	
			bIsCaller = TRUE;  
			bIsRecord=FALSE;
			}	
		}	 
		else if(strtemp.Find("CEND")!=-1)//同上
		{			 // ^CEND: 000015（代号） 通话结束
			sound_switch(4);//3G结束通话，切回默认开关
			SetTimer(19,3000,NULL);//全部语音链路空闲时，定期结束广播
			SetTimer(10,(QUERY_INTERVAL+QUERY_3G),NULL);
			flag_3G_out_busy = 0;
			flag_3G_in_busy = 0;
			bIsAutoResponseProcess = FALSE;
			nCountCheck = 0;
			KillTimer(17);
			KillTimer(31);
			KillTimer(32);
			OnButtonClearNum();
			applay_broadcast(2);//申请终止广播
			KillTimer(18);//对方挂机则关闭定期振铃音
			m_StatBar->SetText("运维板状态：多链路广播终止已下发，未应答",4,0);
			ChuanHaoID = "";
			UpdateData(TRUE);			
			 if (bIsRecord==TRUE)
			 {	
			//	OnButtonCall();	
				HWND hWnd = ::FindWindow(NULL, _T("record11"));//关闭录音
				if (NULL != hWnd) {
					::SendMessage(hWnd, WM_CLOSE, 0, 0);
				}	
				bIsCaller = TRUE;  
				bIsRecord=FALSE;
			 }
			 bRadioIsReady = FALSE;
			 GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("拨打电话");
		}
		else if(strtemp.Find("+CLCC")!=-1)//主动传号
		{			 			
			int	n=strtemp.Find("+CLCC:");
			int k=strtemp.Find(",");				
			ChuanHaoID = strtemp.Mid(n+7,k-(n+7)); //还要改
		}
		else if(strtemp.Find("+RXDTMF:")!=-1) //传号
		{			 //传号显示	 +RXDTMF: 1  			
			if((bIsCaller == FALSE)&&(bIsAutoResponseProcess == FALSE)) //只有被叫和处于自动应答过程中才对传号进行操作，进行广播或者虚拟摘机后就不处于自动应答过程中
			{		
				send_3Gstring+=cReceiveData[11];
				if(send_3Gstring.Find("#")!=-1)
				{		
					CString stre = send_3Gstring.Mid(0,1);//只取第一位
					send_3Gstring = stre;
					send_3Gstring += '#';//加上#号
					
					m_FKXX+="3G传号:";
					m_FKXX+=send_3Gstring;
					m_FKXX+="\r\n";
					UpdateData(FALSE);
					KillTimer(31);
					nCountCheck++;
					if (send_3Gstring == "1#")//自动广播
					{
						SetTimer(17,20000,NULL);
						bIsAutoResponseProcess = TRUE;
						applay_broadcast(1);//申请开始广播
						m_StatBar->SetText("运维板状态：多链路广播申请已下发，未应答",4,0); 
						//通知运维，广播请求
						PlaySound(".//Wav//BroadPrepare.wav", NULL, SND_FILENAME|SND_ASYNC);//广播正在准备中，请稍候   (4s)
					}     
					else if (send_3Gstring == "2#")//原本通话
					{
						PlaySound(".//Wav//RingConnect.wav", NULL, SND_FILENAME|SND_ASYNC);//通话正在连接中，请稍候   (5s)   //这里可以弄一段背景音乐(可重复播放)
						SetTimer(32,NOPICKUP_TIME ,NULL);//自定义振铃等待
						bIsAutoResponseProcess = TRUE;
						//向电台人员通知来电 
						SetTimer(18,4000,NULL);//每隔4s播放一次ring振铃音
						//这里OnButtonCall ，再添加一个状态标志，做虚拟摘机
						GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
						GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("虚拟摘机");
					}
					else
					{
						if (nCountCheck < 3)
						{
							PlaySound(".//Wav//ErrorCmd.wav", NULL, SND_FILENAME|SND_ASYNC);//指令出错，请重新输入
							SetTimer(31,AUTORESPONSE_TIME,NULL);//15s等待时间（等待用户输入，时间包括了播放语音的时间）
						} 
						else
						{										
							GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
							if((flag_3G_in_busy==1)||(flag_3G_out_busy==1)){
								GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
								OnButtonCall();
							}
						}
						
					}
					
					send_3Gstring="";
					UpdateData(FALSE);
				}	
			}
		}
					else if(strtemp.Find("+CMGL:")!=-1)//短信查询返回内容
					{		
						KillTimer(10);
							CString stra;
							int t = 0;
							int	n=strtemp.Find("+CMGL:");
							int k=strtemp.Find(",");				
							stra = strtemp.Mid(n+7,k-(n+7)); //还要改
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
								SetTimer(43,4000,NULL); //防止(5条以上)新短信的到来	
					 }
					else if(strtemp.Find("+CMGR:")!=-1)//读取短信命令
					 {
						bool bSMSToDecode = FALSE;//局部变量，表示是否进行解码
						if (bReadsmsTimeFlag == FALSE)//定时器没开启
						{	
							if (strtemp.Find("050003")!=-1)
							{		
								SetTimer(42,12000,NULL);  //短信查询在定时器42里面 
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

								if (nSMSCount > 0) //这里是为了存储还有长短信，发送AT+CMGL=4，nSMSCount>0（有可能是上次解码，没解全）
								{
									bSMSToDecode = TRUE;
								} 
								else //第一条长短信
								{
									bSMSToDecode = FALSE;
								}								
							}
							else if (strtemp.Find("060804")!=-1)
							{
								SetTimer(42,12000,NULL);  //短信查询在定时器42里面 
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
								
								if (nSMSCount > 0) //还有问题
								{
									bSMSToDecode = TRUE;
								} 
								else //第一条长短信
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
								AfxMessageBox("bReadsmsTimeFlag 出错");
							}

						}

						if (bSMSToDecode)//解码
						{
							CString stra;
							CString str2;
							int	n=strtemp.Find(":");
							int k=strtemp.Find("OK");
							if (k==-1)
								m_StatBar->SetText("3G状态: 短信数据结尾无OK",2,0); //以下类似
							else
							{
								str2 = strtemp.Mid(n+2,1);
								int h=strtemp.Find("\r\n",n);
								stra = strtemp.Mid(h+2,k-4-(h+2));//已经取出，只需变成char，解码
								n=0;
								n=atoi(str2);
								char *pSrc = (LPSTR)(LPCTSTR)stra;					
								sms = ReadSMS(pSrc,sms,n);
								if(bTimer48OnOff == FALSE)
								{
									bTimer48OnOff = TRUE;
									SetTimer(48,120*1000,NULL);//短信解码两分钟没回，开启CSQ
								}
								SetTimer(41,500,NULL);
							}
						}
					 }
					else if(strtemp.Find("+CMTI:")!=-1)//新短信
					{
						KillTimer(10);						
						if ((bIsNewSMS == FALSE)&&(bReadsmsTimeFlag == FALSE)) //无新短信，且定时器没开启
						{
							bIsNewSMS = TRUE;

							m_StatBar->SetText("3G状态: 新短信",2,0); //以下类似
							int k=strtemp.Find(",");				
							strShortSMSIndex = strtemp.Mid(k+1,1); //还要改							
							SendAtCmd("AT+CMGR="+strShortSMSIndex,32);//读取指定短信   AT+CMGR 32
						}
						else
						{
							//不做任何事情
						}							
					}
					else if(strtemp.Find(">")!=-1) //发送短信：PDU数据的发送在定时器41里面
					{
					//	SetTimer(40,1000,NULL);
						nATCmdID = 23;			
					}	
					
					else if(strtemp.Find("CSQ")!=-1)//查询信号强度
					{			 //AT+CSQ
						timer_board_disconnect_times_3G=0;//标记3G可用
						if((tel_manual_select==0)||(m_tel_manual_select.GetCurSel()==2)){//电话部分，自动或选中3G时允许修改
							state_system[1]=0;//3G超时：标记3G故障
						}
						if((msg_manual_select==0)||(m_msg_manual_select.GetCurSel()==1)){//短信部分，自动或选中3G时允许修改
							state_system[3]=0;
						}
						m_board_led_3G.SetIcon(m_hIconRed);
						m_3g_m_r.SetIcon(m_hIconRed);
						m_3g_p_r.SetIcon(m_hIconRed);
						GetDlgItem(IDC_STATIC_BOARDCONNECT_3G)->SetWindowText(" 3G已连接！");
						m_FKXX+="3G信号查询：正常\r\n";
						UpdateData(FALSE);

						CString str3;
						int	n=strtemp.FindOneOf(":");
						int	k=strtemp.FindOneOf(",");
						str3 = strtemp.Mid(n+2,k-(n+2));
						int j = 0;
						j = atoi(str3);
						if ((j<=8)&&(j>0))
						{
							this->GetDlgItem(IDC_RSSI)->SetWindowText("信号强度: 差");
						} 
						else if ((j>=9)&&(j<=14))
						{
							this->GetDlgItem(IDC_RSSI)->SetWindowText("信号强度: 中");
						}
						else
						{
							if (strtemp.Find("99,")!=-1)
							{
								this->GetDlgItem(IDC_RSSI)->SetWindowText("信号未知");
							}
							else
							{
								this->GetDlgItem(IDC_RSSI)->SetWindowText("信号强度: 好");
							}
						}
					//	str3 = strtemp.Mid(k-2,5);
						UpdateData(false);
					}
					else if((strtemp.Find("ERROR")!=-1)&&(bIsINIProcess == TRUE))//初始化返回错误
					{			 
						bINIFail = FALSE;
					}
					else if((strtemp.Find("OK")!=-1)&&(41 == nATCmdID))
					{
						nATCmdID = 0;
						bIsINIProcess = FALSE;
					}
					else if(strtemp.Find("CMS ERROR")!=-1) //短信错误返回
					{			 //CMS
						CString str4;
						int	n=strtemp.FindOneOf("CMS ERROR");
						str4 = strtemp.Mid(n,14);
						m_FKXX+=str4;
						UpdateData(false);
						//取数字，然后进行相应的取值操作
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
					else if(strtemp.Find("+CMGS")!=-1) //短信发送成功，进行短信保存  
					{
						CString str,strDateTime;
						CTime m_Time;
						m_Time = CTime::GetCurrentTime();
						strDateTime = m_Time.Format(_T("%Y/%m/%d %H:%M:%S %A"));
						UpdateData(TRUE);
						str = "收件人:";
						str += m_target_number;
						str += "\r\n";
						char pSrc[1500]; 
						strncpy(pSrc,(LPCTSTR)str,str.GetLength());
						CFile m_File;
						m_File.Open(".//SendSMS.txt",CFile::modeCreate|CFile::modeReadWrite|CFile::modeNoTruncate|CFile::typeBinary|CFile::shareDenyNone,NULL);  
						m_File.SeekToEnd();
						m_File.Write(pSrc,sizeof(char)*str.GetLength());
						str = "时间:";
						str += strDateTime;
						str += "\r\n";
						strncpy(pSrc,(LPCTSTR)str,str.GetLength());
						m_File.SeekToEnd();
						m_File.Write(pSrc,sizeof(char)*str.GetLength());
						str = "内容:";
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
							m_StatBar->SetText("3G状态: 短信发送成功",2,0); //以下类似
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
	smsb = nDecodePdu(pSrc,smsb,nTxRxFlag);//短信解码
	int num = 0;
	m_StatBar->SetText("3G状态: 读取短信中,请稍候",2,0); //以下类似
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
		strLongSMSTextStore[pDst->index - 1] = pDst->TP_UD;//从第一条开始，这里从0开始存
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
		//开始解码长短信后两分钟，自动开启查询CSQ

			if(smsb.nSMSTotalNum == num)
			{//tts，检查在tts里面
		//		TTSModule(strLongSMSText);
		//		m_StatBar->SetText("3G状态：串口已连接",2,0); //以下类似
				strLongSMSText += "\r\n";
				bTimer48OnOff = FALSE;
				KillTimer(48);
				SetTimer(10,(QUERY_INTERVAL+QUERY_3G),NULL);//短信读完了，再打开3G查询定时器
				
				m_showmsg+=strTime;
				m_showmsg+="从";
				m_showmsg += pDst->TPA;
				m_showmsg+="收到信息：\r\n";
				m_showmsg += strLongSMSText;
				UpdateData(FALSE);
//				TTSModule(strLongSMSText);
				ttsStruct.nPlayTimers = 1; //暂时播放两次
				ttsStruct.strTransmit = (LPCSTR)strLongSMSText;
//				pThread=AfxBeginThread(ThreadProc, &ttsStruct);
				pThread=AfxBeginThread(ThreadProc, this);
				m_StatBar->SetText("3G状态：串口已连接",2,0); //以下类似
			}			
	} 
	else
	{
		strLongSMSText = pDst->TP_UD;
		strLongSMSText += "\r\n";
		bTimer48OnOff = FALSE;
		KillTimer(48);
		SetTimer(10,(QUERY_INTERVAL+QUERY_3G),NULL);//短信读完了，再打开3G查询定时器

		
		m_showmsg+=strTime;
		m_showmsg+="从";
		m_showmsg += pDst->TPA;
		m_showmsg+="收到信息：\r\n";
		m_showmsg += strLongSMSText;
		UpdateData(FALSE);
//		TTSModule(strLongSMSText);
		ttsStruct.nPlayTimers = 1; //暂时播放两次
		ttsStruct.strTransmit = (LPCSTR)strLongSMSText;
//		pThread=AfxBeginThread(ThreadProc, &ttsStruct);
		pThread=AfxBeginThread(ThreadProc, this);
		m_StatBar->SetText("3G状态：串口已连接",2,0); //以下类似
	}	


	CString strToSave;
	char pH[1200];
	CFile m_File;
	strToSave = "发件人:";
	strToSave += call_in_number_3G + "\r\n";	 
	strncpy(pH,(LPCTSTR)strToSave,strToSave.GetLength());	
	m_File.Open(".//ReceiveSMS.txt",CFile::modeCreate|CFile::modeReadWrite|CFile::modeNoTruncate|CFile::typeBinary|CFile::shareDenyNone,NULL); 
	m_File.SeekToEnd();
	m_File.Write(pH,sizeof(char)*strToSave.GetLength());
	if (!smsb.bIsLongSMS)
	{
		strToSave = "时间:";
		strToSave += strTime + "\r\n";
		strncpy(pH,(LPCTSTR)strToSave,strToSave.GetLength());
		m_File.SeekToEnd();
		m_File.Write(pH,sizeof(char)*strToSave.GetLength());
	}	
	strToSave = "内容:";
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
		if (m_tel_manual_select.GetCurSel()==0)//自动
		{
			tel_manual_select=0;//自动模式
			/*查询有线电话*/
			char lpOutBuffer[] = {'A','T','H','\r','\n'};//接着上传ATH指令进行挂机
			CByteArray Array;
			Array.RemoveAll();    
			Array.SetSize(5);		
			
			for (int i=0; i<5; i++)
			{
				Array.SetAt(i,lpOutBuffer[i]);
			}
			if(m_comm_WT.GetPortOpen())
			{
				m_comm_WT.SetOutput(COleVariant(Array));//发送数据
			}
			/*查询3G*/
			SendAtCmd("AT+CSQ",6);      //查询CSQ
			/*查询卫星电话*/
			if (SerialPortOpenCloseFlag_ST==TRUE)//只有在串口已经打开时才查询
			{
				CString msg = "AT+RINFO\r\n";
				m_Port_ST.WriteToPort((LPCTSTR)msg);	//发送数据
			}

		} 
		else if (m_tel_manual_select.GetCurSel()==1)//有线电话模式：标记3G卫星均不可用，但有线电话可用与否看能否查询到。下同。
		{
			char lpOutBuffer[] = {'A','T','H','\r','\n'};//接着上传ATH指令进行挂机
			CByteArray Array;
			Array.RemoveAll();    
			Array.SetSize(5);		
			
			for (int i=0; i<5; i++)
			{
				Array.SetAt(i,lpOutBuffer[i]);
			}
			if(m_comm_WT.GetPortOpen())
			{
				m_comm_WT.SetOutput(COleVariant(Array));//发送数据
			}

//			KillTimer(4);
			tel_manual_select=1;//手动模式
//			state_system[0]=0;
			state_system[1]=1;
			state_system[2]=1;

		}else if (m_tel_manual_select.GetCurSel()==2)//3G电话
		{
			SendAtCmd("AT+CSQ",6);      //查询CSQ

//			KillTimer(10);
			tel_manual_select=1;//手动模式
			state_system[0]=1;
//			state_system[1]=0;
			state_system[2]=1;

		}else if (m_tel_manual_select.GetCurSel()==3)//卫星电话
		{
			if (SerialPortOpenCloseFlag_ST==TRUE)//只有在串口已经打开时才查询
			{
				CString msg = "AT+RINFO\r\n";
				m_Port_ST.WriteToPort((LPCTSTR)msg);	//查询卫星电话的定位
			}

//			KillTimer(12);
			tel_manual_select=1;//手动模式
			state_system[0]=1;
			state_system[1]=1;
	//		state_system[2]=0;

		}
		tel_mode_saved=m_tel_manual_select.GetCurSel();//保存正确的选项
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
	CString strPart = "";//用于截取字符串
	CString order ="";
	CString totalOrder = "";
	funReturn d = {FALSE,0};//默认为FALSE,0字符
	order.Format("%d",n+1);
	totalOrder.Format("%d",structUse->nCount);//最多为9， 长短信一般最长为5
	if (1==nLongSMSSendCount)
	{
//		strPart = m_sendmsg.Mid(structUse->nStrSendLoc,m_sendmsg.GetLength()-nTotalNum);
		strPart = m_sendmsg.Mid(structUse->nStrSendLoc,m_sendmsg.GetLength()-structUse->nStrSendLoc);
		strCount countNum = Statistic(strPart);//字符统计
		nLongSMSSendCount --;
		d = cEncodeLongPDU(structUse->strDstNum,strPart,chPDU,structUse->bChineseFlag,countNum.nTotalLenth,order,totalOrder,structUse->nLongSMSID);//收件人，内容  //默认是中文
		if (d.bToReturn)
		{
			if(d.nLenthToReturn <=200)
			{
				CString strTempc = "";
				SendAtCmd("AT+CMGF=0",21); //PDU模式
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
				m_StatBar->SetText("3G状态: 短信发送出错",2,0); //以下类似
			}
			
		}
		else
		{
			longSMSStructINI();
			m_StatBar->SetText("3G状态: 短信发送失败,检查后重试",2,0); //以下类似
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
	//	strCount countNum = Statistic(strPart);//字符统计
		nLongSMSSendCount --;
		//	d = cEncodeLongPDU(structUse.strDstNum,strPart,chPDU,structUse.bChineseFlag,countNum.nTotalLenth,order,totalOrder);//收件人，内容  //默认是中文
		d = cEncodeLongPDU(structUse->strDstNum,strPart,chPDU,structUse->bChineseFlag,countNum.nTotalLenth,order,totalOrder,structUse->nLongSMSID);//收件人，内容  //默认是中文
		if (d.bToReturn)
		{
			if (d.nLenthToReturn <= 200)
			{
				CString strTempd = "";
				SendAtCmd("AT+CMGF=0",21); //PDU模式
				strTempd.Format("%d",d.nLenthToReturn);
				SendAtCmd("AT+CMGS="+strTempd,22);
				nATCmdID = 23;
				SetTimer(46,1000,NULL);
				//定时器47在串口发送成功里面
			} 
			else
			{
				longSMSStructINI();
				m_StatBar->SetText("3G状态: 短信发送出错",2,0); //以下类似
			}
		}
		else
		{
			longSMSStructINI();
			m_StatBar->SetText("3G状态: 短信发送失败，检查后重试",2,0); //以下类似
		}
	}
}

void CBeidouDlg::OnSelendokComboMsgManualSelect() 
{
	// TODO: Add your control notification handler code here
	if (m_msg_manual_select.GetCurSel()==0)//自动
	{
		/*查询3G*/
		SendAtCmd("AT+CSQ",6);      //查询CSQ
		/*查询北斗*/
		CByteArray Array;//选择北斗模式，立刻查询北斗是否可用
		Array.RemoveAll();
		Array.SetSize(12);
		for (int i=0; i<12; i++)
		{
			Array.SetAt(i,frame_IC_check[i]);
		}		
		if(m_comm.GetPortOpen())
		{
			m_comm.SetOutput(COleVariant(Array));//发送数据
		}


		msg_manual_select=0;
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
	} 
	else if(m_msg_manual_select.GetCurSel()==1)//3G短信：将北斗报文链路设置为不可用，至于3G短信可用与否，看定期查询结果。
	{
		SendAtCmd("AT+CSQ",6);      //查询CSQ
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);

		msg_manual_select=1;//手动
		state_system[4]=1;
	}
	else if(m_msg_manual_select.GetCurSel()==2)//北斗报文
	{
		CByteArray Array;//选择北斗模式，立刻查询北斗是否可用
		Array.RemoveAll();
		Array.SetSize(12);
		for (int i=0; i<12; i++)
		{
			Array.SetAt(i,frame_IC_check[i]);
		}		
		if(m_comm.GetPortOpen())
		{
			m_comm.SetOutput(COleVariant(Array));//发送数据
		}

		msg_manual_select=1;//手动
		state_system[3]=1;
		if (flag_bd_is_busy==1)//发送中时，将按钮再次失能。用于：点击北斗发送，切换到3G在切回北斗的情况。
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
	BOOL bToRadio = CheckString(str);//检测字符串中是否有特殊字符双引号
	if (bToRadio) //为真广播
	{		//自动广播，这里暂时只用一个弹窗代替
		bFlag = ttsSpeak(str);//tts,语音输出
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
	strForTTS = "@@@@我是应急广播系统，为省、地以及街道政府职能部门在应急事件中实现应急指挥调度和应急广播服务管理提供技术支持，并面向广大用户提供无线电广播模式的灾害预警、事件告警、救援指导和政策宣传等广播服务。";
//	strForTTS = "@@@@打狗棍，打狗得看主人啊，是吧。小洋人，乳酸菌。我咽不下那口恶气，胖子是我兄弟。";
	//	TTSModule(strTTS);	
	ttsStruct.nPlayTimers = 1; //暂时播放两次
	ttsStruct.strTransmit = (LPCSTR)strForTTS;
//	pThread=AfxBeginThread(ThreadProc, &ttsStruct);
	if (bIsTTSPlay == FALSE) //未开始播放TTS
	{
		bIsTTSPlay = TRUE;
		pThread=AfxBeginThread(ThreadProc, this);
	} 
	else //TTS播放中
	{
		AfxMessageBox("TTS正在进行中，请稍后再试");
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
		pBeiDouDlg->applay_broadcast(3);//TTS申请打开广播
		pBeiDouDlg->SetTimer(23,2000,NULL);//每隔两秒查询TTS是否播放完毕，如果未完，继续查询；如果完毕则结束广
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

void CBeidouDlg::applay_broadcast(int n)//1:申请广播；2：停止广播；3:TTS申请广播;
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
		frame_board_ppp[7]++;//如果异或结果是$或0x0d，则值加一
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
		m_comm_YW.SetOutput(COleVariant(Array));//发送数据
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
			GetDlgItem(IDC_BUTTON_CONNECTST)->SetWindowText("关闭串口");
			m_StatBar->SetText("卫星电话状态：串口已打开",1,0); //以下类似
			UpdateData();
			
			//卫星电话初始化
			nATCmdFlags = 100;
			BISInitial_WX = TRUE;
			BINIfail_WX = TRUE;
			Send_AT_Command_ST("AT");
			SetTimer(59,5000,NULL);
			index_resent_data_frame=0;//连接帧不支持重传机制		
		}else{
			MessageBox("无法打开卫星电话串口，请重试！");
			SerialPortOpenCloseFlag_ST=FALSE;
		}
	}else{
			m_Port_ST.ClosePort();//关闭串口
			
			SerialPortOpenCloseFlag_ST=FALSE;
			GetDlgItem(IDC_BUTTON_CONNECTST)->SetWindowText("打开串口");
			GetDlgItem(IDC_STATIC_BOARDCONNECT_ST)->SetWindowText(" 卫星电话已关闭！");
			UpdateData();

			m_ctrlIconOpenoff_ST.SetIcon(m_hIconOff);
			m_board_led_ST.SetIcon(m_hIconOff);
			m_st_r.SetIcon(m_hIconOff);
			state_system[2]=1;//卫星电话串口关闭，状态设为不可用
			KillTimer(20);//停止定期查询
			KillTimer(55);//返回error也停止初始化
			KillTimer(59);//停止初始化
			KillTimer(50);//停止初始化
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
	int nIndex = 0;//索引
	static bool ST_CSQ_avilable=0;//卫星电话CSQ是否有信号。0：无信号；1：有信号；
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
			ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);//开始录音
			bIsRecording = TRUE;
		}
			strdata="";
	}
	else if (strdata.Find("RING")!=-1) //这个得测试下
	{	
		KillTimer(20);//停止定期查询
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
				this->GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("接听");	
			}
			strdata = "";
		}
	}
	else if (strdata.Find("CLIP:") != -1)//监听来电
	{
		KillTimer(20);//停止定期查询
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
				GetDlgItem(IDC_ST_RSSI)->SetWindowText("卫星电话信号: 差");
				ST_CSQ_avilable=1;//可用
			} 
			else if ((j>=24)&&(j<=28))
			{
				GetDlgItem(IDC_ST_RSSI)->SetWindowText("卫星电话信号: 中");
				ST_CSQ_avilable=1;//可用
			}
			else
			{
				if (strtemp.Find("99,")!=-1)
				{
					GetDlgItem(IDC_ST_RSSI)->SetWindowText("卫星电话信号未知。");
					ST_CSQ_avilable=0;//不可用
				}
				else
				{
					GetDlgItem(IDC_ST_RSSI)->SetWindowText("卫星电话信号: 好");
					ST_CSQ_avilable=1;//可用
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
			GetDlgItem(IDC_ST_POSITION)->SetWindowText("卫星电话定位：成功");
			if (ST_CSQ_avilable==1)//CSQ有信号时
			{
				state_system[2]=0;//标记卫星电话可用
			}else{
				state_system[2]=1;//CSQ无信号时，标记卫星电话不可用
			}
			
		} 
		else
		{
			GetDlgItem(IDC_ST_POSITION)->SetWindowText("卫星电话定位：失败");
			state_system[2]=1;//标记卫星电话不可用
		}
		strdata="";
	}
	else if (strdata.Find("+GPSTRACKD")!=-1)
	{ //先回OK
		int i = strdata.Find("78");
		int j = strdata.Find("48");
		int k = strdata.Find("79");
		if (i!=-1)
		{
			m_StatBar->SetText("卫星电话状态：GPS已定位",2,0);
			strdata=""; 
		}
		else if (k!=-1)
		{
			m_StatBar->SetText("卫星电话状态：GPS已定位",2,0);
			strdata="";
		}
		else if(j!=-1)
		{
			m_StatBar->SetText("卫星电话状态：GPS未定位",2,0);
			strdata="";	
		}
	}
	else if (strdata.Find("+RCIPH:")!=-1)
	{//更新状态栏，已播出
		m_StatBar->SetText("卫星电话状态：拨号中......",2,0);
		strdata="";
	}
	else if(strdata.Find("ERROR")!=-1)//初始化返回错误
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
	else if (strdata.Find("CME ERROR")!=-1)//这里还要改！！！！！！！！！！！！！！
	{
		CString str;
		this->GetDlgItemText(IDC_BUTTON_CALL,str);
		if (str=="挂机")
		{
			if((flag_ST_in_busy==1)||(flag_ST_out_busy==1))OnButtonCall();
		}
		m_FKXX += strdata + "\r\n";
		strdata ="";
	}
	else if((strdata.Find("NO CARRIER")!=-1)||(strdata.Find("BUSY")!=-1)||(strdata.Find("NO ANSWER")!=-1))//挂机
	{
		CString str;
		this->GetDlgItemText(IDC_BUTTON_CALL,str);
		if (str=="挂机")
		{
			if((flag_ST_in_busy==1)||(flag_ST_out_busy==1))OnButtonCall();
		}

		if (bIsRecording)//未关闭就关闭
		{
			bIsRecording = FALSE;
			//关闭录音
			HWND hWnd = ::FindWindow(NULL, _T("record11"));//要改，不是record11
			if (NULL != hWnd) {
				::SendMessage(hWnd, WM_CLOSE, 0, 0);
			}
		}
		strdata="";	
		SetTimer(20,(QUERY_INTERVAL+QUERY_ST),NULL);
	}
	UpdateData(FALSE); //更新编辑框内容	
	return 0;
}

void CBeidouDlg::OnGPS() 
{
	// TODO: Add your control notification handler code here
	if (SerialPortOpenCloseFlag_ST==TRUE)//只有在串口已经打开时才查询
	{
		CString msg = "AT+GPSTRACK=1\r\n";
		m_Port_ST.WriteToPort((LPCTSTR)msg);	//发送数据
	}
}

void CBeidouDlg::On_CSQ_ST() 
{
	// TODO: Add your control notification handler code here
	if (SerialPortOpenCloseFlag_ST==TRUE)//只有在串口已经打开时才查询
	{
		CString msg = "AT+CSQ\r\n";
		m_Port_ST.WriteToPort((LPCTSTR)msg);	//发送数据
	}
}

void CBeidouDlg::On_Position_ST() 
{
	// TODO: Add your control notification handler code here
	if (SerialPortOpenCloseFlag_ST==TRUE)//只有在串口已经打开时才查询
	{
		CString msg = "AT+RINFO\r\n";
		m_Port_ST.WriteToPort((LPCTSTR)msg);	//发送数据
	}
}

void CBeidouDlg::Send_AT_Command_ST(CString msg)
{
	if (SerialPortOpenCloseFlag_ST==TRUE)
	{
		msg+="\r\n";
		m_FKXX += msg;
		int nLentx = 0;
		m_Port_ST.WriteToPort((LPCTSTR)msg);	//发送数据
		UpdateData(FALSE);
	}
	
}
