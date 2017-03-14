#ifndef  _3G_H_   //xxxx���������ⶨ
#define  _3G_H_   //�������xxxx��ͬ
//�����־
#define CODE_7BIT 0
#define CODE_8BIT 4
#define CODE_Unicode 8

// ����Ϣ�����ṹ������/���빲��
// ���У��ַ�����'\0'��β
typedef struct {
	char SCA[16];			// ����Ϣ�������ĺ���(SMSC��ַ)
	char TPA[16];			// Ŀ������ظ�����(TP-DA��TP-RA)
	char TP_PID;			// �û���ϢЭ���ʶ(TP-PID)
	char TP_DCS;			// �û���Ϣ���뷽ʽ(TP-DCS)
	char TP_SCTS[36];		// ����ʱ����ַ���(TP_SCTS), ����ʱ�õ� //ԭ��16���Լ�����˸�ʽ
	char TP_UD[200];		// ԭʼ�û���Ϣ(����ǰ�������TP-UD)  160+40,���ǵ�������
	int index;			// ����Ϣ��ţ��ڶ�ȡʱ�õ�
} SM_PARAM;

typedef struct {
	SM_PARAM SMSInfo;  //����SMS��Ϣ
	int nToReturn;		//Ҫ���ص�����ֵ
	bool bIsLongSMS;	//�Ƿ�Ϊ������
	int nLongSMSCount;	//�����ŵ�λ��
	CString strLongSMSID;//������ID
	int nSMSTotalNum; //���س������ܵ�����
}SMSInfoALL; 

typedef struct {
	int nEnglishLenth;    //Ӣ���ַ���
    int nChineseLenth;    //�����ַ���
    int nTotalLenth;//�ܹ��ַ���
}strCount; 

typedef struct {
    bool bToReturn;    //����ֵ���Ƿ���ȷ��
    int nLenthToReturn;//�ܹ��ַ���
}funReturn; 

typedef struct {
	CString strDstNum;
	CString strSMSText;
	BOOL bChineseFlag;
	int nTextLenth;
	int nCount;//�ܹ�����
	int nStrSendLoc; //the begin of string location to send one SMS
	int nLongSMSID; //the unique identifiers
}strLongSMS; 

typedef struct {
	int ntoreturn;//Ҫ���ص�����ֵ
	CString strID;//���س����ŵ�ID
	int nAllNum; //���س������ܵ�����
}LSMSRETURN;

strCount Statistic(CString str); //ͳ���ַ�����

int checkPhoneNum(const char *pSrc, int nSrcLength);
funReturn ReversePhoneNum(char* pSrc, char* pDst, int nSrcLength);
int RecoverPhoneNum(char* pSrc, char* pDst, int nSrcLength);
int RecoverTime(char *pSrc,char *pDst,int nSrcLength);

int nBytes2CString(const unsigned char* pSrc, char* pDst, int nSrcLength);
int nCString2Bytes(const char* pSrc, unsigned char* pDst, int nSrcLength);//һ��ȡ������
int nEncode7bit(const char* pSrc, unsigned char* pDst, int nSrcLength);
int nDecode7bit(const unsigned char* pSrc, char* pDst, int nSrcLength);
int nDecode8bit(const unsigned char* pSrc, char* pDst, int nSrcLength);
int nEncodeUnicode(const char* pSrc, unsigned char* pDst, int nSrcLength);
int UnicodeToGB2312(const unsigned char *pSrc,char *pDst,int nLength);

funReturn cEncodePDU(CString strDstNum, CString strSMSText,char * chForPDU,BOOL bChineseFlag,int nLenth);
funReturn cEncodePDU(CString strSrcNum,CString strDstNum,CString strSMSText,char * chForPDU,BOOL bChineseFlag,int Lenth);
SMSInfoALL nDecodePdu(char *pSrc, SMSInfoALL smsa,int nTxRxFlag);
LSMSRETURN  LongSMSTextDeCode(char *pSrc, SM_PARAM *pDst,int ndecodeMode,int nLength);

void CMSError(CString str);
funReturn cEncodeLongPDU(CString strDstNum, CString strSMSText,char * chForPDU,BOOL bChineseFlag,int nLenth,CString nOrder,CString TotalOrder,int nID);

#endif