// beidouDlg.cpp : implementation file
//

#include "stdafx.h"
#include "beidou.h"
#include "beidouDlg.h"
#include <math.h> 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define Sig 0
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

unsigned char frame_IC_check[11]={0x24,0x49,0x63,0x63,0x5F,0x00,0x0B,0x00,0x00,0x00,0x39};
unsigned char frame_SYS_check[11]={0x24,0x53,0x74,0x73,0x5F,0x00,0x0B};
unsigned char frame_POWER_check[12]={0x24,0x53,0x69,0x67,0x5F,0x00,0x0C};
unsigned char frame_MSG_check[100]={0x24,0x4D,0x73,0x67,0x5F};

unsigned char frame_receive[received_frame_size][2000]={0};
//unsigned char frame_IC_check[11]={0x24,0x49,0x63,0x63,0x5F,0x00,0x0B,0x00,0x00,0x00,0x39};


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
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	SerialPortOpenCloseFlag=FALSE;//Ĭ�Ϲرմ���
}

void CBeidouDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBeidouDlg)
//	DDX_Control(pDX, IDC_TAB1, m_tab);
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
	DDV_MaxChars(pDX, m_sendmsg, 600);
	DDX_Text(pDX, IDC_EDIT_CARDNUMBER, m_cardnumber);
	DDX_Text(pDX, IDC_EDIT_CATEGORY, m_category);
	DDX_Text(pDX, IDC_EDIT_CARDSTATES, m_cardstate);
	DDX_Text(pDX, IDC_EDIT_BASESTATES, m_basestate);
	DDX_Text(pDX, IDC_EDIT_FKXX, m_FKXX);
	DDX_Text(pDX, IDC_EDIT5, m_otherID);
	DDV_MinMaxInt(pDX, m_otherID, 0, 16777215);
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

	m_hIconRed  = AfxGetApp()->LoadIcon(IDI_ICON_RED);
	m_hIconOff	= AfxGetApp()->LoadIcon(IDI_ICON_OFF);
	GetDlgItem(IDC_COMBO_COMSELECT)->SetWindowText(_T("COM1"));
	GetDlgItem(IDC_COMBO_SPEED)->SetWindowText(_T("115200"));
	GetDlgItem(IDC_COMBO_PARITY)->SetWindowText(_T("NONE"));
	GetDlgItem(IDC_COMBO_DATABITS)->SetWindowText(_T("8"));
	GetDlgItem(IDC_COMBO_STOPBITS)->SetWindowText(_T("1"));
	GetDlgItem(IDC_COMBO_STOPBITS2)->SetWindowText(_T("����"));

	GetDlgItem(IDC_BUTTON_SYSTEMCHECK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ICCHECK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3_POWERCHECK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(FALSE);
	UpdateData(FALSE);
	
	m_comm.SetCommPort(1); //ѡ��com1
	m_comm.SetInputMode(1); //���뷽ʽΪ�����Ʒ�ʽ
	m_comm.SetInBufferSize(10240); //�������뻺������С
	m_comm.SetOutBufferSize(1024); //���������������С
	m_comm.SetSettings("115200,n,8,1"); //������115200����У�飬8������λ��1��ֹͣλ	 
	m_comm.SetRThreshold(1); //����1��ʾÿ�����ڽ��ջ��������ж��ڻ����1���ַ�ʱ������һ���������ݵ�OnComm�¼�
	m_comm.SetInputLen(0); //���õ�ǰ���������ݳ���Ϊ0
	//	 m_comm.GetInput();    //��Ԥ���������������������
	if(!m_comm.GetPortOpen())
	{		 
		//		m_comm.SetPortOpen(TRUE);//�򿪴���(�˴����ش򿪣�����á��򿪴��ڡ���ťʵ��)
	}
	else
		 MessageBox("can not open serial port");

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

	m_timer.SetRange(0,60);
    m_timer.SetPos(0);
	m_timer.SetStep(1);//���ý������ĵ�ǰλ��
	
	
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
//			strTmp.Format("%02x ",bt);    //���ַ�������ʱ����strtemp���
//			strDisp+=strTmp;  //������ձ༭���Ӧ�ַ���
			
		}
		if (framelen==(frame_receive[frame_index][5]*256+frame_receive[frame_index][6]))
		{
			//֡�������
//  			CString buf;
//  			buf.Format("%d",frame_index);
//  			AfxMessageBox(buf,MB_OK,0);

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
		m_showmsg+=strDisp;
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
			MessageBox("can not open serial port");
//			m_comm.SetPortOpen(FALSE);	//	
		m_comm.SetCommPort(m_DCom); //ѡ��˿ڣ�Ĭ����com1
		m_comm.SetSettings((LPSTR)(LPCTSTR)string1); //������9600����У�飬8������λ��1��ֹͣλ
		if(!m_comm.GetPortOpen())
		{			
			m_comm.SetPortOpen(TRUE);//�򿪴���
			GetDlgItem(IDC_OPENCLOSEPORT)->SetWindowText("�رմ���");
			m_ctrlIconOpenoff.SetIcon(m_hIconRed);

			GetDlgItem(IDC_BUTTON_ICCHECK)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SYSTEMCHECK)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON3_POWERCHECK)->EnableWindow(TRUE);

			//�򿪴��ں����IC��ѯ��ȡ������
			CByteArray Array;
			Array.RemoveAll();
            Array.SetSize(11);
			for (int i=0; i<11; i++)
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
}

