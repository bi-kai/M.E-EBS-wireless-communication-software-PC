// beidouDlg.cpp : implementation file
//

#include "stdafx.h"
#include "beidou.h"
#include "beidouDlg.h"
#include <math.h> 
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"WINMM.LIB")
#include "autoResponse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


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
#define SendBufferSize 200
#define received_frame_size 5//缓冲区数组个数
bool frameplus=0;//0:$为第一位；1：$在串的中间

#define BORD_LEFT_HIDE 500//高级配置，隐藏的界面左区域
#define BORD_RIGHT_HIDE 400//高级配置，隐藏的界面右区域

unsigned char frame_IC_check[12]={0x24,0x49,0x43,0x4A,0x43,0x00,0x0C,0x00,0x00,0x00,0x01,0x2A};//IC查询
unsigned char frame_SYS_check[11]={0x24,0x53,0x74,0x73,0x5F,0x00,0x0B};//系统检测
unsigned char frame_POWER_check[12]={0x24,0x47,0x4C,0x4A,0x43,0x00,0x0C,0x00,0x00,0x00,0x01,0x2B};//功率查询
unsigned char frame_MSG_check[250]={0x24,0x54,0x58,0x53,0x51};//通信申请

unsigned char frame_receive[received_frame_size][2000]={0};

/*************有线电话******************************/
unsigned char frame_receive_WT[2000]={0};

/***********运维板***************/
unsigned char frame_board_check_YW[7+2]={'$','r','e','y','_'};//运维连接检测帧
unsigned char frame_board_reset_YW[8+2]={'$','r','s','e','_'};//运维复位帧
unsigned char frame_board_sound_YW[8+2]={'$','s','w','h','_'};//运维切换音频开关申请帧
unsigned char frame_board_connect_YW[7+2]={'$','p','p','p','_'};//运维上位机两个软件相互查询帧
unsigned char frame_board_scan_YW[7+2]={'$','s','c','a','_'};//运维频谱扫描，继电器控制帧

#define TIMER2_MS 10000//定时器2中断间隔，10s查询一次子板的连接状况
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
	m_otherID = 0;
	m_target_number = _T("0");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	SerialPortOpenCloseFlag=FALSE;//默认关闭串口
	SerialPortOpenCloseFlag_WT=FALSE;//默认关闭有线电话的串口
}

void CBeidouDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBeidouDlg)
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
	DDX_Control(pDX, IDC_COMBO_STOPBITS, m_StopBits);
	DDX_Control(pDX, IDC_COMBO_SPEED, m_Speed);
	DDX_Control(pDX, IDC_COMBO_PARITY, m_Parity);
	DDX_Control(pDX, IDC_COMBO_DATABITS, m_DataBits);
	DDX_Control(pDX, IDC_COMBO_COMSELECT, m_Com);
	DDX_Control(pDX, IDC_MSCOMM1, m_comm);
	DDX_Text(pDX, IDC_EDIT_RECEIVEMSG, m_showmsg);
	DDX_Text(pDX, IDC_EDIT_SENDMSG, m_sendmsg);
	DDV_MaxChars(pDX, m_sendmsg, 210);
	DDX_Text(pDX, IDC_EDIT_CARDNUMBER, m_cardnumber);
	DDX_Text(pDX, IDC_EDIT_CATEGORY, m_category);
	DDX_Text(pDX, IDC_EDIT_CARDSTATES, m_cardstate);
	DDX_Text(pDX, IDC_EDIT_BASESTATES, m_basestate);
	DDX_Text(pDX, IDC_EDIT_FKXX, m_FKXX);
	DDX_Text(pDX, IDC_EDIT5, m_otherID);
	DDV_MinMaxInt(pDX, m_otherID, 0, 16777215);
	DDX_Text(pDX, IDC_EDIT_TARGETNUM, m_target_number);
	DDX_Control(pDX, IDC_MSCOMM_WT, m_comm_WT);
	DDX_Control(pDX, IDC_MSCOMM_YW, m_comm_YW);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBeidouDlg, CDialog)
	//{{AFX_MSG_MAP(CBeidouDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPENCLOSEPORT, OnOpencloseport)
	ON_CBN_SELENDOK(IDC_COMBO_COMSELECT, OnSelendokComboComselect)
	ON_CBN_SELENDOK(IDC_COMBO_SPEED, OnSelendokComboSpeed)
	ON_CBN_SELENDOK(IDC_COMBO_PARITY, OnSelendokComboParity)
	ON_CBN_SELENDOK(IDC_COMBO_DATABITS, OnSelendokComboDatabits)
	ON_CBN_SELENDOK(IDC_COMBO_STOPBITS, OnSelendokComboStopbits)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSendMsg)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_SYSTEMCHECK, OnButtonSystemcheck)
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

	m_hIconRed  = AfxGetApp()->LoadIcon(IDI_ICON_RED);
	m_hIconOff	= AfxGetApp()->LoadIcon(IDI_ICON_OFF);
	GetDlgItem(IDC_COMBO_STOPBITS2)->SetWindowText(_T("代码"));

	GetDlgItem(IDC_BUTTON_SYSTEMCHECK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ICCHECK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3_POWERCHECK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(FALSE);

	GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(FALSE);
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
/********************2、有线电话串口配置***********************************/	
	m_comm_WT.SetCommPort(2); //选择com2
	m_comm_WT.SetInputMode(1); //输入方式为二进制方式
	m_comm_WT.SetInBufferSize(1024); //设置输入缓冲区大小
	m_comm_WT.SetOutBufferSize(10240); //设置输出缓冲区大小
	m_comm_WT.SetSettings("1200,n,8,1"); //波特率1200，无校验，8个数据位，1个停止位	 
	m_comm_WT.SetRThreshold(1); //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件
	m_comm_WT.SetInputLen(0); //设置当前接收区数据长度为0
	//	 m_comm_WT.GetInput();    //先预读缓冲区以清除残留数据
/********************3、运维串口配置***********************************/	
	m_comm_YW.SetCommPort(3); //选择com3
	m_comm_YW.SetInputMode(1); //输入方式为二进制方式
	m_comm_YW.SetInBufferSize(1024); //设置输入缓冲区大小
	m_comm_YW.SetOutBufferSize(10240); //设置输出缓冲区大小
	m_comm_YW.SetSettings("115200,n,8,1"); //波特率1200，无校验，8个数据位，1个停止位	 
	m_comm_YW.SetRThreshold(7); //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件
	m_comm_YW.SetInputLen(0); //设置当前接收区数据长度为0
	//	 m_comm_WT.GetInput();    //先预读缓冲区以清除残留数据
/**********************************************************************/
	frame_lock=0;//缓冲帧使用锁，0：允许用；1：禁止使用
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
	GetDlgItem(IDC_BUTTON_CALL)->ShowWindow(SW_SHOW);

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
		::WritePrivateProfileString("ConfigInfo","com","0",".\\config_phonemessage.ini");//串口配置选项组
		::WritePrivateProfileString("ConfigInfo","com_WT","1",".\\config_phonemessage.ini");//串口配置选项组
		::WritePrivateProfileString("ConfigInfo","com_YW","2",".\\config_phonemessage.ini");//串口配置选项组
		::WritePrivateProfileString("ConfigInfo","parity","0",".\\config_phonemessage.ini");
		::WritePrivateProfileString("ConfigInfo","databits","0",".\\config_phonemessage.ini");
		::WritePrivateProfileString("ConfigInfo","speed","5",".\\config_phonemessage.ini");
		::WritePrivateProfileString("ConfigInfo","stopbits","0",".\\config_phonemessage.ini");
		
		::WritePrivateProfileString("ConfigInfo","com_r","1",".\\config_phonemessage.ini");//串口配置数值组
		::WritePrivateProfileString("ConfigInfo","com_r_WT","2",".\\config_phonemessage.ini");//串口配置选项组
		::WritePrivateProfileString("ConfigInfo","com_r_YW","3",".\\config_phonemessage.ini");//串口配置选项组
		::WritePrivateProfileString("ConfigInfo","parity_r","N",".\\config_phonemessage.ini");
		::WritePrivateProfileString("ConfigInfo","databits_r","8",".\\config_phonemessage.ini");
		::WritePrivateProfileString("ConfigInfo","speed_r","115200",".\\config_phonemessage.ini");
		::WritePrivateProfileString("ConfigInfo","stopbits_r","1",".\\config_phonemessage.ini");		
	}

	/**********串口配置**********************/
	CString strBufferReadConfig,strtmpReadConfig;

	GetPrivateProfileString("ConfigInfo","com_r","1",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
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
	
	GetPrivateProfileString("ConfigInfo","parity_r","N",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	m_DParity= (char)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);
	
	GetPrivateProfileString("ConfigInfo","databits_r","8",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	m_DDatabits= (int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);
	
	GetPrivateProfileString("ConfigInfo","speed_r","115200",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	m_DBaud= (long)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);
	
	GetPrivateProfileString("ConfigInfo","stopbits_r","1",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	m_DStopbits= (int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);
	
	///////////////////////////////////////////////////////////////////////////
	GetPrivateProfileString("ConfigInfo","com","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
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

	GetPrivateProfileString("ConfigInfo","parity","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_PARITY))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//设置第n行内容为显示的内容。
	
	GetPrivateProfileString("ConfigInfo","databits","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_DATABITS))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//设置第n行内容为显示的内容。
	
	GetPrivateProfileString("ConfigInfo","speed","5",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_SPEED))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//设置第n行内容为显示的内容。
	
	GetPrivateProfileString("ConfigInfo","stopbits","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_STOPBITS))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//设置第n行内容为显示的内容。
	
	UpdateData(FALSE);
	/*********************************************************************/
	switch_state=0;//打电话
	WT_state=0;//电话机状态，初始化置为空闲
	flag_PW_in_busy=0;//初始值
	flag_PW_out_busy=0;//初始值
	
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
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void CBeidouDlg::OnComm1() 
{
	// TODO: Add your control notification handler code here
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len,k;
	BYTE rxdata[2048]; //设置BYTE数组
	CString strDisp="",strTmp="";

	if((m_comm.GetCommEvent()==2)) //事件值为2表示接收缓冲区内有字符
	{	
//		Sleep(100);//一帧较长，为避免接收半帧就触发响应函数，造成未收完的判断而添加大量保护代码。
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
//			frame_lock=0;
//			frame_len[frame_index]=0;


		for(k=0;k<len;k++)//将数组转化为CString类型
		{
			BYTE bt=*(char*)(rxdata+k);    //字符型
			if (bt=='$')
			{
				if (rxdata[0]=='$')
				{
					frameplus=0;
				} 
				else//$出现在流的中间位置
				{
					frameplus=1;
					frame_index++;//上一条已接收，需要处理
				}

				if (frame_index!=0)
				{
	//				frame_index++;//新的一帧到来
					if(frameplus==1)
					{
						frame_flag[frame_index-1]=1;
						frame_len[frame_index-1]=framelen;
						framelen=0;
//						unsigned char* frame_buf1=frame_receive[frame_index-1];
						for (short i=0;i<received_frame_size;i++)
						{
							if (frame_flag[i]==1)
							{
								decodeheads (frame_receive[i]);
								frame_flag[i]=0;//标记为未使用
							}
						}
						
					}
					if(frame_index==received_frame_size)	frame_index=0;
				}
				
			}
			frame_receive[frame_index][framelen]=bt;
			framelen++;
			strTmp.Format("%02x ",bt);    //将字符送入临时变量strtemp存放
			strDisp+=strTmp;  //加入接收编辑框对应字符串
			
		}
		if (framelen==(frame_receive[frame_index][5]*256+frame_receive[frame_index][6]))
		{
			//帧接收完成
// 			CString buf;
//			buf.Format("%d",frame_index);
// 			AfxMessageBox(buf,MB_OK,0);

			frame_len[frame_index]=framelen;
			framelen=0;
			frame_flag[frame_index]=1;
//			unsigned char* frame_buf=frame_receive[frame_index];
			for (short i=0;i<received_frame_size;i++)
			{
				if (frame_flag[i]==1)
				{
					decodeheads (frame_receive[i]);
					frame_flag[i]=0;//标记为未使用
				}
			}
//			frame_index++;//收到完整的一帧
			if(frame_index==received_frame_size) frame_index=0;
			
		}
//		m_showmsg+=strDisp;
//		m_showmsg+="\r\n";
		UpdateData(FALSE);
	}	
}



void CBeidouDlg::OnOpencloseport() 
{
	// TODO: Add your control notification handler code here
	char buff[2];
	CString string1="",string2="";
	buff[1]='\0';
	buff[0]=m_DParity;
	string1.Format(_T("%d"),m_DBaud);
	string1+=",";
	string2=buff;
	string1+=string2;
	string1+=",";
	string2.Format(_T("%d"),m_DDatabits); 
	string1+=string2;
	string1+=",";
	string2.Format(_T("%d"),m_DStopbits);
	string1+=string2;
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
			m_StatBar->SetText("北斗：已连接",3,0);
			GetDlgItem(IDC_BUTTON_ICCHECK)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SYSTEMCHECK)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON3_POWERCHECK)->EnableWindow(TRUE);

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
		m_StatBar->SetText("北斗：已断开",3,0);
		GetDlgItem(IDC_BUTTON_SYSTEMCHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ICCHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON3_POWERCHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(FALSE);
	}
}

void CBeidouDlg::OnSelendokComboComselect() 
{
	// TODO: Add your control notification handler code here
	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //生成指向应用程序类的指针
	app->m_nCom=m_Com.GetCurSel()+1;
	m_DCom=app->m_nCom;
	UpdateData();	

	CString strTemp;
	strTemp.Format(_T("%d"),m_DCom-1);
	::WritePrivateProfileString("ConfigInfo","com",strTemp,".\\config_phonemessage.ini");
	
	strTemp.Format(_T("%d"),m_DCom);
	::WritePrivateProfileString("ConfigInfo","com_r",strTemp,".\\config_phonemessage.ini");
}

void CBeidouDlg::OnSelendokComboSpeed() 
{
	// TODO: Add your control notification handler code here
	int i=m_Speed.GetCurSel();

	CString strTemp;
	strTemp.Format(_T("%d"),i);
	::WritePrivateProfileString("ConfigInfo","speed",strTemp,".\\config_phonemessage.ini");

	switch(i)
	{
// 	case 0:
// 		i=300;
// 		break;
// 	case 1:
// 		i=600;
// 		break;
// 	case 2:
// 		i=1200;
// 		break;
// 	case 3:
// 		i=2400;
// 		break;
// 	case 4:
// 		i=4800;
// 		break;
// 	case 5:
// 		i=9600;
// 		break;
// 	case 6:
// 		i=19200;
// 		break;
// 	case 7:
// 		i=38400;
// 		break;
// 	case 8:
// 		i=43000;
// 		break;
// 	case 9:
// 		i=56000;
// 		break;
// 	case 10:
// 		i=57600;
// 		break;
// 	case 11:
// 		i=115200;
// 		break;
// 	default:
// 		break;
	case 0:
		i=9600;
		break;
	case 1:
		i=19200;
		break;
	case 2:
		i=43000;
		break;
	case 3:
		i=56000;
		break;
	case 4:
		i=57600;
		break;
	case 5:
		i=115200;
		break;
	default:
		break;
		
	}
	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //生成指向应用程序类的指针
	app->m_nBaud=i;
	m_DBaud=app->m_nBaud;
	UpdateData();	

	strTemp.Format(_T("%d"),m_DBaud);
	::WritePrivateProfileString("ConfigInfo","speed_r",strTemp,".\\config_phonemessage.ini");

}

void CBeidouDlg::OnSelendokComboParity() 
{
	// TODO: Add your control notification handler code here
	char temp;
	int i=m_Parity.GetCurSel();

	CString strTemp;
	strTemp.Format(_T("%d"),i);
	::WritePrivateProfileString("ConfigInfo","parity",strTemp,".\\config_phonemessage.ini");

	switch(i)
	{
	case 0:
		temp='N';
		break;
	case 1:
		temp='O';
		break;
	case 2:
		temp='E';
		break;
	}
	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //生成指向应用程序类的指针
	app->m_cParity=temp;
	m_DParity=app->m_cParity;
	UpdateData();	

	strTemp.Format(_T("%c"),m_DParity);
	::WritePrivateProfileString("ConfigInfo","parity_r",strTemp,".\\config_phonemessage.ini");

}

void CBeidouDlg::OnSelendokComboDatabits() 
{
	// TODO: Add your control notification handler code here
	int i=m_DataBits.GetCurSel();

	CString strTemp;
	strTemp.Format(_T("%d"),i);
	::WritePrivateProfileString("ConfigInfo","databits",strTemp,".\\config_phonemessage.ini");

	switch(i)
	{
	case 0:
		i=8;
		break;
	case 1:
		i=7;
		break;
	case 2:
		i=6;
		break;
	}
	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //生成指向应用程序类的指针
	app->m_nDatabits=i;
	m_DDatabits=app->m_nDatabits;
	UpdateData();
	
	strTemp.Format(_T("%d"),m_DDatabits);
	::WritePrivateProfileString("ConfigInfo","databits_r",strTemp,".\\config_phonemessage.ini");
}

void CBeidouDlg::OnSelendokComboStopbits() 
{
	// TODO: Add your control notification handler code here
	int i=m_StopBits.GetCurSel();

	CString strTemp;
	strTemp.Format(_T("%d"),i);
	::WritePrivateProfileString("ConfigInfo","stopbits",strTemp,".\\config_phonemessage.ini");

	switch(i)
	{
	case 0:
		i=1;
		break;
	case 1:
		i=2;
		break;
	}
	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //生成指向应用程序类的指针
	app->m_nStopbits=i;
	m_DStopbits=app->m_nStopbits;
	UpdateData();
	
	strTemp.Format(_T("%d"),m_DStopbits);
	::WritePrivateProfileString("ConfigInfo","stopbits_r",strTemp,".\\config_phonemessage.ini");
}

void CBeidouDlg::OnButtonSendMsg() 
{
	// TODO: Add your control notification handler code here
//		CString sendTmp="";
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
			SetTimer(2,1000,NULL);
			GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
		}else{
			AfxMessageBox("请输入短信内容。");
		} 
		
}

void CBeidouDlg::OnButtonClear() 
{
	// TODO: Add your control notification handler code here
		m_showmsg="";
		UpdateData(FALSE);

	
}

void CBeidouDlg::OnButtonSystemcheck() 
{
	// TODO: Add your control notification handler code here
	m_sata1.SetPos(0);
	m_sata2.SetPos(0);
 	m_sata3.SetPos(0);
	m_cardnumber=0;
	m_cardstate="";
	m_basestate="";

	CByteArray Array;
	Array.RemoveAll();
	Array.SetSize(12);
	for (int i=0; i<12; i++)
	{
		Array.SetAt(i,frame_SYS_check[i]);
	}
	
	
	if(m_comm.GetPortOpen())
	{
		m_comm.SetOutput(COleVariant(Array));//发送数据
	}
	UpdateData(FALSE);
}

void CBeidouDlg::OnButtonIccheck() 
{
	// TODO: Add your control notification handler code here
	m_cardnumber=0;
	m_category=0;
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
		
		GetDlgItem(IDC_BUTTON_SYSTEMCHECK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON3_POWERCHECK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
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
	m_showmsg+="\r\n\r\n";
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
	if (nIDEvent==1)
	{
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		KillTimer(1);
	} 
	else if(nIDEvent==2)
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
	}else if(nIDEvent==4){//对方未拨通，而挂机，将拨打电话按钮上的文本重置

		KillTimer(4);
	}
	//////////////////////以上用于有线电话/////////////////////////////////
	//-----------------自动应答：5 S----------------------
	else if (nIDEvent==33)  
	{
		KillTimer(33);
		if((flag_PW_in_busy==1)||(flag_PW_out_busy==1))OnButtonCall();//对方输入操作超时，挂机
	}else if (nIDEvent==34)
	{
		KillTimer(34);
		PlaySound(".//Wav//NoResponse.wav", NULL, SND_FILENAME|SND_ASYNC);//对不起，无人应答，请稍微再试
		if((flag_PW_in_busy==1)||(flag_PW_out_busy==1))OnButtonCall();//本机超时未摘机，挂机		
	}
	//-----------------自动应答：5 E----------------------
	///////////////////////以上用于自动应答/////////////////////////////////
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
			GetDlgItem(IDC_BUTTON_CALL)->ShowWindow(SW_SHOW);
		}else if(switch_state==1){//发短信
			SetWindowPos(NULL,0,0,rectMiddle.Width(),rectMiddle.Height(),SWP_NOMOVE|SWP_NOZORDER);
			GetDlgItem(IDC_BUTTON_CALL)->ShowWindow(SW_HIDE);
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
	GetDlgItem(IDC_BUTTON_CALL)->ShowWindow(SW_SHOW);
 	SetDlgItemText(IDC_BUTTON_SET,"配置");
}

void CBeidouDlg::OnButtonMessage() 
{
	// TODO: Add your control notification handler code here
	switch_state=1;//发短信
	SetWindowPos(NULL,0,0,rectMiddle.Width(),rectMiddle.Height(),SWP_NOMOVE|SWP_NOZORDER);
	GetDlgItem(IDC_BUTTON_CALL)->ShowWindow(SW_HIDE);
	SetDlgItemText(IDC_BUTTON_SET,"配置");
}

extern int nCheckCount;//用于计算验证用户或密码(包括开始的1#和2#)的次数，最多三次
extern int nFlag;//0表示其他，1表示正在用户名验证，2表示正在密码验证
 
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
	
	m_StatBar->SetText("有线电话：已连接",0,0);

	if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='N'))//检测是否为DTMF信号
	{

		if(frame_receive_WT[3]!='#'){
			CString tmp;
			tmp.Format("%c",frame_receive_WT[3]);
			send_string+=tmp;
		}else{//把接收到的串传给自动应答函数
			send_string+='#';//加上#号
			m_FKXX+="传号:";
			m_FKXX+=send_string;
			m_FKXX+="\r\n";
			UpdateData(FALSE);
			//---------------1 S----------------------
			int nAuto = 10;//随便定义了一个数
			if (send_string == "1#")
			{
				//广播请求
				SetTimer(33,AUTORESPONSE_TIME,NULL);//20s等待时间（等待用户输入，时间包括了播放语音的时间）
				nAuto = bAutoResponse(PHONEID,send_string);
			}
			else if (send_string == "2#")
			{
				if (nFlag == 1 || nFlag == 2)
				{
					OtherCmdDeal(send_string);
				}
				else
				{
				//通话请求
					PlaySound(".//Wav//RingConnect.wav", NULL, SND_FILENAME|SND_ASYNC);//通话正在连接中，请稍候 
					SetTimer(34,NOPICKUP_TIME,NULL);//自定义振铃等待60s
					//向电台人员通知来电   文件RingPlay.wav

					//Q：这里用户需要怎么操作，
					//这里OnButtonCall ，再添加一个状态标志，做虚拟摘机
					WT_state=4;
					GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("接听电话");
				}
			}
			else//ID和PASSWORD等输入 
			{				
				SetTimer(33,AUTORESPONSE_TIME,NULL);//20s等待时间（等待用户输入，时间包括了播放语音的时间）
				nAuto = bAutoResponse(PHONEID,send_string);
			}

			if(nAuto ==0)
			{
				Sleep(1000);
				OnButtonCall();//挂机
			}
			else if(nAuto ==1)
			{
				//身份验证成功，和其他模块联系，准备广播
			}
			else if(nAuto ==2)
			{
				//继续进行操作
				//此处应不需要做任何事
			}
			//-----------1 E-----------------
			send_string="";
			UpdateData(FALSE);
		}	
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='C')&&(frame_receive_WT[3]=='I')&&(frame_receive_WT[4]=='D'))//检测是否为来电提示
	{
		strDisp=frame_receive_WT+5;
		call_in_number=strDisp;//开始拼接来电号码

	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='Z'))//检测是否摘机成功
	{
		m_FKXX+="摘机成功";
		m_FKXX+="\r\n";
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='D'))//检测是否拨号成功
	{
		m_FKXX+="拨号成功";
		m_FKXX+="\r\n";
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='H'))//检测是否挂机成功
	{
		m_FKXX+="挂机成功";
		m_FKXX+="\r\n";
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='B'))//检测是否传号成功
	{
		m_FKXX+="传号成功";
		m_FKXX+="\r\n";
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='S')&&(frame_receive_WT[3]=='2'))//检测是否为挂机提示
	{
		m_FKXX+="对方挂机";
		m_FKXX+="\r\n";
		OnButtonCall();//对方挂机后，我也挂机，电话状态设置为空闲
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
//		flag_PW_in_busy=0;//对方挂机，清零
//		flag_PW_out_busy=0;//对方挂机，清零
//		WT_state=0;

	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='R')&&(frame_receive_WT[3]=='i')&&(frame_receive_WT[4]=='n')&&(frame_receive_WT[5]=='g'))//检测是否为来电提示音
	{
		m_FKXX+="Ring!  ";
		GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
//		GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("接听电话");
		PlaySound(".//ring.wav", NULL, SND_FILENAME|SND_ASYNC);
		flag_PW_in_busy=1;
		send_string="";//传号字符串清空，准备接收传号
		//----------自动应答：2 S-----------
		OnButtonCall();//摘机，自动播放第一段语音
		PlaySound(".//Wav//Guide.wav", NULL, SND_FILENAME|SND_ASYNC);//应急电台系统，自动广播请按1，原始通话请按2,#号结束
		SetTimer(33,AUTORESPONSE_TIME,NULL);//20s等待时间（等待用户输入，时间包括了播放语音的时间）
		//----------自动应答：2 E-----------
//		SetTimer(4,5000,NULL);//(5S内接收到振铃信号则重置定时器)振铃5秒后未再次接收到振铃，认为是本机未摘机，而对方挂机

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
		m_FKXX+="电话板帧有错误，检察电话线是否连接好\r\n";
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
			m_StatBar->SetText("有线电话：已连接",0,0);
			m_openoff_WT.SetIcon(m_hIconRed);

			GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
		}
		else
			MessageBox("串口无法打开.");	 
	}
	else
	{
		SerialPortOpenCloseFlag_WT=FALSE;
		GetDlgItem(IDC_OPENCLOSEPORT_WT)->SetWindowText("打开串口");
		m_StatBar->SetText("有线电话：已断开",0,0);
		m_openoff_WT.SetIcon(m_hIconOff);
		m_comm_WT.SetPortOpen(FALSE);//关闭串口

		GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(FALSE);
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
	
	if((flag_PW_in_busy==1)||(flag_PW_out_busy==1)){//通信中，按钮始终使能
		GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
	}else{//空闲中
		GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(FALSE);
	}	
	GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(FALSE);
}

