#include "stdafx.h"
#include <wchar.h>
#include <stdio.h>
#include "3G.h"

//��֤��������
//+8615179141171 ����14 ������1
//8615179141171 ����13������2
//15179141171 ����11������3
//0��ʾ�����糤��12�����߲����ֻ�����
int checkPhoneNum(const char *pSrc, int nSrcLength)
{	
	int i;
	if (nSrcLength == 14)
	{
		i = 1;//Ҫɾ��+
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
// �����ߵ����ַ���ת��Ϊ����˳����ַ���
// �磺"683158812764F8" --> "8613851872468"
// ����: pSrc - Դ�ַ���ָ��
//       nSrcLength - Դ�ַ�������
// ���: pDst - Ŀ���ַ���ָ��
// ����: Ŀ���ַ�������
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
				*(pDst+nDstLength-1) = '\0';//��ȥ'F'	
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
				*(pDst+nDstLength-1) = '\0';//��ȥ'F'	
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
				*(pDst+nDstLength-1) = '\0';//��ȥ'F'	
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
				*(pDst+nDstLength-1) = '\0';//��ȥ'F'	
			}
	}
	*(pDst+nDstLength) = '\0';//��ȥ'F'
	return nDstLength;
}
// ����˳����ַ���ת��Ϊ�����ߵ����ַ�����������Ϊ��������'F'�ճ�ż��
// �磺"8613851872468" --> "683158812764F8"
// ����: pSrc - Դ�ַ���ָ��
//       nSrcLength - Դ�ַ�������
// ���: pDst - Ŀ���ַ���ָ��
// ����: Ŀ���ַ�������
funReturn ReversePhoneNum(char *pSrc, char *pDst, int nSrcLength)
{
	funReturn a;
	a.bToReturn = TRUE;
	a.nLenthToReturn =0;

	int nDstLength,nNumStatus,i;		// Ŀ���ַ������� ����״̬
	char chTemp;			// ���ڱ���һ���ַ�
	nDstLength = nSrcLength;	// ���ƴ�����
	nNumStatus = checkPhoneNum(pSrc,nSrcLength);
	switch (nNumStatus) //����14:0~13
	{
	case 1://ɾ����ǰ��+��������F ,����14
		for (i=0;i<=nDstLength-2;i++) //0~12
		{
			*(pDst+i) = *(pSrc+i+1);
		}
		*(pDst+nDstLength-1)='F'; //13
		break;
	case 2://������F ,����13
		for (i=0;i<=nDstLength-1;i++) //0~12
		{
			*(pDst+i) = *(pSrc+i);
		}
		nDstLength++;//����14
		*(pDst+nDstLength-1)='F'; //13
		break;
	case 3://ǰ���86��������F,����11
		*pDst ='8';//0
		*(pDst+1) ='6';//1
		for (i=0;i<=nDstLength-1;i++) //(0~10) 2~12
		{
			*(pDst+i+2) = *(pSrc+i);
		}
		nDstLength += 3;//����14
		*(pDst+nDstLength-1)='F'; //13
		break;
	default:
		a.bToReturn = FALSE;
		AfxMessageBox("�Է����볤���������ʵ�����ԣ�");
		break;
	}
	
	for(i=0;i<=nDstLength-1;i+=2)//0~13
	{
		chTemp = *(pDst+i);		// �����ȳ��ֵ��ַ�
		*(pDst+i) = *(pDst+i+1);	// ���ƺ���ֵ��ַ�
		*(pDst+i+1) = chTemp;		// �����ȳ��ֵ��ַ�
	}
	*(pDst+nDstLength) = '\0';//14,�ַ�����
	a.nLenthToReturn = nDstLength;
	return a;
//	return nDstLength;
}
//����ʱ��
//ע�����Ҫ��pDst�����㹻���Ŀռ䣻����ᳬ��
// 01 40 82 11 54 44 23(14��)---->2010/04/28 11:45:44 (19+'\0',20��) �ټ���Time Zone: (32���ַ�)
//������5���ַ��������ո�
//---------------�ָ�ʱ��
//01408211544423->2010/04/28 11:45:44 
int RecoverTime(char *pSrc, char *pDst, int nSrcLength)
{ //�����ȸ����£���ת���ټ�
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
			chTemp = *(pDst+i+2);//pDst,23ת��
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
			chTemp = *(pDst+i+3);//pDst,56ת��
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
			chTemp = *(pDst+i+4);//pDst,89ת��
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
			chTemp = *(pDst+i+5);//pDst,1112ת��
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
			chTemp = *(pDst+i+6);//pDst,1415ת��
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
			chTemp = *(pDst+i+7);//pDst,1718ת��
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
			chTemp = *(pDst+i+18);//pDst,3031ת��
			*(pDst+i+18) = *(pDst+i+17);
			*(pDst+i+17) = chTemp;
		}
	}
	*(pDst+nSrcLength+18) = '\0';
	return nSrcLength+18;
}
// �ֽ�����ת��Ϊ�ɴ�ӡ�ַ���
// �磺{0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} --> "C8329BFD0E01" 
// ����: pSrc - Դ����ָ��
//       nSrcLength - Դ���ݳ���
// ���: pDst - Ŀ���ַ���ָ��
// ����: Ŀ���ַ�������
int nBytes2CString(const unsigned char* pSrc, char* pDst, int nSrcLength)
{
	char tab[] = "0123456789ABCDEF"; //0x0~0xF 
	for (int i=0;i<nSrcLength;i+=2)
	{
		*pDst = tab[*pSrc >> 4]; //����4λ()
		pDst++;
		*pDst++ = tab[*pSrc & 0x0f]; // �����4λ
	//	pDst++;
		pSrc++;
	}
		// ����ַ����Ӹ�������
	*pDst = '\0';
	return nSrcLength*2;	
}
// �ɴ�ӡ�ַ���ת��Ϊ�ֽ�����
// �磺"C8329BFD0E01" --> {0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01}
// ����: pSrc - Դ�ַ���ָ��
//       nSrcLength - Դ�ַ�������
// ���: pDst - Ŀ������ָ��
// ����: Ŀ�����ݳ��� (ԭ�����ȵ�һ��)
int nCString2Bytes(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
	for (int i=0;i<nSrcLength;i+=2)
	{
		//�����4λ
		if ((*pSrc >='0') && (*pSrc <= '9'))
		{
			*pDst = (*pSrc - '0') << 4;//��ֵ����4λ
		} 
		else
		{
			*pDst = (*pSrc - 'A'+ 10) << 4;//��ֵ����4λ
		}
		pSrc++;
		// �����4λ  
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
// 7bit����
// ����: pSrc - Դ���봮ָ��
//       nSrcLength - Դ���봮����
// ���: pDst - Ŀ���ַ���ָ��
// ����: Ŀ���ַ�������
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
					*(pDst) = (*pSrc & n); //ȡ���һλ
					*(pDst) = (*(pDst) << i); //7-iλ���Լ�λ��
					*(pDst) |= (*(pSrc-1) >> (8-i));
					pDst++;//7�ֽڱ�8�ֽ�;
					n = 0xFE;			
					*(pDst) = (*pSrc & n);
					*(pDst) = (*(pDst) >> 1);//��8�ֽ�
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
						*(pDst) = (*pSrc & n); //ȡ��7λ
						n = n / 2; //����2						
					}
					if (i>0)
					{
						*(pDst) = (*pSrc & n); //ȡ��7-iλ
						n = n / 2;  //����2
						*(pDst) = (*(pDst) << i); //7-iλ���Լ�λ��
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
// 8bit����
// ����: pSrc - Դ���봮ָ��
//       nSrcLength -  Դ���봮����
// ���: pDst -  Ŀ���ַ���ָ��
// ����: Ŀ���ַ�������
int nDecode8bit(const unsigned char* pSrc, char* pDst, int nSrcLength)
{	
	memcpy(pDst, pSrc, nSrcLength);	// �򵥸���
	*pDst = '\0';// ����ַ����Ӹ�������
	return nSrcLength;
}
//-----------------------
//7λ���룬��Ҫ����Ӣ�ĸ�ʽ���Լ���TEXT��ʽ��
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
					if (*pSrc == 0x0d)  //���ܻ������
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
// ���: pDst - Ŀ����봮ָ��
// ����: Ŀ����봮����
int nEncodeUnicode(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
	 int nDstLength;        // UNICODE���ַ���Ŀ
     WCHAR wchar[128];  // UNICODE�������� 
     // �ַ���-->UNICODE��
     nDstLength = MultiByteToWideChar(CP_ACP, 0, pSrc, nSrcLength, wchar, 128); 
     // �ߵ��ֽڶԵ������
     for(int i=0; i<nDstLength; i++)
     {
         *pDst++ = wchar[i] >> 8;         // �������λ�ֽ�
         *pDst++ = wchar[i] & 0xff;       // �������λ�ֽ�
     }
     // ����Ŀ����봮����
     return nDstLength * 2;
}

int UnicodeToGB2312(const unsigned char *pSrc,char *pDst,int nLength)
{
     int nDstLength;        // UNICODE���ַ���Ŀ
     WCHAR wchar[256];  // UNICODE��������
	 wmemset(wchar,0,sizeof(wchar)/sizeof(wchar_t));
 
     // �ߵ��ֽڶԵ���ƴ��UNICODE
     for(int i=0; i<nLength/2; i++)
     {
           wchar[i] = *pSrc++ << 8;    // �ȸ�λ�ֽ�
         wchar[i] |= *pSrc++;        // ���λ�ֽ�
     } 
     // UNICODE��-->�ַ��� //CP_ACP
     nDstLength = WideCharToMultiByte(936, 0, wchar, nLength/2, pDst, 160, NULL, NULL);
     // ����ַ����Ӹ�������
     pDst[nDstLength] = '\0';
     // ����Ŀ���ַ�������
     return nDstLength;
}
//������������PDU����
//ȱ��strSrcNum �����˺���  ��Ϊ��������  00��ʾ���ֻ������ö������ĺ���
//strDstNum �ռ��˺���
//strSMSText ��������
//chForPDU PDU�洢ָ��
//����TPDU�ĳ���
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
	//-----------------------��������
//00  00��ʾ���ֻ������ö������ĺ���
	chForPDU[nPos] = '0';
	chForPDU[nPos+1] = '0';
	nPos += 2;//���Ȳ���
//11   51 ��������(TP-MTI/VFP) ��Ҫ���ͻظ� ���� 31  01 11
	chForPDU[nPos] = '1';
	chForPDU[nPos+1] = '1';
	nPos += 2;
	nTPDUlen += 2;
//00 ��Ϣ��׼ֵ(TP-MR)
	chForPDU[nPos] = '0';
	chForPDU[nPos+1] = '0';
	nPos += 2;
	nTPDUlen += 2;
//-----------------------------�ռ���
//����0D
	chForPDU[nPos] = '0';
	chForPDU[nPos+1] = 'D';
	nPos += 2;
	nTPDUlen += 2;
//91
	chForPDU[nPos] = '9';
	chForPDU[nPos+1] = '1';
	nPos += 2;
	nTPDUlen += 2;
//����
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
//08 TP-DCS  Unicode����           ���ﻹҪ�Ӹ�7bit���룬��8bit������
	if (bChineseFlag)
	{
		chForPDU[nPos] = '0';
		chForPDU[nPos+1] = '8';
		nPos += 2;	
		nTPDUlen += 2;
	} 
	else //	���ﻹҪ�Ӹ�7bit���룬
	{
		chForPDU[nPos] = '0';
		chForPDU[nPos+1] = '0';
		nPos += 2;	
		nTPDUlen += 2;
	}
//00  �ɱ�   A7 AA
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
//		PDUlength = nPos + i*2; //PDU �ܳ���
		chForPDU[nPos + i*2] = 0x1a;//     ctrl+z   �ں���cEncodePDU��ʵ��
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
		//7bit����
		unsigned char buf[256];
		int i = nEncode7bit(chText,buf,strSMSText.GetLength());
		int j=nBytes2CString(buf,&chForPDU[nPos],i*2);
		nTPDUlen += i*2;
//		PDUlength = nPos + i*2; //PDU �ܳ���
		chForPDU[nPos + i*2] = 0x1a;//     ctrl+z  �ں���cEncodePDU��ʵ��
		chForPDU[nPos + i*2+1] = '\0';
	}
	//---------------------����PDU
	b.nLenthToReturn = nTPDUlen/2;
	return b;
//	return nTPDUlen/2;
}
//������PDU����
//strSrcNum �����˺���  strDstNum �ռ��˺���  strSMSText ��������    chForPDU PDU�洢ָ��  bChineseFlag �Ƿ��������ַ���TRUEΪ�У���Unicode���룻FALSE��7bit����
//����TPDU�ĳ���
funReturn  cEncodePDU(CString strSrcNum, CString strDstNum, CString strSMSText,char * chForPDU,BOOL bChineseFlag,int nLenth)
{
	funReturn c;
	c.bToReturn =  TRUE;
	c.nLenthToReturn = 0;
	//����CString���ڲ�Ҫתchar (���ַ�ʽ)
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
	//-----------------------------������
	c = ReversePhoneNum(cSrcNum,&chForPDU[nPos],strSrcNum.GetLength());
	if(c.bToReturn==FALSE)
	{
		return c;
	}
	nPos += 14;
//����Ϊ1100
	chForPDU[nPos] = '1';
	chForPDU[nPos+1] = '1';
	chForPDU[nPos+2] = '0';
	chForPDU[nPos+3] = '0';
	nPos += 4;
	nTPDUlen += 4;
//-----------------------------�ռ���
//����0D
	chForPDU[nPos] = '0';
	chForPDU[nPos+1] = 'D';
	nPos += 2;
	nTPDUlen += 2;
//91
	chForPDU[nPos] = '9';
	chForPDU[nPos+1] = '1';
	nPos += 2;
	nTPDUlen += 2;
//����
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
//08 TP-DCS  Unicode����  ��8bit������
	if (bChineseFlag)
	{
		chForPDU[nPos] = '0';
		chForPDU[nPos+1] = '8';
		nPos += 2;	
		nTPDUlen += 2;
	} 
	else //	���ﻹҪ�Ӹ�7bit���룬
	{
		chForPDU[nPos] = '0';
		chForPDU[nPos+1] = '0';
		nPos += 2;	
		nTPDUlen += 2;
	}
//00  �ɱ�   A7 AA
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
//		PDUlength = nPos + i*2; //PDU �ܳ���
		chForPDU[nPos + i*2] = 0x1a;//     ctrl+z   �ں���cEncodePDU��ʵ��
		chForPDU[nPos + i*2+1] = '\0';
//���ﲹ�ϳ���
	} 
	else
	{
//TP-UDL
	char a = nLenth;
	nBytes2CString((unsigned char *)&a,&chForPDU[nPos],2);//TP-UDL
	nPos += 2;
	nTPDUlen += 2;
//TP-UD
		//7bit����
		unsigned char buf[256];
		int i = nEncode7bit(chText,buf,strSMSText.GetLength());
		int j=nBytes2CString(buf,&chForPDU[nPos],i*2);
		nTPDUlen += i*2;
//		PDUlength = nPos + i*2; //PDU �ܳ���
		chForPDU[nPos + i*2] = 0x1a;//     ctrl+z  �ں���cEncodePDU��ʵ��
		chForPDU[nPos + i*2+1] = '\0';
	}
	//---------------------����PDU
	c.nLenthToReturn = nTPDUlen/2;
	return c;