void CBeidouDlg::OnSelendokComboSpeed() 
{
	// TODO: Add your control notification handler code here
	int i=m_Speed.GetCurSel();
	switch(i)
	{
	case 0:
		i=300;
		break;
	case 1:
		i=600;
		break;
	case 2:
		i=1200;
		break;
	case 3:
		i=2400;
		break;
	case 4:
		i=4800;
		break;
	case 5:
		i=9600;
		break;
	case 6:
		i=19200;
		break;
	case 7:
		i=38400;
		break;
	case 8:
		i=43000;
		break;
	case 9:
		i=56000;
		break;
	case 10:
		i=57600;
		break;
	case 11:
		i=115200;
		break;
	default:
		break;
		
	}
	CBeidouApp *app = (CBeidouApp *)AfxGetApp(); //����ָ��Ӧ�ó������ָ��
	app->m_nBaud=i;
	m_DBaud=app->m_nBaud;
	UpdateData();	
}

void CBeidouDlg::OnSelendokComboParity() 
{
	// TODO: Add your control notification handler code here
	char temp;
	int i=m_Parity.GetCurSel();
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
}

void CBeidouDlg::OnSelendokComboDatabits() 
{
	// TODO: Add your control notification handler code here
	int i=m_DataBits.GetCurSel();
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
}

void CBeidouDlg::OnSelendokComboStopbits() 
{
	// TODO: Add your control notification handler code here
	int i=m_StopBits.GetCurSel();
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
}

