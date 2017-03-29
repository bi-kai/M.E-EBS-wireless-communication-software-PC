#include "stdafx.h"
#include <wchar.h>
#include <stdio.h>
#include "3G.h"

//验证号码的情况
//+8615179141171 长度14 ，返回1
//8615179141171 长度13，返回2
//15179141171 长度11，返回3
//0表示出错，如长度12，或者不是手机号码
int checkPhoneNum(const char *pSrc, int nSrcLength)
{	
	int i;
	if (nSrcLength == 14)
	{
		i = 1;//要删除+
		return i;
	}
	else if (nSrcLength == 13)
	{
		i = 2;
		return i;
	}
	else if (nSrcLength == 11)
	{
		if((*pSrc=='1'))
		{
			i = 3;
		}
		else if(*(pSrc+1)>='3' || *(pSrc+1)<='8' )
		{		
			i = 3;				
		}
		else
		{		
			i = 0;				
		}
		return i;
	}
	else
	{
		i=0;
		return i;
	}
}
// 两两颠倒的字符串转换为正常顺序的字符串
// 如："683158812764F8" --> "8613851872468"
// 输入: pSrc - 源字符串指针
//       nSrcLength - 源字符串长度
// 输出: pDst - 目标字符串指针
// 返回: 目标字符串长度
int RecoverPhoneNum(char* pSrc, char* pDst, int nSrcLength)
{
	int nDstLength;
	nDstLength = nSrcLength;
	if (nDstLength == 14)
	{
			*(pDst) = '+';
			for (int i=0;i<nSrcLength;i+=2)
			{
		//		chTemp =*(pSrc+i);
				*(pDst+i+1) = *(pSrc+i+1);
				*(pDst+i+2) = *(pSrc+i);	
			}
			if(*(pDst+nDstLength-1) == 'F')
			{
				*(pDst+nDstLength-1) = '\0';//除去'F'	
			}
	} 
	else if (nDstLength == 12)
	{
			for (int i=0;i<nSrcLength;i+=2)
			{
				*(pDst+i) = *(pSrc+i+1);
				*(pDst+i+1) = *(pSrc+i);	
			}
			if(*(pDst+nDstLength-1) == 'F')
			{
				*(pDst+nDstLength-1) = '\0';//除去'F'	
			}
	}
	else if (nDstLength == 6)
	{
			for (int i=0;i<nSrcLength;i+=2)
			{
				*(pDst+i) = *(pSrc+i+1);
				*(pDst+i+1) = *(pSrc+i);	
			}
			if(*(pDst+nDstLength-1) == 'F')
			{
				*(pDst+nDstLength-1) = '\0';//除去'F'	
			}
	}
	else
	{
			for (int i=0;i<nSrcLength;i+=2)
			{
				*(pDst+i) = *(pSrc+i+1);
				*(pDst+i+1) = *(pSrc+i);	
			}
			if(*(pDst+nDstLength-1) == 'F')
			{
				*(pDst+nDstLength-1) = '\0';//除去'F'	
			}
	}
	*(pDst+nDstLength) = '\0';//除去'F'
	return nDstLength;
}
// 正常顺序的字符串转换为两两颠倒的字符串，若长度为奇数，补'F'凑成偶数
// 如："8613851872468" --> "683158812764F8"
// 输入: pSrc - 源字符串指针
//       nSrcLength - 源字符串长度
// 输出: pDst - 目标字符串指针
// 返回: 目标字符串长度
funReturn ReversePhoneNum(char *pSrc, char *pDst, int nSrcLength)
{
	funReturn a;
	a.bToReturn = TRUE;
	a.nLenthToReturn =0;

	int nDstLength,nNumStatus,i;		// 目标字符串长度 号码状态
	char chTemp;			// 用于保存一个字符
	nDstLength = nSrcLength;	// 复制串长度
	nNumStatus = checkPhoneNum(pSrc,nSrcLength);
	switch (nNumStatus) //操作14:0~13
	{
	case 1://删除最前的+，最后添加F ,长度14
		for (i=0;i<=nDstLength-2;i++) //0~12
		{
			*(pDst+i) = *(pSrc+i+1);
		}
		*(pDst+nDstLength-1)='F'; //13
		break;
	case 2://最后添加F ,长度13
		for (i=0;i<=nDstLength-1;i++) //0~12
		{
			*(pDst+i) = *(pSrc+i);
		}
		nDstLength++;//长度14
		*(pDst+nDstLength-1)='F'; //13
		break;
	case 3://前面加86，最后添加F,长度11
		*pDst ='8';//0
		*(pDst+1) ='6';//1
		for (i=0;i<=nDstLength-1;i++) //(0~10) 2~12
		{
			*(pDst+i+2) = *(pSrc+i);
		}
		nDstLength += 3;//长度14
		*(pDst+nDstLength-1)='F'; //13
		break;
	default:
		a.bToReturn = FALSE;
		AfxMessageBox("对方号码长度有误，请核实后重试！");
		break;
	}
	
	for(i=0;i<=nDstLength-1;i+=2)//0~13
	{
		chTemp = *(pDst+i);		// 保存先出现的字符
		*(pDst+i) = *(pDst+i+1);	// 复制后出现的字符
		*(pDst+i+1) = chTemp;		// 复制先出现的字符
	}
	*(pDst+nDstLength) = '\0';//14,字符结束
	a.nLenthToReturn = nDstLength;
	return a;
//	return nDstLength;
}
//解析时间
//注意必须要给pDst分配足够长的空间；否则会超长
// 01 40 82 11 54 44 23(14个)---->2010/04/28 11:45:44 (19+'\0',20个) 再加上Time Zone: (32个字符)
//插入了5个字符（包括空格）
//---------------恢复时间
//01408211544423->2010/04/28 11:45:44 
int RecoverTime(char *pSrc, char *pDst, int nSrcLength)
{ //或者先复制下，翻转后再加
	char chTemp;
	*pDst = '2';
	*(pDst+1) = '0';
	for (int i=0;i<nSrcLength;i++)
	{
		if (i==0)//0
		{
			*(pDst+i+2) = *(pSrc+i);
		} 
		else if (i==1)//1
		{
			*(pDst+i+2) = *(pSrc+i);
			chTemp = *(pDst+i+2);//pDst,23转换
			*(pDst+i+2) = *(pDst+i+1);
			*(pDst+i+1) = chTemp;
		} 
		else if(i==2)//2
		{
			*(pDst+i+2) = '/';
			*(pDst+i+3) = *(pSrc+i);
		}
		else if (i==3)//3
		{
			*(pDst+i+3) = *(pSrc+i);
			chTemp = *(pDst+i+3);//pDst,56转换
			*(pDst+i+3) = *(pDst+i+2);
			*(pDst+i+2) = chTemp;
		}
		else if(i==4)//4
		{
			*(pDst+i+3) = '/';
			*(pDst+i+4) = *(pSrc+i);
		}
		else if(i==5)//5
		{
			*(pDst+i+4) = *(pSrc+i);
			chTemp = *(pDst+i+4);//pDst,89转换
			*(pDst+i+4) = *(pDst+i+3);
			*(pDst+i+3) = chTemp;
			*(pDst+i+5) = '|';
		}
		else if(i==6)//6
		{
			*(pDst+i+5) = *(pSrc+i);
		}
		else if(i==7)//7
		{
			*(pDst+i+5) = *(pSrc+i);
			chTemp = *(pDst+i+5);//pDst,1112转换
			*(pDst+i+5) = *(pDst+i+4);
			*(pDst+i+4) = chTemp;
		}
		else if(i==8)//8
		{
			*(pDst+i+5) = ':';
			*(pDst+i+6) = *(pSrc+i);
		}
		else if(i==9)//9
		{
			*(pDst+i+6) = *(pSrc+i);
			chTemp = *(pDst+i+6);//pDst,1415转换
			*(pDst+i+6) = *(pDst+i+5);
			*(pDst+i+5) = chTemp;
		}
		else if(i==10)//10
		{
			*(pDst+i+6) = ':';
			*(pDst+i+7) = *(pSrc+i);
		}
		else if(i==11)//11
		{
			*(pDst+i+7) = *(pSrc+i);
			chTemp = *(pDst+i+7);//pDst,1718转换
			*(pDst+i+7) = *(pDst+i+6);
			*(pDst+i+6) = chTemp;
		}
		else if(i==12)//12
		{
			char ch[] = "|Time Zone:";
			for (int k=0;k<11;k++)
			{
				*(pDst+i+7+k) = ch[k];
			}
			*(pDst+i+18) = *(pSrc+i);
		}
		else if(i==13)//13
		{
			*(pDst+i+18) = *(pSrc+i);
			chTemp = *(pDst+i+18);//pDst,3031转换
			*(pDst+i+18) = *(pDst+i+17);
			*(pDst+i+17) = chTemp;
		}
	}
	*(pDst+nSrcLength+18) = '\0';
	return nSrcLength+18;
}
// 字节数据转换为可打印字符串
// 如：{0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} --> "C8329BFD0E01" 
// 输入: pSrc - 源数据指针
//       nSrcLength - 源数据长度
// 输出: pDst - 目标字符串指针
// 返回: 目标字符串长度
int nBytes2CString(const unsigned char* pSrc, char* pDst, int nSrcLength)
{
	char tab[] = "0123456789ABCDEF"; //0x0~0xF 
	for (int i=0;i<nSrcLength;i+=2)
	{
		*pDst = tab[*pSrc >> 4]; //右移4位()
		pDst++;
		*pDst++ = tab[*pSrc & 0x0f]; // 输出低4位
	//	pDst++;
		pSrc++;
	}
		// 输出字符串加个结束符
	*pDst = '\0';
	return nSrcLength*2;	
}
// 可打印字符串转换为字节数据
// 如："C8329BFD0E01" --> {0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01}
// 输入: pSrc - 源字符串指针
//       nSrcLength - 源字符串长度
// 输出: pDst - 目标数据指针
// 返回: 目标数据长度 (原来长度的一半)
int nCString2Bytes(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
	for (int i=0;i<nSrcLength;i+=2)
	{
		//输出高4位
		if ((*pSrc >='0') && (*pSrc <= '9'))
		{
			*pDst = (*pSrc - '0') << 4;//差值左移4位
		} 
		else
		{
			*pDst = (*pSrc - 'A'+ 10) << 4;//差值左移4位
		}
		pSrc++;
		// 输出低4位  
		if ((*pSrc>='0') && (*pSrc<='9'))
		{
			*pDst |= *pSrc - '0';
		}
		else
		{
			*pDst |= *pSrc - 'A' + 10;
		}
		pSrc++;
		pDst++;
	}
	return nSrcLength/2;
}
// 7bit解码
// 输入: pSrc - 源编码串指针
//       nSrcLength - 源编码串长度
// 输出: pDst - 目标字符串指针
// 返回: 目标字符串长度
int nDecode7bit(const unsigned char* pSrc, char* pDst, int nSrcLength)
{
//	/*
	int nSrc,nDst;
	nSrc = 0;
	nDst = 0;
	int j = 0;
	int n = 0x7F;
	for(int i=0;i < nSrcLength;i++)
		{
				if (i == 6 )
				{
					*(pDst) = (*pSrc & n); //取最后一位
					*(pDst) = (*(pDst) << i); //7-i位回自己位置
					*(pDst) |= (*(pSrc-1) >> (8-i));
					pDst++;//7字节变8字节;
					n = 0xFE;			
					*(pDst) = (*pSrc & n);
					*(pDst) = (*(pDst) >> 1);//第8字节
					*(pDst) &= 0x7F;
					n = 0x7F;
					if (nSrcLength > 7)
					{	
						nSrcLength -= 7;
						i = -1;
						pSrc++;
						pDst++;
						j++;
					}
				}
				else
				{
					if (i==0)
					{
						*(pDst) = (*pSrc & n); //取后7位
						n = n / 2; //除以2						
					}
					if (i>0)
					{
						*(pDst) = (*pSrc & n); //取后7-i位
						n = n / 2;  //除以2
						*(pDst) = (*(pDst) << i); //7-i位回自己位置
						*(pDst) |= (*(pSrc-1) >> (8-i));
					}
					pSrc++;
					pDst++;
					nDst++;

				}
	}
	*pDst = '\0';
	return (nDst+j)/2; 
}
// 8bit解码
// 输入: pSrc - 源编码串指针
//       nSrcLength -  源编码串长度
// 输出: pDst -  目标字符串指针
// 返回: 目标字符串长度
int nDecode8bit(const unsigned char* pSrc, char* pDst, int nSrcLength)
{	
	memcpy(pDst, pSrc, nSrcLength);	// 简单复制
	*pDst = '\0';// 输出字符串加个结束符
	return nSrcLength;
}
//-----------------------
//7位编码，主要用于英文格式，以及（TEXT格式）
//8    7    6    5    4    3    2    1
//b1  a7  a6  a5  a4  a3  a2  a1
//c2  c1  b7  b6  b5  b4  b3  b2
//d3  d2  d2  c7  c6  c5  c4  c3
//e4  e3  e2  e1  d7  d6  d5  d4
//f5   f4   f3   f2   f1  e7   e6  e5
//g6  g5  g4  g3  g2  g1  f7   f6
//0    0    0    0   0     0    0   g7
//-----------------------
int nEncode7bit(const char* pSrc, unsigned char* pDst, int len)
{
	int nSrc,nDst;
	nSrc = 0;
	nDst = 0;
	int j = 0;
	for(int i=0;i < len;i++)
		{
				if (i == 7 )
				{
					*(pDst) = (*pSrc >> i);
					*(pDst) = 0;
					if (len > 8)
					{
						len -= 8;
						i = -1;
						pSrc++;
						j++;
					}
				}
				else
				{
					if (*pSrc == 0x0d)  //可能会出问题
					{
						break;
					}
					*(pDst) = (*pSrc >> (i));
					pSrc++;
					nDst++;
					if (*pSrc == 0x0d)
					{
						break;
					}
					*(pDst) |= (*pSrc << (7-i));
					pDst++;			
				}
	}
	*(pDst+1) = '\0';
	return nDst;
}
// 输出: pDst - 目标编码串指针
// 返回: 目标编码串长度
int nEncodeUnicode(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
	 int nDstLength;        // UNICODE宽字符数目
     WCHAR wchar[128];  // UNICODE串缓冲区 
     // 字符串-->UNICODE串
     nDstLength = MultiByteToWideChar(CP_ACP, 0, pSrc, nSrcLength, wchar, 128); 
     // 高低字节对调，输出
     for(int i=0; i<nDstLength; i++)
     {
         *pDst++ = wchar[i] >> 8;         // 先输出高位字节
         *pDst++ = wchar[i] & 0xff;       // 后输出低位字节
     }
     // 返回目标编码串长度
     return nDstLength * 2;
}

