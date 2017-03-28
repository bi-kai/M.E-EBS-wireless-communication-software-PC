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
#define received_frame_size 5//�������������
bool frameplus=0;//0:$Ϊ��һλ��1��$�ڴ����м�

#define BORD_LEFT_HIDE 500//�߼����ã����صĽ���������
#define BORD_RIGHT_HIDE 400//�߼����ã����صĽ���������

unsigned char frame_IC_check[12]={0x24,0x49,0x43,0x4A,0x43,0x00,0x0C,0x00,0x00,0x00,0x01,0x2A};//IC��ѯ
unsigned char frame_SYS_check[11]={0x24,0x53,0x74,0x73,0x5F,0x00,0x0B};//ϵͳ���
unsigned char frame_POWER_check[12]={0x24,0x47,0x4C,0x4A,0x43,0x00,0x0C,0x00,0x00,0x00,0x01,0x2B};//���ʲ�ѯ
unsigned char frame_MSG_check[250]={0x24,0x54,0x58,0x53,0x51};//ͨ������

unsigned char frame_receive[received_frame_size][2000]={0};

/*************���ߵ绰******************************/
unsigned char frame_receive_WT[2000]={0};

/***********��ά��***************/
unsigned char frame_board_check_YW[7+2]={'$','r','e','y','_'};//��ά���Ӽ��֡
unsigned char frame_board_reset_YW[8+2]={'$','r','s','e','_'};//��ά��λ֡
unsigned char frame_board_sound_YW[8+2]={'$','s','w','h','_'};//��ά�л���Ƶ��������֡
unsigned char frame_board_connect_YW[7+2]={'$','p','p','p','_'};//��ά��λ����������໥��ѯ֡
unsigned char frame_board_scan_YW[7+2]={'$','s','c','a','_'};//��άƵ��ɨ�裬�̵�������֡