void CBeidouDlg::OnButtonSendMsg() 
{
	// TODO: Add your control notification handler code here
//		CString sendTmp="";
		UpdateData();
		if (m_otherID==0)
		{
			AfxMessageBox("������Է����š�");
			return;
		}
		if(m_sendmsg!="")
		{
			unsigned char hexdata[100];
			int len=strHex(m_sendmsg,hexdata,100);
			CByteArray Array;
			Array.RemoveAll();
			int total_len=len+17;
			Array.SetSize(total_len);

			frame_MSG_check[5]=total_len/256;//���ݰ��ܳ���
			frame_MSG_check[6]=total_len%256;

			frame_MSG_check[10]=m_otherID/pow(2, 16);//�Է�ID
			frame_MSG_check[11]=m_otherID/pow(2, 8);
			frame_MSG_check[12]=m_otherID%256;

			frame_MSG_check[13]=0x46;//Ĭ��ʹ�á����롱��ʽ��������

			frame_MSG_check[14]=8*len/256;//���ĳ���
			frame_MSG_check[15]=8*len%256;

			
			for (int i=0; i<16; i++)
			{
				Array.SetAt(i,frame_MSG_check[i]);
			}
			int k=0;
			for (int j=16; j<(len+16); j++)
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
		}
 
		SetTimer(1,60000,NULL);//61���ʹ��
		SetTimer(2,1000,NULL);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
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
	Array.SetSize(11);
	for (int i=0; i<11; i++)
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
	Array.SetSize(11);
	for (int i=0; i<11; i++)
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

	_Useraddr=((long)BUFF[10])*pow(2, 16)+((long)BUFF[11])*pow(2, 8)+((long)BUFF[12]);
	IccFrq= ((int)BUFF[13])*pow(2, 8)+((int)BUFF[14]);
	comlev = BUFF[15];
	
	if ((comm_init==0)&&(_Useraddr!=0))//���γ�ʼ�����ڣ��������������ɸ�֡
	{
		comm_init=1;
		frame_POWER_check[7]=BUFF[10];
		frame_POWER_check[8]=BUFF[11];
		frame_POWER_check[9]=BUFF[12];
		frame_POWER_check[10]=0;
		frame_POWER_check[11]=XOR(frame_POWER_check,11);

		frame_SYS_check[7]=BUFF[10];
		frame_SYS_check[8]=BUFF[11];
		frame_SYS_check[9]=BUFF[12];
		frame_SYS_check[10]=XOR(frame_SYS_check,10);

		frame_MSG_check[7]=BUFF[10];
		frame_MSG_check[8]=BUFF[11];
		frame_MSG_check[9]=BUFF[12];
		
		GetDlgItem(IDC_BUTTON_SYSTEMCHECK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON3_POWERCHECK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
	}

	m_cardnumber=_Useraddr; 
	m_category=IccFrq;
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
		if(strncmp((const char *)BUFF, "$Sig_",5)==0) command_type= Sig;
		else if (strncmp((const char *)BUFF, "$Pos_",5)==0) command_type= Pos;
		else if (strncmp((const char *)BUFF, "$Msg_",5)==0) command_type= Msg;
		else if (strncmp((const char *)BUFF, "$Bst_",5)==0) command_type= Bst;
		else if (strncmp((const char *)BUFF, "$Icc_",5)==0) command_type= Icc;
		else if (strncmp((const char *)BUFF, "$Sts_",5)==0) command_type= Sts;
		else if (strncmp((const char *)BUFF, "$Zst_",5)==0) command_type= Zst;
		else if (strncmp((const char *)BUFF, "$Zrd_",5)==0) command_type= Zrd;
		else if (strncmp((const char *)BUFF, "$Tim_",5)==0) command_type= Tim;
		else if (strncmp((const char *)BUFF, "$Fbk_",5)==0) command_type= FKXX;
		
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
	long _Useraddr;
	unsigned char Sigx[6]={0},power_buf=0;

	for(short i = 0;i<6;i++)
	Sigx[i]=BUFF[i+10];
	_Useraddr=((long)BUFF[7])*pow(2, 16)+((long)BUFF[8])*pow(2, 8)+((long)BUFF[9]);

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
 
 	Recvaddr=((long)BUFF[10])*pow(2,16)+((long)BUFF[11])*pow(2,8)+((long)BUFF[12]);
 	Hour = BUFF[13];
 	Min = BUFF[14];
 	len = ((int)BUFF[15])*pow(2,8)+((int)BUFF[16]);
 	for(i=0;i<(int)(len/8);i++)
 	{
 		rec_text[i]=BUFF[17+i];
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
	m_showmsg+="�յ���Ϣ��";
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
		m_FKXX="���ͳɹ���";
		break;
	case 0x01:
		m_FKXX="����ʧ�ܣ�";
		break;
	case 0x02:
		m_FKXX="�ź�Ϊ������";
		break;
	case 0x03:
		m_FKXX="�������㣡";
		break;
	case 0x04:
		m_FKXX="����Ƶ��δ����\r\n��"+temp+"������ԡ�";
		break;
	case 0x05:
		m_FKXX="�ӽ��ܴ���";
		break;
	case 0x06:
		m_FKXX="CRC����";
		break;
	default:
		m_FKXX="��չ���ܡ�";
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
		m_timer.StepIt();
	if(m_timer.GetPos()==60){
		m_timer.SetPos(0);
		KillTimer(2);	
       }
		UpdateData(FALSE);
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CBeidouDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}