int UnicodeToGB2312(const unsigned char *pSrc,char *pDst,int nLength)
{
     int nDstLength;        // UNICODE宽字符数目
     WCHAR wchar[256];  // UNICODE串缓冲区
	 wmemset(wchar,0,sizeof(wchar)/sizeof(wchar_t));
 
     // 高低字节对调，拼成UNICODE
     for(int i=0; i<nLength/2; i++)
     {
           wchar[i] = *pSrc++ << 8;    // 先高位字节
         wchar[i] |= *pSrc++;        // 后低位字节
     } 
     // UNICODE串-->字符串 //CP_ACP
     nDstLength = WideCharToMultiByte(936, 0, wchar, nLength/2, pDst, 160, NULL, NULL);
     // 输出字符串加个结束符
     pDst[nDstLength] = '\0';
     // 返回目标字符串长度
     return nDstLength;
}
//不完整完整的PDU编码
//缺少strSrcNum 发件人号码  即为本机号码  00表示用手机上设置短信中心号码
//strDstNum 收件人号码
//strSMSText 发送内容
//chForPDU PDU存储指针
//返回TPDU的长度
funReturn	cEncodePDU(CString strDstNum, CString strSMSText, char *chForPDU,BOOL bChineseFlag,int nLenth)
{
	
	funReturn b;
	b.bToReturn =TRUE;
	b.nLenthToReturn =0;

	char *cDstNum;
	cDstNum = strDstNum.GetBuffer(strDstNum.GetLength()); 
	strDstNum.ReleaseBuffer(); //dst number
	
	char chText[512];
	memcpy(chText,strSMSText,strSMSText.GetLength());
	int	len = strSMSText.GetLength() +2;
	chText[len-2] = '\r';
	chText[len-1] = '\n';
	//-----------------------------
	int nTPDUlen = 0;
	int nPos = 0;
	//-----------------------本机号码
//00  00表示用手机上设置短信中心号码
	chForPDU[nPos] = '0';
	chForPDU[nPos+1] = '0';
	nPos += 2;//长度不算
//11   51 基本参数(TP-MTI/VFP) 不要求发送回复 或者 31  01 11
	chForPDU[nPos] = '1';
	chForPDU[nPos+1] = '1';
	nPos += 2;
	nTPDUlen += 2;
//00 消息基准值(TP-MR)
	chForPDU[nPos] = '0';
	chForPDU[nPos+1] = '0';
	nPos += 2;
	nTPDUlen += 2;
//-----------------------------收件人
//长度0D
	chForPDU[nPos] = '0';
	chForPDU[nPos+1] = 'D';
	nPos += 2;
	nTPDUlen += 2;
//91
	chForPDU[nPos] = '9';
	chForPDU[nPos+1] = '1';
	nPos += 2;
	nTPDUlen += 2;
//号码
	b = ReversePhoneNum(cDstNum,&chForPDU[nPos],strDstNum.GetLength());
	if(b.bToReturn==FALSE)
	{
		return b;
	}
	nPos += 14;
	nTPDUlen += 14;
//00 TP-PID
	chForPDU[nPos] = '0';
	chForPDU[nPos+1] = '0';
	nPos += 2;
	nTPDUlen += 2;
//08 TP-DCS  Unicode编码           这里还要加个7bit编码，（8bit待定）
	if (bChineseFlag)
	{
		chForPDU[nPos] = '0';
		chForPDU[nPos+1] = '8';
		nPos += 2;	
		nTPDUlen += 2;
	} 
	else //	这里还要加个7bit编码，
	{
		chForPDU[nPos] = '0';
		chForPDU[nPos+1] = '0';
		nPos += 2;	
		nTPDUlen += 2;
	}
//00  可变   A7 AA
	chForPDU[nPos] = 'A';
	chForPDU[nPos+1] = 'A';
	nPos += 2;
	nTPDUlen += 2;
	if (bChineseFlag)
	{
		//TP-UDL
	char a = nLenth*2;
	nBytes2CString((unsigned char *)&a,&chForPDU[nPos],2);//TP-UDL
	nPos += 2;
	nTPDUlen += 2;
//TP-UD
		unsigned char buf[256];
		int i = nEncodeUnicode(chText,buf,strSMSText.GetLength());
		int j=nBytes2CString(buf,&chForPDU[nPos],i*2);
		nTPDUlen += i*2;
//		PDUlength = nPos + i*2; //PDU 总长度
		chForPDU[nPos + i*2] = 0x1a;//     ctrl+z   在函数cEncodePDU中实现
		chForPDU[nPos + i*2+1] = '\0';
	} 
	else
	{
//TP-UDL
	char a = nLenth;
	nBytes2CString((unsigned char *)&a,&chForPDU[nPos],2);//TP-UDL
	nPos += 2;
	nTPDUlen += 2;
//TP-UD
		//7bit编码
		unsigned char buf[256];
		int i = nEncode7bit(chText,buf,strSMSText.GetLength());
		int j=nBytes2CString(buf,&chForPDU[nPos],i*2);
		nTPDUlen += i*2;
//		PDUlength = nPos + i*2; //PDU 总长度
		chForPDU[nPos + i*2] = 0x1a;//     ctrl+z  在函数cEncodePDU中实现
		chForPDU[nPos + i*2+1] = '\0';
	}
	//---------------------返回PDU
	b.nLenthToReturn = nTPDUlen/2;
	return b;
//	return nTPDUlen/2;
}
//完整的PDU编码
//strSrcNum 发件人号码  strDstNum 收件人号码  strSMSText 发送内容    chForPDU PDU存储指针  bChineseFlag 是否有中文字符，TRUE为有，用Unicode编码；FALSE用7bit编码
//返回TPDU的长度
funReturn  cEncodePDU(CString strSrcNum, CString strDstNum, CString strSMSText,char * chForPDU,BOOL bChineseFlag,int nLenth)
{
	funReturn c;
	c.bToReturn =  TRUE;
	c.nLenthToReturn = 0;
	//三个CString在内部要转char (三种方式)
	char *cSrcNum;
	cSrcNum = (LPSTR)(LPCTSTR)strSrcNum;//src number

	char *cDstNum;
	cDstNum = strDstNum.GetBuffer(strDstNum.GetLength()); 
	strDstNum.ReleaseBuffer(); //dst number
	
	char chText[512];
	memcpy(chText,strSMSText,strSMSText.GetLength());
	int	len = strSMSText.GetLength() +2;
	chText[len-2] = '\r';
	chText[len-1] = '\n';
	//-----------------------------
	int nTPDUlen = 0;
	int nPos = 0;
//08
	chForPDU[nPos] = '0';
	chForPDU[nPos+1] = '8';
	nPos += 2;
//91 
	chForPDU[nPos] = '9';
	chForPDU[nPos+1] = '1';
	nPos += 2;
	//-----------------------------发件人
	c = ReversePhoneNum(cSrcNum,&chForPDU[nPos],strSrcNum.GetLength());
	if(c.bToReturn==FALSE)
	{
		return c;
	}
	nPos += 14;
//暂设为1100
	chForPDU[nPos] = '1';
	chForPDU[nPos+1] = '1';
	chForPDU[nPos+2] = '0';
	chForPDU[nPos+3] = '0';
	nPos += 4;
	nTPDUlen += 4;
//-----------------------------收件人
//长度0D
	chForPDU[nPos] = '0';
	chForPDU[nPos+1] = 'D';
	nPos += 2;
	nTPDUlen += 2;
//91
	chForPDU[nPos] = '9';
	chForPDU[nPos+1] = '1';
	nPos += 2;
	nTPDUlen += 2;
//号码
	c = ReversePhoneNum(cDstNum,&chForPDU[nPos],strDstNum.GetLength());
	if(c.bToReturn==FALSE)
	{
		return c;
	}
	nPos += 14;
	nTPDUlen += 14;
//00 TP-PID
	chForPDU[nPos] = '0';
	chForPDU[nPos+1] = '0';
	nPos += 2;
	nTPDUlen += 2;
//08 TP-DCS  Unicode编码  （8bit待定）
	if (bChineseFlag)
	{
		chForPDU[nPos] = '0';
		chForPDU[nPos+1] = '8';
		nPos += 2;	
		nTPDUlen += 2;
	} 
	else //	这里还要加个7bit编码，
	{
		chForPDU[nPos] = '0';
		chForPDU[nPos+1] = '0';
		nPos += 2;	
		nTPDUlen += 2;
	}
//00  可变   A7 AA
	chForPDU[nPos] = 'A';
	chForPDU[nPos+1] = 'A';
	nPos += 2;
	nTPDUlen += 2;
	if (bChineseFlag)
	{
		//TP-UDL
	char a = nLenth*2;
	nBytes2CString((unsigned char *)&a,&chForPDU[nPos],2);//TP-UDL
	nPos += 2;
	nTPDUlen += 2;
//TP-UD
		unsigned char buf[256];
		int i = nEncodeUnicode(chText,buf,strSMSText.GetLength());
		int j=nBytes2CString(buf,&chForPDU[nPos],i*2);
		nTPDUlen += i*2;
//		PDUlength = nPos + i*2; //PDU 总长度
		chForPDU[nPos + i*2] = 0x1a;//     ctrl+z   在函数cEncodePDU中实现
		chForPDU[nPos + i*2+1] = '\0';
//这里补上长度
	} 
	else
	{
//TP-UDL
	char a = nLenth;
	nBytes2CString((unsigned char *)&a,&chForPDU[nPos],2);//TP-UDL
	nPos += 2;
	nTPDUlen += 2;
//TP-UD
		//7bit编码
		unsigned char buf[256];
		int i = nEncode7bit(chText,buf,strSMSText.GetLength());
		int j=nBytes2CString(buf,&chForPDU[nPos],i*2);
		nTPDUlen += i*2;
//		PDUlength = nPos + i*2; //PDU 总长度
		chForPDU[nPos + i*2] = 0x1a;//     ctrl+z  在函数cEncodePDU中实现
		chForPDU[nPos + i*2+1] = '\0';
	}
	//---------------------返回PDU
	c.nLenthToReturn = nTPDUlen/2;
	return c;
//	return nTPDUlen/2;
}
/*------分析接受短信
接收数据： 08 91683108200105F0 04 0D 91683184821969F2 000470404271726423026869 
码段                       含义                           说明 
08                         SMSC地址信息的长度             共8个八位字节(包括91) 
91                         SMSC地址格式(TON/NPI)          用国际格式号码(在前面加’+’) 
683108200105F0             SMSC地址                       8613800210500，补’F’凑成偶数个 
04                         基本参数(TP-MTI/MMS/RP) 
0D                         回复地址数字个数 
91                         回复地址格式(TON/NPI) 
683184821969F2             回复地址(TP-RA)                8613482891962，补’F’凑成偶数个 
00                         协议标识(TP-PID)               是普通GSM类型，点到点方式 
04                         用户信息编码方式(TP-DCS)       8-bit编码 
70404271726423             时间戳(TP-SCTS)                07-04-24 17:27:46  23表示时间区 
02                         用户信息长度(TP-UDL)           实际长度2个字节 
6869                       用户信息(TP-UD)                0x68 0x69 
// 返回值： 0 表示解析不成功，1,表示解析成功   //应先做判断，nPos才能移动
//nTxRxFlag接受还是发送的标志，0、1表示接受，2、3表示发送
*/
SMSInfoALL nDecodePdu(char *pSrc, SMSInfoALL smsa,int nTxRxFlag)
{
	smsa.nToReturn = 0;
	SM_PARAM *pDst = &smsa.SMSInfo;
	int nDstLength=0; //用于解析PDU时，（表示其中某些字符量存的）长度
	unsigned char uCharTmp;		// 内部用的临时字节变量
	unsigned char buf[256];	// 内部用的缓冲区
	int nPos=0;//表示字符位置(指针位置)
	LSMSRETURN L;
	if (nTxRxFlag==0||nTxRxFlag==1)//解析接受PDU
	{	//短信查询
				//08 SMSC开始
					nCString2Bytes(pSrc,&uCharTmp,2);
					nDstLength = (uCharTmp - 1) * 2; //长度14
					if (nDstLength != 14)
					{
						AfxMessageBox(" PDU SMSC error.");
						smsa.nToReturn=0;
					}
					nPos += 2;
				//91
					if ((*(pSrc+nPos) != '9') && (*(pSrc+nPos+1) != '1'))
					{
						AfxMessageBox(" SMSC现只能解析0x91国际型.");
						smsa.nToReturn=0;
					}
					nPos += 2;
				//目的号码 （收件人）
					RecoverPhoneNum(pSrc+nPos,pDst->SCA,nDstLength);//长度14
					nPos += nDstLength;				
				//PDU Type:04 First octet of this SMS-DELIVER message.意思很多可以不管。或者 24   解析TAP开始
					nPos += 2;
					//不用做什么。。。
				//0D  0A 05    08 
					nCString2Bytes(pSrc+nPos,&uCharTmp,2);
					int len = uCharTmp;
					if(len%2==1)//0D 0A 05
					{
						nPos += 2;
						//91  A1
						nPos += 2; //不用做什么		
						//来源号码
						RecoverPhoneNum(pSrc+nPos,pDst->TPA,len+1);//14(F)等
						nPos += (len+1);					
					}
					else if(len%2==0) //08
					{
						nPos += 2;
						//  A1
						nPos += 2; 	//不用做什么
						RecoverPhoneNum(pSrc+nPos,pDst->TPA,len);//8
						nPos += len;						
					}
				//00 TP-PID
					nCString2Bytes(pSrc+nPos,(unsigned char*)&pDst->TP_PID,2);
					smsa.SMSInfo.TP_PID = pDst->TP_PID;
					nPos += 2;
				//08表示Unicode TP-DCS
					nCString2Bytes(pSrc+nPos,(unsigned char*)&pDst->TP_DCS,2);
					smsa.SMSInfo.TP_DCS = pDst->TP_DCS;
					if ((*(pSrc+nPos) == '0') && (*(pSrc+nPos+1) == '8'))
					{
					} 
					else if ((*(pSrc+nPos) == '0') && (*(pSrc+nPos+1) == '0'))
					{
					} 
					else
					{
						AfxMessageBox("编码方式不是7位编码或者Unicode16位编码");
					}
					nPos += 2;
				//时间 TP-SCTS  如:01 40 82 11 54 44 23
					RecoverTime(pSrc+nPos,pDst->TP_SCTS,nDstLength);
					nPos += nDstLength;
				// 用户信息长度(TP-UDL)
					int j = nCString2Bytes(pSrc+nPos,&uCharTmp,2);
					len = uCharTmp;
					nPos += 2;
					CString strIndex;
					smsa.bIsLongSMS = FALSE;
					pDst->index = 0;
				//信息内容解码
					for(int index=0;index<=5;index++)
					{
						strIndex += *(pSrc+nPos+index); 
					}
					if(pDst->TP_DCS == CODE_7BIT)	
					{
						if (j<140)
						{	// 7-bit解码
							if ((strIndex.Find("050003") != -1)||(strIndex.Find("060804") != -1))
							{	//长短信
								L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
								smsa.bIsLongSMS = TRUE;
							}
							else
							{
								j = 0;
								j = nCString2Bytes(pSrc+nPos, buf, uCharTmp*2);
								nDecode7bit(buf, pDst->TP_UD, uCharTmp*2);	// 转换到TP-DU  nDstLengthnDstLength
								nDstLength = uCharTmp;
								*(pDst->TP_UD+j) = '\0';
							}
						} 
						else
						{	//长短信
							L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
							smsa.bIsLongSMS = TRUE;
						}
					}
					else if(pDst->TP_DCS == CODE_Unicode)
					{	// Unicode解码   //要特别注意little endian和big endian的区别，两个正好相反
						j = 0;
						j = nCString2Bytes(pSrc+nPos, buf, uCharTmp*2);
						if (j<140)
						{
							if ((strIndex.Find("050003") != -1)||(strIndex.Find("060804") != -1))
							{	//长短信
								L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
								smsa.bIsLongSMS = TRUE;
							}
							else
							{
								for (int i=0;i<len*2-1;i+=4) //ok done
								{
									nCString2Bytes(pSrc+nPos+i,buf+(i/2),2);
									nCString2Bytes(pSrc+nPos+i+2,buf+(i+2)/2,2);
							//		uCharTemp = *(buf+(i/2));
							//		*(buf+(i/2)) = *(buf+(i+2)/2);
							//		*(buf+(i+2)/2) = uCharTemp;   //翻转
								}
								 nDstLength = UnicodeToGB2312(buf,pDst->TP_UD,len);//buf
							}
						} 
						else 
						{	//长短信
							L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
							smsa.bIsLongSMS = TRUE;
						}
					}
					else 
					{	// 8-bit解码
						j = nCString2Bytes(pSrc+nPos, buf, uCharTmp*2);
						if (j<140)
						{
							if ((strIndex.Find("050003") != -1)||(strIndex.Find("060804") != -1))
							{	//长短信
								L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
								smsa.bIsLongSMS = TRUE;
							}
							else
							{
								nDstLength = nCString2Bytes(pSrc+nPos, buf, uCharTmp * 2);			// 格式转换
								nDstLength = nDecode8bit(buf, pDst->TP_UD, len);	// 转换到TP-DU
							}
						} 
						else
						{	//长短信
							L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
							smsa.bIsLongSMS = TRUE;
						}
					}
			smsa.nToReturn=1;
	} 
	else if(nTxRxFlag==2||nTxRxFlag==3)//解析发送PDU
	{
				if ((*(pSrc) != '0') && (*(pSrc+1) != '0'))
				{
					nPos += 2;
				}
				else if ((*(pSrc) != '0') && (*(pSrc+1) != '8'))
				{
					nPos += 2;
					if ((*(pSrc+nPos) != '9') && (*(pSrc+nPos+1) != '1'))
					{
						AfxMessageBox(" SMSC现只能解析0x91国际型.");
						smsa.nToReturn=0;
					}
					nPos += 2;
				//目的号码 （收件人）
					RecoverPhoneNum(pSrc+nPos,pDst->SCA,nDstLength);//长度14
					nPos += nDstLength;
				}
					//1100
					nPos += 4;//不用做什么
				//0D  
					nCString2Bytes(pSrc+nPos,&uCharTmp,2);
					int len = uCharTmp;
					if ((*(pSrc+nPos) == '0') && (*(pSrc+nPos+1) == 'D'))
					{
						nPos += 2;
						//91
						if ((*(pSrc+nPos) != '9') && (*(pSrc+nPos+1) != '1'))
						{
							AfxMessageBox("TPA现只能解析0x91国际型.");
							smsa.nToReturn=0;
						}
						else 
						{
							nPos += 2; 
							//来源号码
								RecoverPhoneNum(pSrc+nPos,pDst->TPA,len+1);//14(F)
								nPos += (len+1);
						}
					}
					else if ((*(pSrc+nPos) == '0') && (*(pSrc+nPos+1) == 'B'))
					{
						nPos += 2;
							if ( ((*(pSrc+nPos) == '8') && (*(pSrc+nPos+1) == '1')) || ((*(pSrc+nPos) == 'A') && (*(pSrc+nPos+1) == '1')) )
							{
								nPos += 2; 
								//来源号码
									RecoverPhoneNum(pSrc+nPos,pDst->TPA,len+1);//12(F)
									nPos += (len+1);
							}
							else
							{
								nPos += (len+1);
								AfxMessageBox("号码类型出错，不是91,81,A1型.");
								smsa.nToReturn=0;	
							}
					}
					else
					{
						nPos += 2;
						AfxMessageBox(" PDU TPA error. Length error.");
						smsa.nToReturn=0;
					}
					nCString2Bytes(pSrc+nPos,(unsigned char*)&pDst->TP_PID,2);
					smsa.SMSInfo.TP_PID = pDst->TP_PID;
					nPos += 2;
				//08表示Unicode TP-DCS
					nCString2Bytes(pSrc+nPos,(unsigned char*)&pDst->TP_DCS,2);
					smsa.SMSInfo.TP_DCS = pDst->TP_DCS;

					if ((*(pSrc+nPos) == '0') && (*(pSrc+nPos+1) == '8'))//	AfxMessageBox("编码方式是Unicode 16位编码");
					{					
					} 
					else if ((*(pSrc+nPos) == '0') && (*(pSrc+nPos+1) == '0'))//	AfxMessageBox("编码方式是 7位编码");
					{					
					} 
					else
					{
						AfxMessageBox("编码方式不是 7位编码或者Unicode 16位编码");
					}
					nPos += 2;
				//有效期：
					nPos += 2;
				// 用户信息长度(TP-UDL)
					int j = nCString2Bytes(pSrc+nPos,&uCharTmp,2);
					len = uCharTmp;
					nPos += 2;
					CString strIndex;
					for(int index=0;index<=5;index++)
					{
						strIndex += *(pSrc+nPos+index); 
					}
					smsa.bIsLongSMS = FALSE;
					pDst->index = 0;
				//信息内容解码
					if(pDst->TP_DCS == CODE_7BIT)	
					{
						if (j<140)
						{	// 7-bit解码
							if ((strIndex.Find("050003") != -1)||(strIndex.Find("060804") != -1))
							{	//长短信
								L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
								smsa.bIsLongSMS = TRUE;
							}
							else
							{
							j = 0;
							j = nCString2Bytes(pSrc+nPos, buf, uCharTmp*2);
							nDecode7bit(buf, pDst->TP_UD, uCharTmp*2);	// 转换到TP-DU  nDstLengthnDstLength
							nDstLength = uCharTmp;
							*(pDst->TP_UD+j) = '\0';
							}
						} 
						else
						{	//长短信
							L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
							smsa.bIsLongSMS = TRUE;
						}
					}
					else if(pDst->TP_DCS == CODE_Unicode)
					{	// Unicode解码   //要特别注意little endian和big endian的区别，两个正好相反
						j = 0;
						j = nCString2Bytes(pSrc+nPos, buf, uCharTmp*2);
						if (j<140)
						{
								if ((strIndex.Find("050003") != -1)||(strIndex.Find("060804") != -1))
								{	//长短信
									L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
									smsa.bIsLongSMS = TRUE;
								}	
								else
								{
									for (int i=0;i<len*2 - 1;i+=4) //ok done
									{
										nCString2Bytes(pSrc+nPos+i,buf+(i/2),2);
										nCString2Bytes(pSrc+nPos+i+2,buf+(i+2)/2,2);
								//		uCharTemp = *(buf+(i/2));
								//		*(buf+(i/2)) = *(buf+(i+2)/2);
								//		*(buf+(i+2)/2) = uCharTemp;   //翻转
									}
									 nDstLength = UnicodeToGB2312(buf,pDst->TP_UD,len);//buf
								}
						} 
						else
						{	//长短信
							L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
							smsa.bIsLongSMS = TRUE;
						}
					}
					else
					{	// 8-bit解码
						j = nCString2Bytes(pSrc+nPos, buf, uCharTmp*2);
						if (j<140)
						{
							if ((strIndex.Find("050003") != -1)||(strIndex.Find("060804") != -1))
							{	//长短信
								L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
								smsa.bIsLongSMS = TRUE;
							}
							else
							{								
								nDstLength = nCString2Bytes(pSrc+nPos, buf, uCharTmp * 2);			// 格式转换
								nDstLength = nDecode8bit(buf, pDst->TP_UD, len);	// 转换到TP-DU
							}
						} 
						else
						{	//长短信
							L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
							smsa.bIsLongSMS = TRUE;
						}
					}
			smsa.nToReturn=1;
	}
	smsa.nSMSTotalNum = L.nAllNum;
	smsa.strLongSMSID = L.strID; 
	return smsa;
}
//长短信的内容解析，PDU中信息长度后面内容的解析
//1表示解码成功，0表示解码出错
LSMSRETURN LongSMSTextDeCode(char *pSrc, SM_PARAM *pDst,int ndecodeMode,int nLength)
{
	LSMSRETURN lToReturn;
	lToReturn.ntoreturn = 0;
	lToReturn.strID = "";
	CString strSMSText = "";
	CString strSMSTotalNum = "";
	int nPos = 0;
	int nlen = 0;
	int i = 0;
	int nDstLength=0; //用于解析PDU时，（表示其中某些字符量存的）长度
	unsigned char uCharTmp;		// 内部用的临时字节变量
	unsigned char buf[256];	// 内部用的缓冲区
	nCString2Bytes(pSrc+nPos,&uCharTmp,2);
	nlen = uCharTmp;
	if (nlen==5) //05
	{
		nPos += 2;
		if ((*(pSrc+nPos) != '0') && (*(pSrc+nPos+1) != '0'))  //00  超长短信的标识位长度为1
		{
			AfxMessageBox("长短信0500出错。");
			i = 0;
			lToReturn.ntoreturn = i;
			return lToReturn;
		}
		nPos += 2;
		if ((*(pSrc+nPos) != '0') && (*(pSrc+nPos+1) != '3'))  //03  剩下短信标识的长度 
		{
			AfxMessageBox("长短信050003出错。");
			i = 0;
			lToReturn.ntoreturn = i;
			return lToReturn;
		}
		nPos += 2;
		// XX短信标志 
//		strSMSText = "ID:";
//		strSMSText += *(pSrc+nPos);
//		strSMSText += *(pSrc+nPos+1);
//		strSMSText += ". ";
		lToReturn.strID += *(pSrc+nPos);
		lToReturn.strID += *(pSrc+nPos+1);
		nPos += 2;
		//MM短信数量一般最多为5 
		strSMSTotalNum += *(pSrc+nPos);
		strSMSTotalNum += *(pSrc+nPos+1);
		lToReturn.nAllNum = atoi(strSMSTotalNum);
		nPos += 2;
		//NN第几条
//		strSMSText += "第";
//		strSMSText += *(pSrc+nPos) + *(pSrc+nPos+1);
		char aTemp = *(pSrc+nPos+1);
		pDst->index = atoi(&aTemp);
//		strSMSText += "条: ";
		nPos += 2;		
	} 
	else if(nlen==6)
	{
		nPos += 2;
		if ((*(pSrc+nPos) != '0') && (*(pSrc+nPos+1) != '8'))  //08 超长短信的标识位长度为2
		{
			AfxMessageBox("长短信0608出错。");
			i = 0;
			lToReturn.ntoreturn = i;
			return lToReturn;
		}
		nPos += 2;
		if ((*(pSrc+nPos) != '0') && (*(pSrc+nPos+1) != '4'))  //04  剩下短信标识的长度 
		{
			AfxMessageBox("长短信060804出错。");
			i = 0;
			lToReturn.ntoreturn = i;
			return lToReturn;
		}
		nPos += 2;
		// XX XX短信标志 
//		strSMSText = "ID:";
//		strSMSText += *(pSrc+nPos);
//		strSMSText += *(pSrc+nPos+1);
//		strSMSText += " ";
//		strSMSText += *(pSrc+nPos+2);
//		strSMSText += *(pSrc+nPos+3);
//		strSMSText += ". ";
		lToReturn.strID += *(pSrc+nPos);
		lToReturn.strID += *(pSrc+nPos+1);
		lToReturn.strID += *(pSrc+nPos+2);
		lToReturn.strID += *(pSrc+nPos+3);
		nPos += 4;
		//MM短信数量一般最多为5 
		strSMSTotalNum += *(pSrc+nPos);
		strSMSTotalNum += *(pSrc+nPos+1);
		lToReturn.nAllNum = atoi(strSMSTotalNum);
		nPos += 2;
		//NN第几条
//		strSMSText += "第";
		strSMSText += *(pSrc+nPos);
		strSMSText += *(pSrc+nPos+1);
		char aTemp = *(pSrc+nPos+1);
		pDst->index = atoi(&aTemp);
//		strSMSText += "条: ";
		nPos += 2;
	}
	else
	{
		AfxMessageBox("长短信协议长度不是05或者08。");
		i = 0;
		lToReturn.ntoreturn = i;
		return lToReturn;
	}
			strCount num;
			num	= Statistic(strSMSText);
			int length = num.nChineseLenth+num.nEnglishLenth*2;
			int len = nLength - (nlen+1);
			int j = nCString2Bytes(pSrc+nPos, buf, len*2);
				if(ndecodeMode == CODE_7BIT)	
				{
					// 7-bit解码
					nDecode7bit(buf, pDst->TP_UD, len*2);	// 转换到TP-DU  nDstLengthnDstLength
					nDstLength = uCharTmp;
					*(pDst->TP_UD+j) = '\0';
				}
				else if(ndecodeMode == CODE_Unicode)
				{
					// Unicode解码   //要特别注意little endian和big endian的区别，两个正好相反
					for (int i=0;i<len*2 - 1;i+=4) //ok done
					{
						nCString2Bytes(pSrc+nPos+i,buf+(i/2),2);
						nCString2Bytes(pSrc+nPos+i+2,buf+(i+2)/2,2);
				//		uCharTemp = *(buf+(i/2));
				//		*(buf+(i/2)) = *(buf+(i+2)/2);
				//		*(buf+(i+2)/2) = uCharTemp;   //翻转
					}
					 nDstLength = UnicodeToGB2312(buf,pDst->TP_UD,len);//buf
					 *(pDst->TP_UD+j) = '\0';
				}
				else  // 8-bit解码
				{					
					nDstLength = nCString2Bytes(pSrc+nPos, buf, uCharTmp * 2);			// 格式转换
					nDstLength = nDecode8bit(buf, pDst->TP_UD, len);	// 转换到TP-DU
					*(pDst->TP_UD+j) = '\0';
				}				
				strSMSText += pDst->TP_UD;
		//		pDst->TP_UD = (LPSTR)(LPCTSTR)strSMSText;
				memcpy(pDst->TP_UD,strSMSText,length+j); 
				*(pDst->TP_UD+j+length) = '\0';
			i=1;
	//		return i;
	  		lToReturn.ntoreturn = i;
			return lToReturn;
			
}