#define TIMER2_MS 10000//��ʱ��2�жϼ����10s��ѯһ���Ӱ������״��
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

	SerialPortOpenCloseFlag=FALSE;//Ĭ�Ϲرմ���
	SerialPortOpenCloseFlag_WT=FALSE;//Ĭ�Ϲر����ߵ绰�Ĵ���
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
	GetDlgItem(IDC_COMBO_STOPBITS2)->SetWindowText(_T("����"));

	GetDlgItem(IDC_BUTTON_SYSTEMCHECK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ICCHECK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3_POWERCHECK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(FALSE);

	GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(FALSE);
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
/********************2�����ߵ绰��������***********************************/	
	m_comm_WT.SetCommPort(2); //ѡ��com2
	m_comm_WT.SetInputMode(1); //���뷽ʽΪ�����Ʒ�ʽ
	m_comm_WT.SetInBufferSize(1024); //�������뻺������С
	m_comm_WT.SetOutBufferSize(10240); //���������������С
	m_comm_WT.SetSettings("1200,n,8,1"); //������1200����У�飬8������λ��1��ֹͣλ	 
	m_comm_WT.SetRThreshold(1); //����1��ʾÿ�����ڽ��ջ��������ж��ڻ����1���ַ�ʱ������һ���������ݵ�OnComm�¼�
	m_comm_WT.SetInputLen(0); //���õ�ǰ���������ݳ���Ϊ0
	//	 m_comm_WT.GetInput();    //��Ԥ���������������������
/********************3����ά��������***********************************/	
	m_comm_YW.SetCommPort(3); //ѡ��com3
	m_comm_YW.SetInputMode(1); //���뷽ʽΪ�����Ʒ�ʽ
	m_comm_YW.SetInBufferSize(1024); //�������뻺������С
	m_comm_YW.SetOutBufferSize(10240); //���������������С
	m_comm_YW.SetSettings("115200,n,8,1"); //������1200����У�飬8������λ��1��ֹͣλ	 
	m_comm_YW.SetRThreshold(7); //����1��ʾÿ�����ڽ��ջ��������ж��ڻ����1���ַ�ʱ������һ���������ݵ�OnComm�¼�
	m_comm_YW.SetInputLen(0); //���õ�ǰ���������ݳ���Ϊ0
	//	 m_comm_WT.GetInput();    //��Ԥ���������������������
/**********************************************************************/
	frame_lock=0;//����֡ʹ������0�������ã�1����ֹʹ��
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
	GetDlgItem(IDC_BUTTON_CALL)->ShowWindow(SW_SHOW);

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
		::WritePrivateProfileString("ConfigInfo","com","0",".\\config_phonemessage.ini");//��������ѡ����
		::WritePrivateProfileString("ConfigInfo","com_WT","1",".\\config_phonemessage.ini");//��������ѡ����
		::WritePrivateProfileString("ConfigInfo","com_YW","2",".\\config_phonemessage.ini");//��������ѡ����
		::WritePrivateProfileString("ConfigInfo","parity","0",".\\config_phonemessage.ini");
		::WritePrivateProfileString("ConfigInfo","databits","0",".\\config_phonemessage.ini");
		::WritePrivateProfileString("ConfigInfo","speed","5",".\\config_phonemessage.ini");
		::WritePrivateProfileString("ConfigInfo","stopbits","0",".\\config_phonemessage.ini");
		
		::WritePrivateProfileString("ConfigInfo","com_r","1",".\\config_phonemessage.ini");//����������ֵ��
		::WritePrivateProfileString("ConfigInfo","com_r_WT","2",".\\config_phonemessage.ini");//��������ѡ����
		::WritePrivateProfileString("ConfigInfo","com_r_YW","3",".\\config_phonemessage.ini");//��������ѡ����
		::WritePrivateProfileString("ConfigInfo","parity_r","N",".\\config_phonemessage.ini");
		::WritePrivateProfileString("ConfigInfo","databits_r","8",".\\config_phonemessage.ini");
		::WritePrivateProfileString("ConfigInfo","speed_r","115200",".\\config_phonemessage.ini");
		::WritePrivateProfileString("ConfigInfo","stopbits_r","1",".\\config_phonemessage.ini");		
	}

	/**********��������**********************/
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
	((CComboBox*)GetDlgItem(IDC_COMBO_COMSELECT))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//���õ�n������Ϊ��ʾ�����ݡ�
	
	GetPrivateProfileString("ConfigInfo","com_WT","1",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_COMSELECT_WT))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//���õ�n������Ϊ��ʾ�����ݡ�

	GetPrivateProfileString("ConfigInfo","com_YW","2",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_COMSELECT_YW))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//���õ�n������Ϊ��ʾ�����ݡ�

	GetPrivateProfileString("ConfigInfo","parity","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_PARITY))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//���õ�n������Ϊ��ʾ�����ݡ�
	
	GetPrivateProfileString("ConfigInfo","databits","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_DATABITS))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//���õ�n������Ϊ��ʾ�����ݡ�
	
	GetPrivateProfileString("ConfigInfo","speed","5",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_SPEED))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//���õ�n������Ϊ��ʾ�����ݡ�
	
	GetPrivateProfileString("ConfigInfo","stopbits","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_phonemessage.ini");
	strBufferReadConfig.ReleaseBuffer();
	strtmpReadConfig+=","+strBufferReadConfig;
	((CComboBox*)GetDlgItem(IDC_COMBO_STOPBITS))->SetCurSel((int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig));//���õ�n������Ϊ��ʾ�����ݡ�
	
	UpdateData(FALSE);
	/*********************************************************************/
	switch_state=0;//��绰
	WT_state=0;//�绰��״̬����ʼ����Ϊ����
	flag_PW_in_busy=0;//��ʼֵ
	flag_PW_out_busy=0;//��ʼֵ
	
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
	BYTE rxdata[2048]; //����BYTE����
	CString strDisp="",strTmp="";

	if((m_comm.GetCommEvent()==2)) //�¼�ֵΪ2��ʾ���ջ����������ַ�
	{	
//		Sleep(100);//һ֡�ϳ���Ϊ������հ�֡�ʹ�����Ӧ���������δ������ж϶���Ӵ����������롣
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
//			frame_lock=0;
//			frame_len[frame_index]=0;


		for(k=0;k<len;k++)//������ת��ΪCString����
		{
			BYTE bt=*(char*)(rxdata+k);    //�ַ���
			if (bt=='$')
			{
				if (rxdata[0]=='$')
				{
					frameplus=0;
				} 
				else//$�����������м�λ��
				{
					frameplus=1;
					frame_index++;//��һ���ѽ��գ���Ҫ����
				}

				if (frame_index!=0)
				{
	//				frame_index++;//�µ�һ֡����
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
								frame_flag[i]=0;//���Ϊδʹ��
							}
						}
						
					}
					if(frame_index==received_frame_size)	frame_index=0;
				}
				
			}
			frame_receive[frame_index][framelen]=bt;
			framelen++;
			strTmp.Format("%02x ",bt);    //���ַ�������ʱ����strtemp���
			strDisp+=strTmp;  //������ձ༭���Ӧ�ַ���
			
		}
		if (framelen==(frame_receive[frame_index][5]*256+frame_receive[frame_index][6]))
		{
			//֡�������
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
					frame_flag[i]=0;//���Ϊδʹ��
				}
			}
