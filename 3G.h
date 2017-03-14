#ifndef  _3G_H_   //xxxx可以你任意定
#define  _3G_H_   //跟上面的xxxx相同
//编码标志
#define CODE_7BIT 0
#define CODE_8BIT 4
#define CODE_Unicode 8

// 短消息参数结构，编码/解码共用
// 其中，字符串以'\0'结尾
typedef struct {
	char SCA[16];			// 短消息服务中心号码(SMSC地址)
	char TPA[16];			// 目标号码或回复号码(TP-DA或TP-RA)
	char TP_PID;			// 用户信息协议标识(TP-PID)
	char TP_DCS;			// 用户信息编码方式(TP-DCS)
	char TP_SCTS[36];		// 服务时间戳字符串(TP_SCTS), 接收时用到 //原来16，自己添加了格式
	char TP_UD[200];		// 原始用户信息(编码前或解码后的TP-UD)  160+40,考虑到长短信
	int index;			// 短消息序号，在读取时用到
} SM_PARAM;

typedef struct {
	SM_PARAM SMSInfo;  //所有SMS信息
	int nToReturn;		//要返回的整数值
	bool bIsLongSMS;	//是否为长短信
	int nLongSMSCount;	//长短信的位置
	CString strLongSMSID;//长短信ID
	int nSMSTotalNum; //返回长短信总的条数
}SMSInfoALL; 

typedef struct {
	int nEnglishLenth;    //英文字符数
    int nChineseLenth;    //中文字符数
    int nTotalLenth;//总共字符数
}strCount; 

typedef struct {
    bool bToReturn;    //返回值，是否正确，
    int nLenthToReturn;//总共字符数
}funReturn; 

typedef struct {
	CString strDstNum;
	CString strSMSText;
	BOOL bChineseFlag;
	int nTextLenth;
	int nCount;//总共条数
	int nStrSendLoc; //the begin of string location to send one SMS
	int nLongSMSID; //the unique identifiers
}strLongSMS; 

typedef struct {
	int ntoreturn;//要返回的整数值
	CString strID;//返回长短信的ID
	int nAllNum; //返回长短信总的条数
}LSMSRETURN;

strCount Statistic(CString str); //统计字符函数

int checkPhoneNum(const char *pSrc, int nSrcLength);
funReturn ReversePhoneNum(char* pSrc, char* pDst, int nSrcLength);
int RecoverPhoneNum(char* pSrc, char* pDst, int nSrcLength);
int RecoverTime(char *pSrc,char *pDst,int nSrcLength);

int nBytes2CString(const unsigned char* pSrc, char* pDst, int nSrcLength);
int nCString2Bytes(const char* pSrc, unsigned char* pDst, int nSrcLength);//一般取长度用
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