void CBeidouDlg::OnButtonCall() 
{
	// TODO: Add your control notification handler code here
		if((flag_PW_out_busy==0)&&(flag_PW_in_busy==0)&&(m_target_number=="0")){//只有拨打电话时(全部空闲)，才检测是否有号码输入
			AfxMessageBox("请先输入对方号码");
			return;
		}

		if(WT_state==0){//摘机
			
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
			if (flag_PW_in_busy==1)//接听电话
			{
//				KillTimer(4);//关闭定时器4。已经接听了，就不需要监测振铃了
				WT_state=3;//该挂机啦
				GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
			} 
			else//向外打电话
			{
				flag_PW_out_busy=1;//主动拨打电话
				GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("正在拨号...");
				GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(FALSE);//拨打电话时，关闭退格按钮
				SetTimer(3,100,NULL);//100ms后再调用本函数进行拨号，使摘机动作对用户透明，先操作一次摘机再调用一次，用于拨号
				WT_state=2;//该拨号啦
			}
			
			
		}else if (WT_state==3)//挂机
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
			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("拨打电话");
			GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(TRUE);//挂机时，打开退格按钮
			WT_state=0;//挂机，空闲中
			flag_PW_out_busy=0;//主动挂机，清零
			flag_PW_in_busy=0;//主动挂机，清零
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
			WT_state=3;//通话中
		}
		//------------------自动应答：4 S---------------------
		else if(WT_state==4)//虚拟摘机
		{
			KillTimer(34);
			//开mic
			//正式开始普通电话通话
	//		ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);	//启动录音
			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("挂机");
			WT_state=3;//通话中
		}
		//-------------------自动应答：4 E--------------------
		//end of WT_state
	 


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
		if(SerialPortOpenCloseFlag_WT)GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
	}
}

void CBeidouDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	if(modulereset==TRUE) 
	{
		module_reset(1);//复位模块 1：有线电话；2：卫星电话；3：3G模块；4：北斗模块；5：广播板；6：其他；
	}
	else if (soundswitch==TRUE)
	{
		sound_switch(1);
	}
	else{
		chuanhao('1');
		m_target_number+="1";
		UpdateData(FALSE);
	}
}

void CBeidouDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
	if(modulereset==TRUE) 
	{
		module_reset(2);//复位模块 1：有线电话；2：卫星电话；3：3G模块；4：北斗模块；5：广播板；6：其他；
	}
	else if (soundswitch==TRUE)
	{
		sound_switch(2);
	}
	else{
		chuanhao('2');
		m_target_number+="2";
		UpdateData(FALSE);
	}
}

void CBeidouDlg::chuanhao(char num)
{
	if (m_target_number=="0")//把零移除
	{
		m_target_number=m_target_number.Left(m_target_number.GetLength()-1);
	}
	if(SerialPortOpenCloseFlag_WT)GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);

	if((flag_PW_in_busy==1)||(flag_PW_out_busy==1)){//呼入或呼出后，才传号
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
		if(flag_PW_out_busy==0)GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(TRUE);//主动呼出时，失能退格按钮
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
	else{
		chuanhao('3');
		m_target_number+="3";
		UpdateData(FALSE);
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
	else{
		chuanhao('4');
		m_target_number+="4";
		UpdateData(FALSE);
	}
}

void CBeidouDlg::OnButton5() 
{
	// TODO: Add your control notification handler code here
	if((modulereset==FALSE)&&(soundswitch==FALSE)){
		chuanhao('5');
		m_target_number+="5";
		UpdateData(FALSE);
	}
}

void CBeidouDlg::OnButton6() 
{
	// TODO: Add your control notification handler code here
	if((modulereset==FALSE)&&(soundswitch==FALSE)){
	chuanhao('6');
	m_target_number+="6";
	UpdateData(FALSE);
	}
}

void CBeidouDlg::OnButton8() //对应数字7键，写反了
{
	// TODO: Add your control notification handler code here
	if((modulereset==FALSE)&&(soundswitch==FALSE)){
	chuanhao('7');
	m_target_number+="7";
	UpdateData(FALSE);
	}
}

void CBeidouDlg::OnButton7() //对应数字8键，写反了
{
	// TODO: Add your control notification handler code here
	if((modulereset==FALSE)&&(soundswitch==FALSE)){
	chuanhao('8');
	m_target_number+="8";
	UpdateData(FALSE);
	}
}

void CBeidouDlg::OnButton9() 
{
	// TODO: Add your control notification handler code here
	if((modulereset==FALSE)&&(soundswitch==FALSE)){
	chuanhao('9');
	m_target_number+="9";
	UpdateData(FALSE);
	}
}

void CBeidouDlg::OnButton10() 
{
	// TODO: Add your control notification handler code here
	if((modulereset==FALSE)&&(soundswitch==FALSE)){
	chuanhao('0');
	m_target_number+="0";
	UpdateData(FALSE);
	}
}

void CBeidouDlg::OnButtonXing() 
{
	// TODO: Add your control notification handler code here
	if((modulereset==FALSE)&&(soundswitch==FALSE)){
	chuanhao('*');
	m_target_number+="*";
	UpdateData(FALSE);
	}
}

void CBeidouDlg::OnButtonJing() 
{
	// TODO: Add your control notification handler code here
	if((modulereset==FALSE)&&(soundswitch==FALSE)){
	chuanhao('#');
	m_target_number+="#";
	UpdateData(FALSE);
	}
}

void CBeidouDlg::OnButtonBack() 
{
	// TODO: Add your control notification handler code here
	if(flag_PW_out_busy==1){//拨出电话时，退格键对传号而言起不到任何作用，此时直接返回
		return;
	}
	
	m_target_number=m_target_number.Left(m_target_number.GetLength()-1);
	if (m_target_number.GetLength()==0)
	{
		m_target_number+="0";
		if((flag_PW_in_busy==0)&&(flag_PW_out_busy==0)){
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

//DEL void CBeidouDlg::OnCaptureChanged(CWnd *pWnd) 
//DEL {
//DEL 	// TODO: Add your message handler code here
//DEL // 	HANDLE hself = GetCurrentProcess();
//DEL // 	TerminateProcess(hself, 0);
//DEL 
//DEL 	CDialog::OnCaptureChanged(pWnd);
//DEL }

//DEL void CBeidouDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
//DEL {
//DEL 	// TODO: Add your message handler code here and/or call default
//DEL 	HANDLE hself = GetCurrentProcess();
//DEL // 	TerminateProcess(hself, 0);
//DEL 	CDialog::OnChar(nChar, nRepCnt, nFlags);
//DEL }

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
		variant_inp=m_comm_YW.GetInput(); //读缓冲区
		safearray_inp=variant_inp;  //VARIANT型变量转换为ColeSafeArray型变量
		len=safearray_inp.GetOneDimSize(); //得到有效数据长度
		for(k=0;k<len;k++)
		{
			safearray_inp.GetElement(&k,rxdata+k);//转换为BYTE型数组
		}
		
		//			AfxMessageBox("OK",MB_OK,0);
		//			frame=frame_len[frame_index_YW];
		//			frame_lock=0;
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
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_wakeup_times)&&(frame_receive[7]==XOR(frame_receive,7)))//首次连接握手，上位机软件接收时，不用避免$,\r,\n
	{
		flag_com_init_ack_YW=1;
		m_board_led_YW.SetIcon(m_hIconRed);
		GetDlgItem(IDC_STATIC_BOARDCONNECT2)->SetWindowText(" 运维板已连接！"); 
		timer_board_disconnect_times_YW=0;//收到反馈则清零
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
			m_StatBar->SetText("运维板状态：其他模块复位完毕",4,0); 
			break;
		}

	}else if ((flag_com_init_ack_YW==1)&&(frame_receive[1]=='s')&&(frame_receive[2]=='w')&&(frame_receive[3]=='h')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_control_times)&&(frame_receive[8]==XOR(frame_receive,8)))//切换音频开关申请帧
	{
		if ((frame_receive[7]-0x30)==1)
		{
			m_StatBar->SetText("运维板状态：开关切换为1",4,0); 
		} 
		else if ((frame_receive[7]-0x30)==2)
		{
			m_StatBar->SetText("运维板状态：开关切换为2",4,0); 
		}
		else if ((frame_receive[7]-0x30)==3)
		{
			m_StatBar->SetText("运维板状态：开关切换为3",4,0); 
		}
		else if ((frame_receive[7]-0x30)==4)
		{
			m_StatBar->SetText("运维板状态：开关切换为4",4,0); 
		}
		


	}else if ((flag_com_init_ack_YW==1)&&(frame_receive[1]=='s')&&(frame_receive[2]=='c')&&(frame_receive[3]=='a')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_control_times)&&(frame_receive[7]==XOR(frame_receive,7)))//频谱扫描帧反馈信息
	{
		m_StatBar->SetText("运维板状态：配置为频谱扫描模式",4,0);
		
		
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
		AfxMessageBox("运维板回传帧有错误！",MB_OK,0);
	}
	UpdateData(FALSE);
	}
}