//			frame_index++;//�յ�������һ֡
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
			m_StatBar->SetText("������������",3,0);
			GetDlgItem(IDC_BUTTON_ICCHECK)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SYSTEMCHECK)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON3_POWERCHECK)->EnableWindow(TRUE);

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
		m_StatBar->SetText("�������ѶϿ�",3,0);
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
	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //����ָ��Ӧ�ó������ָ��
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
	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //����ָ��Ӧ�ó������ָ��
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
	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //����ָ��Ӧ�ó������ָ��
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
	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //����ָ��Ӧ�ó������ָ��
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
	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //����ָ��Ӧ�ó������ָ��
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
			SetTimer(2,1000,NULL);
			GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
		}else{
			AfxMessageBox("������������ݡ�");
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
		m_comm.SetOutput(COleVariant(Array));//��������
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
		
		GetDlgItem(IDC_BUTTON_SYSTEMCHECK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON3_POWERCHECK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
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
	if (nIDEvent==1)
	{
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		KillTimer(1);
	} 
	else if(nIDEvent==2)
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
	}else if(nIDEvent==4){//�Է�δ��ͨ�����һ���������绰��ť�ϵ��ı�����

		KillTimer(4);
	}
	//////////////////////�����������ߵ绰/////////////////////////////////
	//-----------------�Զ�Ӧ��5 S----------------------
	else if (nIDEvent==33)  
	{
		KillTimer(33);
		if((flag_PW_in_busy==1)||(flag_PW_out_busy==1))OnButtonCall();//�Է����������ʱ���һ�
	}else if (nIDEvent==34)
	{
		KillTimer(34);
		PlaySound(".//Wav//NoResponse.wav", NULL, SND_FILENAME|SND_ASYNC);//�Բ�������Ӧ������΢����
		if((flag_PW_in_busy==1)||(flag_PW_out_busy==1))OnButtonCall();//������ʱδժ�����һ�		
	}
	//-----------------�Զ�Ӧ��5 E----------------------
	///////////////////////���������Զ�Ӧ��/////////////////////////////////
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
			GetDlgItem(IDC_BUTTON_CALL)->ShowWindow(SW_SHOW);
		}else if(switch_state==1){//������
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
	switch_state=0;//��绰
 	SetWindowPos(NULL,0,0,rectSmall.Width(),rectSmall.Height(),SWP_NOMOVE|SWP_NOZORDER);
	GetDlgItem(IDC_BUTTON_CALL)->ShowWindow(SW_SHOW);
 	SetDlgItemText(IDC_BUTTON_SET,"����");
}

void CBeidouDlg::OnButtonMessage() 
{
	// TODO: Add your control notification handler code here
	switch_state=1;//������
	SetWindowPos(NULL,0,0,rectMiddle.Width(),rectMiddle.Height(),SWP_NOMOVE|SWP_NOZORDER);
	GetDlgItem(IDC_BUTTON_CALL)->ShowWindow(SW_HIDE);
	SetDlgItemText(IDC_BUTTON_SET,"����");
}