//	return nTPDUlen/2;
}
/*------�������ܶ���
�������ݣ� 08 91683108200105F0 04 0D 91683184821969F2 000470404271726423026869 
���                       ����                           ˵�� 
08                         SMSC��ַ��Ϣ�ĳ���             ��8����λ�ֽ�(����91) 
91                         SMSC��ַ��ʽ(TON/NPI)          �ù��ʸ�ʽ����(��ǰ��ӡ�+��) 
683108200105F0             SMSC��ַ                       8613800210500������F���ճ�ż���� 
04                         ��������(TP-MTI/MMS/RP) 
0D                         �ظ���ַ���ָ��� 
91                         �ظ���ַ��ʽ(TON/NPI) 
683184821969F2             �ظ���ַ(TP-RA)                8613482891962������F���ճ�ż���� 
00                         Э���ʶ(TP-PID)               ����ͨGSM���ͣ��㵽�㷽ʽ 
04                         �û���Ϣ���뷽ʽ(TP-DCS)       8-bit���� 
70404271726423             ʱ���(TP-SCTS)                07-04-24 17:27:46  23��ʾʱ���� 
02                         �û���Ϣ����(TP-UDL)           ʵ�ʳ���2���ֽ� 
6869                       �û���Ϣ(TP-UD)                0x68 0x69 
// ����ֵ�� 0 ��ʾ�������ɹ���1,��ʾ�����ɹ�   //Ӧ�����жϣ�nPos�����ƶ�
//nTxRxFlag���ܻ��Ƿ��͵ı�־��0��1��ʾ���ܣ�2��3��ʾ����
*/
SMSInfoALL nDecodePdu(char *pSrc, SMSInfoALL smsa,int nTxRxFlag)
{
	smsa.nToReturn = 0;
	SM_PARAM *pDst = &smsa.SMSInfo;
	int nDstLength=0; //���ڽ���PDUʱ������ʾ����ĳЩ�ַ�����ģ�����
	unsigned char uCharTmp;		// �ڲ��õ���ʱ�ֽڱ���
	unsigned char buf[256];	// �ڲ��õĻ�����
	int nPos=0;//��ʾ�ַ�λ��(ָ��λ��)
	LSMSRETURN L;
	if (nTxRxFlag==0||nTxRxFlag==1)//��������PDU
	{	//���Ų�ѯ
				//08 SMSC��ʼ
					nCString2Bytes(pSrc,&uCharTmp,2);
					nDstLength = (uCharTmp - 1) * 2; //����14
					if (nDstLength != 14)
					{
						AfxMessageBox(" PDU SMSC error.");
						smsa.nToReturn=0;
					}
					nPos += 2;
				//91
					if ((*(pSrc+nPos) != '9') && (*(pSrc+nPos+1) != '1'))
					{
						AfxMessageBox(" SMSC��ֻ�ܽ���0x91������.");
						smsa.nToReturn=0;
					}
					nPos += 2;
				//Ŀ�ĺ��� ���ռ��ˣ�
					RecoverPhoneNum(pSrc+nPos,pDst->SCA,nDstLength);//����14
					nPos += nDstLength;				
				//PDU Type:04 First octet of this SMS-DELIVER message.��˼�ܶ���Բ��ܡ����� 24   ����TAP��ʼ
					nPos += 2;
					//������ʲô������
				//0D  0A 05    08 
					nCString2Bytes(pSrc+nPos,&uCharTmp,2);
					int len = uCharTmp;
					if(len%2==1)//0D 0A 05
					{
						nPos += 2;
						//91  A1
						nPos += 2; //������ʲô		
						//��Դ����
						RecoverPhoneNum(pSrc+nPos,pDst->TPA,len+1);//14(F)��
						nPos += (len+1);					
					}
					else if(len%2==0) //08
					{
						nPos += 2;
						//  A1
						nPos += 2; 	//������ʲô
						RecoverPhoneNum(pSrc+nPos,pDst->TPA,len);//8
						nPos += len;						
					}
				//00 TP-PID
					nCString2Bytes(pSrc+nPos,(unsigned char*)&pDst->TP_PID,2);
					smsa.SMSInfo.TP_PID = pDst->TP_PID;
					nPos += 2;
				//08��ʾUnicode TP-DCS
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
						AfxMessageBox("���뷽ʽ����7λ�������Unicode16λ����");
					}
					nPos += 2;
				//ʱ�� TP-SCTS  ��:01 40 82 11 54 44 23
					RecoverTime(pSrc+nPos,pDst->TP_SCTS,nDstLength);
					nPos += nDstLength;
				// �û���Ϣ����(TP-UDL)
					int j = nCString2Bytes(pSrc+nPos,&uCharTmp,2);
					len = uCharTmp;
					nPos += 2;
					CString strIndex;
					smsa.bIsLongSMS = FALSE;
					pDst->index = 0;
				//��Ϣ���ݽ���
					for(int index=0;index<=5;index++)
					{
						strIndex += *(pSrc+nPos+index); 
					}
					if(pDst->TP_DCS == CODE_7BIT)	
					{
						if (j<140)
						{	// 7-bit����
							if ((strIndex.Find("050003") != -1)||(strIndex.Find("060804") != -1))
							{	//������
								L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
								smsa.bIsLongSMS = TRUE;
							}
							else
							{
								j = 0;
								j = nCString2Bytes(pSrc+nPos, buf, uCharTmp*2);
								nDecode7bit(buf, pDst->TP_UD, uCharTmp*2);	// ת����TP-DU  nDstLengthnDstLength
								nDstLength = uCharTmp;
								*(pDst->TP_UD+j) = '\0';
							}
						} 
						else
						{	//������
							L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
							smsa.bIsLongSMS = TRUE;
						}
					}
					else if(pDst->TP_DCS == CODE_Unicode)
					{	// Unicode����   //Ҫ�ر�ע��little endian��big endian���������������෴
						j = 0;
						j = nCString2Bytes(pSrc+nPos, buf, uCharTmp*2);
						if (j<140)
						{
							if ((strIndex.Find("050003") != -1)||(strIndex.Find("060804") != -1))
							{	//������
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
							//		*(buf+(i+2)/2) = uCharTemp;   //��ת
								}
								 nDstLength = UnicodeToGB2312(buf,pDst->TP_UD,len);//buf
							}
						} 
						else 
						{	//������
							L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
							smsa.bIsLongSMS = TRUE;
						}
					}
					else 
					{	// 8-bit����
						j = nCString2Bytes(pSrc+nPos, buf, uCharTmp*2);
						if (j<140)
						{
							if ((strIndex.Find("050003") != -1)||(strIndex.Find("060804") != -1))
							{	//������
								L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
								smsa.bIsLongSMS = TRUE;
							}
							else
							{
								nDstLength = nCString2Bytes(pSrc+nPos, buf, uCharTmp * 2);			// ��ʽת��
								nDstLength = nDecode8bit(buf, pDst->TP_UD, len);	// ת����TP-DU
							}
						} 
						else
						{	//������
							L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
							smsa.bIsLongSMS = TRUE;
						}
					}
			smsa.nToReturn=1;
	} 
	else if(nTxRxFlag==2||nTxRxFlag==3)//��������PDU
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
						AfxMessageBox(" SMSC��ֻ�ܽ���0x91������.");
						smsa.nToReturn=0;
					}
					nPos += 2;
				//Ŀ�ĺ��� ���ռ��ˣ�
					RecoverPhoneNum(pSrc+nPos,pDst->SCA,nDstLength);//����14
					nPos += nDstLength;
				}
					//1100
					nPos += 4;//������ʲô
				//0D  
					nCString2Bytes(pSrc+nPos,&uCharTmp,2);
					int len = uCharTmp;
					if ((*(pSrc+nPos) == '0') && (*(pSrc+nPos+1) == 'D'))
					{
						nPos += 2;
						//91
						if ((*(pSrc+nPos) != '9') && (*(pSrc+nPos+1) != '1'))
						{
							AfxMessageBox("TPA��ֻ�ܽ���0x91������.");
							smsa.nToReturn=0;
						}
						else 
						{
							nPos += 2; 
							//��Դ����
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
								//��Դ����
									RecoverPhoneNum(pSrc+nPos,pDst->TPA,len+1);//12(F)
									nPos += (len+1);
							}
							else
							{
								nPos += (len+1);
								AfxMessageBox("�������ͳ�������91,81,A1��.");
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
				//08��ʾUnicode TP-DCS
					nCString2Bytes(pSrc+nPos,(unsigned char*)&pDst->TP_DCS,2);
					smsa.SMSInfo.TP_DCS = pDst->TP_DCS;

					if ((*(pSrc+nPos) == '0') && (*(pSrc+nPos+1) == '8'))//	AfxMessageBox("���뷽ʽ��Unicode 16λ����");
					{					
					} 
					else if ((*(pSrc+nPos) == '0') && (*(pSrc+nPos+1) == '0'))//	AfxMessageBox("���뷽ʽ�� 7λ����");
					{					
					} 
					else
					{
						AfxMessageBox("���뷽ʽ���� 7λ�������Unicode 16λ����");
					}
					nPos += 2;
				//��Ч�ڣ�
					nPos += 2;
				// �û���Ϣ����(TP-UDL)
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
				//��Ϣ���ݽ���
					if(pDst->TP_DCS == CODE_7BIT)	
					{
						if (j<140)
						{	// 7-bit����
							if ((strIndex.Find("050003") != -1)||(strIndex.Find("060804") != -1))
							{	//������
								L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
								smsa.bIsLongSMS = TRUE;
							}
							else
							{
							j = 0;
							j = nCString2Bytes(pSrc+nPos, buf, uCharTmp*2);
							nDecode7bit(buf, pDst->TP_UD, uCharTmp*2);	// ת����TP-DU  nDstLengthnDstLength
							nDstLength = uCharTmp;
							*(pDst->TP_UD+j) = '\0';
							}
						} 
						else
						{	//������
							L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
							smsa.bIsLongSMS = TRUE;
						}
					}
					else if(pDst->TP_DCS == CODE_Unicode)
					{	// Unicode����   //Ҫ�ر�ע��little endian��big endian���������������෴
						j = 0;
						j = nCString2Bytes(pSrc+nPos, buf, uCharTmp*2);
						if (j<140)
						{
								if ((strIndex.Find("050003") != -1)||(strIndex.Find("060804") != -1))
								{	//������
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
								//		*(buf+(i+2)/2) = uCharTemp;   //��ת
									}
									 nDstLength = UnicodeToGB2312(buf,pDst->TP_UD,len);//buf
								}
						} 
						else
						{	//������
							L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
							smsa.bIsLongSMS = TRUE;
						}
					}
					else
					{	// 8-bit����
						j = nCString2Bytes(pSrc+nPos, buf, uCharTmp*2);
						if (j<140)
						{
							if ((strIndex.Find("050003") != -1)||(strIndex.Find("060804") != -1))
							{	//������
								L = LongSMSTextDeCode(pSrc+nPos,pDst,pDst->TP_DCS,len);
								smsa.bIsLongSMS = TRUE;
							}
							else
							{								
								nDstLength = nCString2Bytes(pSrc+nPos, buf, uCharTmp * 2);			// ��ʽת��
								nDstLength = nDecode8bit(buf, pDst->TP_UD, len);	// ת����TP-DU
							}
						} 
						else
						{	//������
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
//�����ŵ����ݽ�����PDU����Ϣ���Ⱥ������ݵĽ���
//1��ʾ����ɹ���0��ʾ�������
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
	int nDstLength=0; //���ڽ���PDUʱ������ʾ����ĳЩ�ַ�����ģ�����
	unsigned char uCharTmp;		// �ڲ��õ���ʱ�ֽڱ���
	unsigned char buf[256];	// �ڲ��õĻ�����
	nCString2Bytes(pSrc+nPos,&uCharTmp,2);
	nlen = uCharTmp;
	if (nlen==5) //05
	{
		nPos += 2;
		if ((*(pSrc+nPos) != '0') && (*(pSrc+nPos+1) != '0'))  //00  �������ŵı�ʶλ����Ϊ1
		{
			AfxMessageBox("������0500����");
			i = 0;
			lToReturn.ntoreturn = i;
			return lToReturn;
		}
		nPos += 2;
		if ((*(pSrc+nPos) != '0') && (*(pSrc+nPos+1) != '3'))  //03  ʣ�¶��ű�ʶ�ĳ��� 
		{
			AfxMessageBox("������050003����");
			i = 0;
			lToReturn.ntoreturn = i;
			return lToReturn;
		}
		nPos += 2;
		// XX���ű�־ 
//		strSMSText = "ID:";
//		strSMSText += *(pSrc+nPos);
//		strSMSText += *(pSrc+nPos+1);
//		strSMSText += ". ";
		lToReturn.strID += *(pSrc+nPos);
		lToReturn.strID += *(pSrc+nPos+1);
		nPos += 2;
		//MM��������һ�����Ϊ5 
		strSMSTotalNum += *(pSrc+nPos);
		strSMSTotalNum += *(pSrc+nPos+1);
		lToReturn.nAllNum = atoi(strSMSTotalNum);
		nPos += 2;
		//NN�ڼ���
//		strSMSText += "��";
//		strSMSText += *(pSrc+nPos) + *(pSrc+nPos+1);
		char aTemp = *(pSrc+nPos+1);
		pDst->index = atoi(&aTemp);
//		strSMSText += "��: ";
		nPos += 2;		
	} 
	else if(nlen==6)
	{
		nPos += 2;
		if ((*(pSrc+nPos) != '0') && (*(pSrc+nPos+1) != '8'))  //08 �������ŵı�ʶλ����Ϊ2
		{
			AfxMessageBox("������0608����");
			i = 0;
			lToReturn.ntoreturn = i;
			return lToReturn;
		}
		nPos += 2;
		if ((*(pSrc+nPos) != '0') && (*(pSrc+nPos+1) != '4'))  //04  ʣ�¶��ű�ʶ�ĳ��� 
		{
			AfxMessageBox("������060804����");
			i = 0;
			lToReturn.ntoreturn = i;
			return lToReturn;
		}
		nPos += 2;
		// XX XX���ű�־ 
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
		//MM��������һ�����Ϊ5 
		strSMSTotalNum += *(pSrc+nPos);
		strSMSTotalNum += *(pSrc+nPos+1);
		lToReturn.nAllNum = atoi(strSMSTotalNum);
		nPos += 2;
		//NN�ڼ���
//		strSMSText += "��";
		strSMSText += *(pSrc+nPos);
		strSMSText += *(pSrc+nPos+1);
		char aTemp = *(pSrc+nPos+1);
		pDst->index = atoi(&aTemp);
//		strSMSText += "��: ";
		nPos += 2;
	}
	else
	{
		AfxMessageBox("������Э�鳤�Ȳ���05����08��");
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
					// 7-bit����
					nDecode7bit(buf, pDst->TP_UD, len*2);	// ת����TP-DU  nDstLengthnDstLength
					nDstLength = uCharTmp;
					*(pDst->TP_UD+j) = '\0';
				}
				else if(ndecodeMode == CODE_Unicode)
				{
					// Unicode����   //Ҫ�ر�ע��little endian��big endian���������������෴
					for (int i=0;i<len*2 - 1;i+=4) //ok done
					{
						nCString2Bytes(pSrc+nPos+i,buf+(i/2),2);
						nCString2Bytes(pSrc+nPos+i+2,buf+(i+2)/2,2);
				//		uCharTemp = *(buf+(i/2));
				//		*(buf+(i/2)) = *(buf+(i+2)/2);
				//		*(buf+(i+2)/2) = uCharTemp;   //��ת
					}
					 nDstLength = UnicodeToGB2312(buf,pDst->TP_UD,len);//buf
					 *(pDst->TP_UD+j) = '\0';
				}
				else  // 8-bit����
				{					
					nDstLength = nCString2Bytes(pSrc+nPos, buf, uCharTmp * 2);			// ��ʽת��
					nDstLength = nDecode8bit(buf, pDst->TP_UD, len);	// ת����TP-DU
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
{    //����ַ������ֽ�������ĳ���
	strCount count;
	int Lenth = str.GetLength();
	count.nEnglishLenth =0;
	count.nTotalLenth = 0;
	count.nChineseLenth = 0;
    for(int i=0;i<Lenth;i++)
    {
        char c = str.GetAt(i);
        //�������ַ� �����ַ����� 1XXX XXXX 1XXX XXXX
        if(c<0||c>255)
            continue;
        //��Ӣ���ַ�
        else 
            count.nEnglishLenth ++;
    }
    //���������ַ�����ÿ�������ַ�ռ�����ֽ�
    count.nChineseLenth = (Lenth-count.nEnglishLenth)/2;
    count.nTotalLenth = count.nChineseLenth + count.nEnglishLenth;
	return count;
}
//�����ص�CMS ERROR�������ת��Ϊ��Ӧ����������
void CMSError(CString str)
{
	int _int;
	_int = atoi(str);
	switch(_int)
	{
		case 300:
			AfxMessageBox("�ƶ��豸����");
			break;
		case 301:
			AfxMessageBox("���ŷ���������");
			break;
		case 302:
			AfxMessageBox("����������");
			break;
		case 303:
			AfxMessageBox("������֧��");
			break;
		case 304:
			AfxMessageBox("��Ч��PDUģʽ����");
			break;
		case 305:
			AfxMessageBox("��Ч��TEXTģʽ����");
			break;
		case 310:
			AfxMessageBox("SIM��δ����");
			break;
		case 311:
			AfxMessageBox("PIN���������");
			break;
		case 312:
			AfxMessageBox("PH-SIM PIN ����");
			break;
		case 313:
			AfxMessageBox("SIM ��ʧ��");
			break;
		case 314:
			AfxMessageBox("SIM ��æ");
			break;
		case 315:
			AfxMessageBox("SIM ������");
			break;
		case 316:
			AfxMessageBox("SIM PUK �������");
			break;
		case 317:
			AfxMessageBox("SIM PIN2 �������");
			break;
		case 318:
			AfxMessageBox("SIM PUK2 �������");
			break;
		case 320:
			AfxMessageBox("�洢ʧ��");
			break;
		case 321:
			AfxMessageBox("��Ч�Ĵ洢����");
			break;
		case 322:
			AfxMessageBox("�ڴ���");
			break;
		case 330:
			AfxMessageBox("�������ĵ�ַδ֪");
			break;
		case 331:
			AfxMessageBox("���������");
			break;
		case 332:
			AfxMessageBox("���糬ʱ");
			break;
		case 340:
			AfxMessageBox("+CNMA ACK ����");
			break;
		case 342:
			AfxMessageBox("���ų��ȴ���");
			break;
		case 500:
			AfxMessageBox("500: δ֪����");
			break;
		default:
			AfxMessageBox("δ֪����");
			break;
	}
}
//�����ű��룬��CEncodePDU���˸�nOrder����ʾ�ڼ�����������һ�¡�
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
//   51 ��������(TP-MTI/VFP) ��Ҫ���ͻظ� ���� 31  01 11
	chForPDU[nPos] = '5';
	chForPDU[nPos+1] = '1';
	nPos += 2;
	nTPDUlen += 2;
//00 ��Ϣ��׼ֵ(TP-MR)
	chForPDU[nPos] = '0';
	chForPDU[nPos+1] = '0';
	nPos += 2;
	nTPDUlen += 2;
//-----------------------------�ռ���
//����0D
	chForPDU[nPos] = '0';
	chForPDU[nPos+1] = 'D';
	nPos += 2;
	nTPDUlen += 2;
//91
	chForPDU[nPos] = '9';
	chForPDU[nPos+1] = '1';
	nPos += 2;
	nTPDUlen += 2;
//����
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
//08 TP-DCS  Unicode����           ���ﻹҪ�Ӹ�7bit���룬��8bit������
	if (bChineseFlag)
	{
		chForPDU[nPos] = '0';
		chForPDU[nPos+1] = '8';
		nPos += 2;	
		nTPDUlen += 2;
	} 
	else //	���ﻹҪ�Ӹ�7bit���룬
	{
		chForPDU[nPos] = '0';
		chForPDU[nPos+1] = '0';
		nPos += 2;	
		nTPDUlen += 2;
	}
//00  �ɱ�   A7 AA
	chForPDU[nPos] = 'A';
	chForPDU[nPos+1] = 'A';
	nPos += 2;
	nTPDUlen += 2;
	if (bChineseFlag)
	{
		CString strTmp = "";
	//--------------------------------------------
	//TP-UDL   ����050003XXXXXX 
	char a = nLenth*2+6;
	nBytes2CString((unsigned char *)&a,&chForPDU[nPos],2);//TP-UDL
	nPos += 2;
	nTPDUlen += 2;
	//TP-UD��ʼ  �����ű�־
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
	//������
	chForPDU[nPos] = '0';
	strncpy(&chForPDU[nPos+1],(LPCTSTR)TotalOrder,1);
	nPos += 2;
	nTPDUlen += 2;
	//�ڼ���
	chForPDU[nPos] = '0';
//	chForPDU[nPos+1] = Order.Mid(0,1);
	strncpy(&chForPDU[nPos+1],(LPCTSTR)Order,1);
	nPos += 2;
	nTPDUlen += 2;

//������TP-UD����
		unsigned char buf[256];
		int i = nEncodeUnicode(chText,buf,strSMSText.GetLength());
//		int i = nEncodeUnicode(chText,buf,nLenth*2);
		int j=nBytes2CString(buf,&chForPDU[nPos],i*2);
		nTPDUlen += i*2;
//		PDUlength = nPos + i*2; //PDU �ܳ���
		chForPDU[nPos + i*2] = 0x1a;//     ctrl+z   �ں���cEncodePDU��ʵ��
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
		//7bit����
		unsigned char buf[256];
	//	int i = nEncode7bit(chText,buf,strSMSText.GetLength());
		int i = nEncode7bit(chText,buf,nLenth*2);
		int j=nBytes2CString(buf,&chForPDU[nPos],i*2);
		nTPDUlen += i*2;
//		PDUlength = nPos + i*2; //PDU �ܳ���
		chForPDU[nPos + i*2] = 0x1a;//     ctrl+z  �ں���cEncodePDU��ʵ��
		chForPDU[nPos + i*2+1] = '\0';
	}
	//---------------------����PDU
	c.nLenthToReturn = nTPDUlen/2;
	return c;
}