void CBeidouDlg::OnButtonConnect_YW() 
{
	// TODO: Add your control notification handler code here
	char buff[2];
	CString string1="",string2="";
	buff[1]='\0';
	buff[0]=m_DParity;
	string1.Format(_T("%d"),m_DBaud);
	string1+=",";
	string2=buff;
	string1+=string2;
	string1+=",";
	string2.Format(_T("%d"),m_DDatabits); 
	string1+=string2;
	string1+=",";
	string2.Format(_T("%d"),m_DStopbits);
	string1+=string2;
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
			MessageBox("can not open serial port");
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
			SetTimer(6,(TIMER2_MS+3000),NULL);//没有处在频谱扫描阶段才打开定期查询，10+3秒(与广播板的查询在时间上错开)查询一次子板是否保持连接。恢复硬件连接时，可以自动连接
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
		GetDlgItem(IDC_STATIC_BOARDCONNECT2)->SetWindowText(" 运维板未连接！");
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
		frame_board_reset_YW[6]=index+0x30;//复位模块 1：有线电话；2：卫星电话；3：3G模块；4：北斗模块；5：广播板；6：其他；
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
	} 
	else
	{
		modulereset=FALSE;
		GetDlgItem(IDC_MODULE_RESET)->SetWindowText("手动复位");

		GetDlgItem(IDC_BUTTON_1)->SetWindowText("1");
		GetDlgItem(IDC_BUTTON_2)->SetWindowText("2");
		GetDlgItem(IDC_BUTTON_3)->SetWindowText("3");
		GetDlgItem(IDC_BUTTON_4)->SetWindowText("4");
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
		frame_board_sound_YW[6]=index+0x30;//复位模块 1：有线电话；2：卫星电话；3：3G模块；4：北斗模块；5：广播板；6：其他；
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
	} 
	else
	{
		soundswitch=FALSE;
		GetDlgItem(IDC_SOUND_SWITCH)->SetWindowText("音频切换");
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