extern int nCheckCount;//���ڼ�����֤�û�������(������ʼ��1#��2#)�Ĵ������������
extern int nFlag;//0��ʾ������1��ʾ�����û�����֤��2��ʾ����������֤
 
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
	
	m_StatBar->SetText("���ߵ绰��������",0,0);

	if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='N'))//����Ƿ�ΪDTMF�ź�
	{

		if(frame_receive_WT[3]!='#'){
			CString tmp;
			tmp.Format("%c",frame_receive_WT[3]);
			send_string+=tmp;
		}else{//�ѽ��յ��Ĵ������Զ�Ӧ����
			send_string+='#';//����#��
			m_FKXX+="����:";
			m_FKXX+=send_string;
			m_FKXX+="\r\n";
			UpdateData(FALSE);
			//---------------1 S----------------------
			int nAuto = 10;//��㶨����һ����
			if (send_string == "1#")
			{
				//�㲥����
				SetTimer(33,AUTORESPONSE_TIME,NULL);//20s�ȴ�ʱ�䣨�ȴ��û����룬ʱ������˲���������ʱ�䣩
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
				//ͨ������
					PlaySound(".//Wav//RingConnect.wav", NULL, SND_FILENAME|SND_ASYNC);//ͨ�����������У����Ժ� 
					SetTimer(34,NOPICKUP_TIME,NULL);//�Զ�������ȴ�60s
					//���̨��Ա֪ͨ����   �ļ�RingPlay.wav

					//Q�������û���Ҫ��ô������
					//����OnButtonCall �������һ��״̬��־��������ժ��
					WT_state=4;
					GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�����绰");
				}
			}
			else//ID��PASSWORD������ 
			{				
				SetTimer(33,AUTORESPONSE_TIME,NULL);//20s�ȴ�ʱ�䣨�ȴ��û����룬ʱ������˲���������ʱ�䣩
				nAuto = bAutoResponse(PHONEID,send_string);
			}

			if(nAuto ==0)
			{
				Sleep(1000);
				OnButtonCall();//�һ�
			}
			else if(nAuto ==1)
			{
				//�����֤�ɹ���������ģ����ϵ��׼���㲥
			}
			else if(nAuto ==2)
			{
				//�������в���
				//�˴�Ӧ����Ҫ���κ���
			}
			//-----------1 E-----------------
			send_string="";
			UpdateData(FALSE);
		}	
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='C')&&(frame_receive_WT[3]=='I')&&(frame_receive_WT[4]=='D'))//����Ƿ�Ϊ������ʾ
	{
		strDisp=frame_receive_WT+5;
		call_in_number=strDisp;//��ʼƴ���������

	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='Z'))//����Ƿ�ժ���ɹ�
	{
		m_FKXX+="ժ���ɹ�";
		m_FKXX+="\r\n";
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='D'))//����Ƿ񲦺ųɹ�
	{
		m_FKXX+="���ųɹ�";
		m_FKXX+="\r\n";
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='H'))//����Ƿ�һ��ɹ�
	{
		m_FKXX+="�һ��ɹ�";
		m_FKXX+="\r\n";
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='B'))//����Ƿ񴫺ųɹ�
	{
		m_FKXX+="���ųɹ�";
		m_FKXX+="\r\n";
	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='S')&&(frame_receive_WT[3]=='2'))//����Ƿ�Ϊ�һ���ʾ
	{
		m_FKXX+="�Է��һ�";
		m_FKXX+="\r\n";
		OnButtonCall();//�Է��һ�����Ҳ�һ����绰״̬����Ϊ����
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
//		flag_PW_in_busy=0;//�Է��һ�������
//		flag_PW_out_busy=0;//�Է��һ�������
//		WT_state=0;

	}else if ((frame_receive_WT[0]=='A')&&(frame_receive_WT[1]=='T')&&(frame_receive_WT[2]=='R')&&(frame_receive_WT[3]=='i')&&(frame_receive_WT[4]=='n')&&(frame_receive_WT[5]=='g'))//����Ƿ�Ϊ������ʾ��
	{
		m_FKXX+="Ring!  ";
		GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
//		GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�����绰");
		PlaySound(".//ring.wav", NULL, SND_FILENAME|SND_ASYNC);
		flag_PW_in_busy=1;
		send_string="";//�����ַ�����գ�׼�����մ���
		//----------�Զ�Ӧ��2 S-----------
		OnButtonCall();//ժ�����Զ����ŵ�һ������
		PlaySound(".//Wav//Guide.wav", NULL, SND_FILENAME|SND_ASYNC);//Ӧ����̨ϵͳ���Զ��㲥�밴1��ԭʼͨ���밴2,#�Ž���
		SetTimer(33,AUTORESPONSE_TIME,NULL);//20s�ȴ�ʱ�䣨�ȴ��û����룬ʱ������˲���������ʱ�䣩
		//----------�Զ�Ӧ��2 E-----------
//		SetTimer(4,5000,NULL);//(5S�ڽ��յ������ź������ö�ʱ��)����5���δ�ٴν��յ����壬��Ϊ�Ǳ���δժ�������Է��һ�

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
		m_FKXX+="�绰��֡�д��󣬼��绰���Ƿ����Ӻ�\r\n";
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
			m_StatBar->SetText("���ߵ绰��������",0,0);
			m_openoff_WT.SetIcon(m_hIconRed);

			GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
		}
		else
			MessageBox("�����޷���.");	 
	}
	else
	{
		SerialPortOpenCloseFlag_WT=FALSE;
		GetDlgItem(IDC_OPENCLOSEPORT_WT)->SetWindowText("�򿪴���");
		m_StatBar->SetText("���ߵ绰���ѶϿ�",0,0);
		m_openoff_WT.SetIcon(m_hIconOff);
		m_comm_WT.SetPortOpen(FALSE);//�رմ���

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
	
	if((flag_PW_in_busy==1)||(flag_PW_out_busy==1)){//ͨ���У���ťʼ��ʹ��
		GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);
	}else{//������
		GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(FALSE);
	}	
	GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(FALSE);
}

