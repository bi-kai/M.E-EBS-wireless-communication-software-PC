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
	char TP_SCTS[36];		// ����ʱ����ַ���(TP_SCTS), ����ʱ�õ� //ԭ��16���Լ������˸�ʽ
	char TP_UD[200];		// ԭʼ�û���Ϣ(����ǰ�������TP-UD)  160+40,���ǵ�������
	CString index;			// ����Ϣ��ţ��ڶ�ȡʱ�õ�
} SM_PARAM;

typedef struct {
	SM_PARAM SMSInfo;  //����SMS��Ϣ
	int nToReturn;		//Ҫ���ص�����ֵ
	bool bIsLongSMS;	//�Ƿ�Ϊ������
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

funReturn cEncodePDU(CString strDstNum, CString strSMSText,char * chForPDU,BOOL bChineseFlag,int nTotalLenth);
funReturn cEncodePDU(CString strSrcNum,CString strDstNum,CString strSMSText,char * chForPDU,BOOL bChineseFlag,int nTotalLenth);
SMSInfoALL nDecodePdu(char *pSrc, SMSInfoALL smsa,int nTxRxFlag);
int  LongSMSTextDeCode(char *pSrc, SM_PARAM *pDst,int ndecodeMode,int nLength);

void CMSError(CString str);

#endif