strCount Statistic(CString str) 
{    //获得字符串按字节数计算的长度
	strCount count;
	int Lenth = str.GetLength();
	count.nEnglishLenth =0;
	count.nTotalLenth = 0;
	count.nChineseLenth = 0;
    for(int i=0;i<Lenth;i++)
    {
        char c = str.GetAt(i);
        //是中文字符 中文字符编码 1XXX XXXX 1XXX XXXX
        if(c<0||c>255)
            continue;
        //是英文字符
        else 
            count.nEnglishLenth ++;
    }
    //计算中文字符数，每个中文字符占两个字节
    count.nChineseLenth = (Lenth-count.nEnglishLenth)/2;
    count.nTotalLenth = count.nChineseLenth + count.nEnglishLenth;
	return count;
}
//将返回的CMS ERROR错误代号转化为相应的中文数据
void CMSError(CString str)
{
	int _int;
	_int = atoi(str);
	switch(_int)
	{
		case 300:
			AfxMessageBox("移动设备错误");
			break;
		case 301:
			AfxMessageBox("短信服务器错误");
			break;
		case 302:
			AfxMessageBox("操作不允许");
			break;
		case 303:
			AfxMessageBox("操作不支持");
			break;
		case 304:
			AfxMessageBox("无效的PDU模式参数");
			break;
		case 305:
			AfxMessageBox("无效的TEXT模式参数");
			break;
		case 310:
			AfxMessageBox("SIM卡未插入");
			break;
		case 311:
			AfxMessageBox("PIN码请求错误");
			break;
		case 312:
			AfxMessageBox("PH-SIM PIN 错误");
			break;
		case 313:
			AfxMessageBox("SIM 卡失败");
			break;
		case 314:
			AfxMessageBox("SIM 卡忙");
			break;
		case 315:
			AfxMessageBox("SIM 卡错误");
			break;
		case 316:
			AfxMessageBox("SIM PUK 请求错误");
			break;
		case 317:
			AfxMessageBox("SIM PIN2 请求错误");
			break;
		case 318:
			AfxMessageBox("SIM PUK2 请求错误");
			break;
		case 320:
			AfxMessageBox("存储失败");
			break;
		case 321:
			AfxMessageBox("无效的存储索引");
			break;
		case 322:
			AfxMessageBox("内存满");
			break;
		case 330:
			AfxMessageBox("短信中心地址未知");
			break;
		case 331:
			AfxMessageBox("无网络服务");
			break;
		case 332:
			AfxMessageBox("网络超时");
			break;
		case 340:
			AfxMessageBox("+CNMA ACK 错误");
			break;
		case 342:
			AfxMessageBox("短信长度错误");
			break;
		case 500:
			AfxMessageBox("500: 未知错误");
			break;
		default:
			AfxMessageBox("未知错误");
			break;
	}
}
//长短信编码，比CEncodePDU多了个nOrder，表示第几条，其他均一致。
funReturn cEncodeLongPDU(CString strDstNum, CString strSMSText,char * chForPDU,BOOL bChineseFlag,int nLenth,CString Order,CString TotalOrder,int nID)
{
	funReturn c;
	c.bToReturn =TRUE;
	c.nLenthToReturn =0;

	char *cDstNum;
	cDstNum = strDstNum.GetBuffer(strDstNum.GetLength()); 
	strDstNum.ReleaseBuffer(); //dst number
	
	char chText[256];
	memset(chText,0,256);
	memcpy(chText,strSMSText,strSMSText.GetLength());
	int len = strSMSText.GetLength();
//	int	len = strSMSText.GetLength() +2;
//	chText[len-2] = '\r';
//	chText[len-1] = '\n';
	//-----------------------------
	int nTPDUlen = 0;
	int nPos = 0;
//   51 基本参数(TP-MTI/VFP) 不要求发送回复 或者 31  01 11
	chForPDU[nPos] = '5';
	chForPDU[nPos+1] = '1';
	nPos += 2;
	nTPDUlen += 2;
//00 消息基准值(TP-MR)
	chForPDU[nPos] = '0';
	chForPDU[nPos+1] = '0';
	nPos += 2;
	nTPDUlen += 2;
//-----------------------------收件人
//长度0D
	chForPDU[nPos] = '0';
	chForPDU[nPos+1] = 'D';
	nPos += 2;
	nTPDUlen += 2;
//91
	chForPDU[nPos] = '9';
	chForPDU[nPos+1] = '1';
	nPos += 2;
	nTPDUlen += 2;
//号码
	c = ReversePhoneNum(cDstNum,&chForPDU[nPos],strDstNum.GetLength());
	if(c.bToReturn==FALSE)
	{
		return c;
	}
	nPos += 14;
	nTPDUlen += 14;
//00 TP-PID
	chForPDU[nPos] = '0';
	chForPDU[nPos+1] = '0';
	nPos += 2;
	nTPDUlen += 2;
//08 TP-DCS  Unicode编码           这里还要加个7bit编码，（8bit待定）
	if (bChineseFlag)
	{
		chForPDU[nPos] = '0';
		chForPDU[nPos+1] = '8';
		nPos += 2;	
		nTPDUlen += 2;
	} 
	else //	这里还要加个7bit编码，
	{
		chForPDU[nPos] = '0';
		chForPDU[nPos+1] = '0';
		nPos += 2;	
		nTPDUlen += 2;
	}
//00  可变   A7 AA
	chForPDU[nPos] = 'A';
	chForPDU[nPos+1] = 'A';
	nPos += 2;
	nTPDUlen += 2;
	if (bChineseFlag)
	{
		CString strTmp = "";
	//--------------------------------------------
	//TP-UDL   采用050003XXXXXX 
	char a = nLenth*2+6;
	nBytes2CString((unsigned char *)&a,&chForPDU[nPos],2);//TP-UDL
	nPos += 2;
	nTPDUlen += 2;
	//TP-UD开始  长短信标志
	chForPDU[nPos] = '0';
	chForPDU[nPos+1] = '5';
	chForPDU[nPos+2] = '0';
	chForPDU[nPos+3] = '0';
	chForPDU[nPos+4] = '0';
	chForPDU[nPos+5] = '3';
	nPos += 6;
	nTPDUlen += 6;
//	chForPDU[nPos] = '8';
	strTmp.Format("%d",nID);
//	chForPDU[nPos+1] = strTmp;
	strncpy(&chForPDU[nPos],(LPCTSTR)strTmp,2);
	nPos += 2;
	nTPDUlen += 2;
	//总条数
	chForPDU[nPos] = '0';
	strncpy(&chForPDU[nPos+1],(LPCTSTR)TotalOrder,1);
	nPos += 2;
	nTPDUlen += 2;
	//第几条
	chForPDU[nPos] = '0';
//	chForPDU[nPos+1] = Order.Mid(0,1);
	strncpy(&chForPDU[nPos+1],(LPCTSTR)Order,1);
	nPos += 2;
	nTPDUlen += 2;

//真正的TP-UD数据
		unsigned char buf[256];
		int i = nEncodeUnicode(chText,buf,strSMSText.GetLength());
//		int i = nEncodeUnicode(chText,buf,nLenth*2);
		int j=nBytes2CString(buf,&chForPDU[nPos],i*2);
		nTPDUlen += i*2;
//		PDUlength = nPos + i*2; //PDU 总长度
		chForPDU[nPos + i*2] = 0x1a;//     ctrl+z   在函数cEncodePDU中实现
		chForPDU[nPos + i*2+1] = '\0';
	} 
	else
	{
//TP-UDL
	char a = nLenth;
	nBytes2CString((unsigned char *)&a,&chForPDU[nPos],2);//TP-UDL
	nPos += 2;
	nTPDUlen += 2;
//TP-UD
		//7bit编码
		unsigned char buf[256];
	//	int i = nEncode7bit(chText,buf,strSMSText.GetLength());
		int i = nEncode7bit(chText,buf,nLenth*2);
		int j=nBytes2CString(buf,&chForPDU[nPos],i*2);
		nTPDUlen += i*2;
//		PDUlength = nPos + i*2; //PDU 总长度
		chForPDU[nPos + i*2] = 0x1a;//     ctrl+z  在函数cEncodePDU中实现
		chForPDU[nPos + i*2+1] = '\0';
	}
	//---------------------返回PDU
	c.nLenthToReturn = nTPDUlen/2;
	return c;
}