void CBeidouDlg::OnButtonCall() 
{
	// TODO: Add your control notification handler code here
		if((flag_PW_out_busy==0)&&(flag_PW_in_busy==0)&&(m_target_number=="0")){//ֻ�в���绰ʱ(ȫ������)���ż���Ƿ��к�������
			AfxMessageBox("��������Է�����");
			return;
		}

		if(WT_state==0){//ժ��
			
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
			if (flag_PW_in_busy==1)//�����绰
			{
//				KillTimer(4);//�رն�ʱ��4���Ѿ������ˣ��Ͳ���Ҫ���������
				WT_state=3;//�ùһ���
				GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
			} 
			else//�����绰
			{
				flag_PW_out_busy=1;//��������绰
				GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("���ڲ���...");
				GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(FALSE);//����绰ʱ���ر��˸�ť
				SetTimer(3,100,NULL);//100ms���ٵ��ñ��������в��ţ�ʹժ���������û�͸�����Ȳ���һ��ժ���ٵ���һ�Σ����ڲ���
				WT_state=2;//�ò�����
			}
			
			
		}else if (WT_state==3)//�һ�
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
			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("����绰");
			GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(TRUE);//�һ�ʱ�����˸�ť
			WT_state=0;//�һ���������
			flag_PW_out_busy=0;//�����һ�������
			flag_PW_in_busy=0;//�����һ�������
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
			WT_state=3;//ͨ����
		}
		//------------------�Զ�Ӧ��4 S---------------------
		else if(WT_state==4)//����ժ��
		{
			KillTimer(34);
			//��mic
			//��ʽ��ʼ��ͨ�绰ͨ��
	//		ShellExecute(NULL,"open","record.exe",NULL,NULL,SW_SHOWNORMAL);	//����¼��
			GetDlgItem(IDC_BUTTON_CALL)->SetWindowText("�һ�");
			WT_state=3;//ͨ����
		}
		//-------------------�Զ�Ӧ��4 E--------------------
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
		module_reset(1);//��λģ�� 1�����ߵ绰��2�����ǵ绰��3��3Gģ�飻4������ģ�飻5���㲥�壻6��������
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
		module_reset(2);//��λģ�� 1�����ߵ绰��2�����ǵ绰��3��3Gģ�飻4������ģ�飻5���㲥�壻6��������
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
	if (m_target_number=="0")//�����Ƴ�
	{
		m_target_number=m_target_number.Left(m_target_number.GetLength()-1);
	}
	if(SerialPortOpenCloseFlag_WT)GetDlgItem(IDC_BUTTON_CALL)->EnableWindow(TRUE);

	if((flag_PW_in_busy==1)||(flag_PW_out_busy==1)){//���������󣬲Ŵ���
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
		if(flag_PW_out_busy==0)GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(TRUE);//��������ʱ��ʧ���˸�ť
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
		module_reset(4);//��λģ�� 1�����ߵ绰��2�����ǵ绰��3��3Gģ�飻4������ģ�飻5���㲥�壻6��������
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

void CBeidouDlg::OnButton8() //��Ӧ����7����д����
{
	// TODO: Add your control notification handler code here
	if((modulereset==FALSE)&&(soundswitch==FALSE)){
	chuanhao('7');
	m_target_number+="7";
	UpdateData(FALSE);
	}
}

void CBeidouDlg::OnButton7() //��Ӧ����8����д����
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
	if(flag_PW_out_busy==1){//�����绰ʱ���˸���Դ��Ŷ����𲻵��κ����ã���ʱֱ�ӷ���
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
		variant_inp=m_comm_YW.GetInput(); //��������
		safearray_inp=variant_inp;  //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���
		len=safearray_inp.GetOneDimSize(); //�õ���Ч���ݳ���
		for(k=0;k<len;k++)
		{
			safearray_inp.GetElement(&k,rxdata+k);//ת��ΪBYTE������
		}
		
		//			AfxMessageBox("OK",MB_OK,0);
		//			frame=frame_len[frame_index_YW];
		//			frame_lock=0;
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
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_wakeup_times)&&(frame_receive[7]==XOR(frame_receive,7)))//�״��������֣���λ���������ʱ�����ñ���$,\r,\n
	{
		flag_com_init_ack_YW=1;
		m_board_led_YW.SetIcon(m_hIconRed);
		GetDlgItem(IDC_STATIC_BOARDCONNECT2)->SetWindowText(" ��ά�������ӣ�"); 
		timer_board_disconnect_times_YW=0;//�յ�����������
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
			m_StatBar->SetText("��ά��״̬������ģ�鸴λ���",4,0); 
			break;
		}

	}else if ((flag_com_init_ack_YW==1)&&(frame_receive[1]=='s')&&(frame_receive[2]=='w')&&(frame_receive[3]=='h')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_control_times)&&(frame_receive[8]==XOR(frame_receive,8)))//�л���Ƶ��������֡
	{
		if ((frame_receive[7]-0x30)==1)
		{
			m_StatBar->SetText("��ά��״̬�������л�Ϊ1",4,0); 
		} 
		else if ((frame_receive[7]-0x30)==2)
		{
			m_StatBar->SetText("��ά��״̬�������л�Ϊ2",4,0); 
		}
		else if ((frame_receive[7]-0x30)==3)
		{
			m_StatBar->SetText("��ά��״̬�������л�Ϊ3",4,0); 
		}
		else if ((frame_receive[7]-0x30)==4)
		{
			m_StatBar->SetText("��ά��״̬�������л�Ϊ4",4,0); 
		}
		


	}else if ((flag_com_init_ack_YW==1)&&(frame_receive[1]=='s')&&(frame_receive[2]=='c')&&(frame_receive[3]=='a')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_control_times)&&(frame_receive[7]==XOR(frame_receive,7)))//Ƶ��ɨ��֡������Ϣ
	{
		m_StatBar->SetText("��ά��״̬������ΪƵ��ɨ��ģʽ",4,0);
		
		
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
		AfxMessageBox("��ά��ش�֡�д���",MB_OK,0);
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

		//�����Ǵ��ڵĳ�ʼ������
		if(m_comm_YW.GetPortOpen())//�򿪶˿�ǰ�ļ�⣬�ȹأ��ٿ�
			MessageBox("can not open serial port");
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
			SetTimer(6,(TIMER2_MS+3000),NULL);//û�д���Ƶ��ɨ��׶βŴ򿪶��ڲ�ѯ��10+3��(��㲥��Ĳ�ѯ��ʱ���ϴ�)��ѯһ���Ӱ��Ƿ񱣳����ӡ��ָ�Ӳ������ʱ�������Զ�����
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
		GetDlgItem(IDC_STATIC_BOARDCONNECT2)->SetWindowText(" ��ά��δ���ӣ�");
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
		frame_board_reset_YW[6]=index+0x30;//��λģ�� 1�����ߵ绰��2�����ǵ绰��3��3Gģ�飻4������ģ�飻5���㲥�壻6��������
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
	} 
	else
	{
		modulereset=FALSE;
		GetDlgItem(IDC_MODULE_RESET)->SetWindowText("�ֶ���λ");

		GetDlgItem(IDC_BUTTON_1)->SetWindowText("1");
		GetDlgItem(IDC_BUTTON_2)->SetWindowText("2");
		GetDlgItem(IDC_BUTTON_3)->SetWindowText("3");
		GetDlgItem(IDC_BUTTON_4)->SetWindowText("4");
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
		frame_board_sound_YW[6]=index+0x30;//��λģ�� 1�����ߵ绰��2�����ǵ绰��3��3Gģ�飻4������ģ�飻5���㲥�壻6��������
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
	} 
	else
	{
		soundswitch=FALSE;
		GetDlgItem(IDC_SOUND_SWITCH)->SetWindowText("��Ƶ�л�");
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
