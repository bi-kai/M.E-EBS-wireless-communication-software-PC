#include "stdafx.h"
#include "autoResponse.h"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"WINMM.LIB")


int nCheckCount;//���ڼ�����֤�û�������(������ʼ��1#��2#)�Ĵ������������
int nFlag;//0��ʾ������1��ʾ�����û�����֤��2��ʾ����������֤ 
//�Զ�Ӧ����
//ModeID,����ģ��ʶ���־ ����δ�ã�
//str, �ϲ���ܵ����ַ��������û����������
//����ֵ:
//������֤�ɹ�����й㲥��1
//����ʧ�� 0 ������Ӧ��Ҫ�һ���
//�Զ�Ӧ��������ڽ����� 2
//ע��㣺
//a��ÿ������������Ҫ�Ӹ���ʱ��,ʱ�䵽Ҫ�һ�������Ҫ���趨��ȫ�ֱ�����ʼ������Ϊ0����NNNNNNNNN��Ҫ������ʱ�Ͳ����Զ��һ���
//��ʱ����������Ϊ15S
//b����ȫ�ֱ�����int nFlag = 0; //0��ʾ������1��ʾ�����û�����֤��2��ʾ����������֤ 
//c����ȫ�ֱ�����int nCheckCount = 0;//���ڼ�����֤�û�������Ĵ������������
//d�������û������������Ϊ123456�������������Լ������ݿ⡣��ʱnLength�ж�������Ҫ�ı�
 

//int nFlag = 0; //0��ʾ������1��ʾ�����û�����֤��2��ʾ����������֤ ��ȫ�ֱ�����
//int nCheckCount = 0;//���ڼ�����֤�û�������Ĵ������������ ��ȫ�ֱ�����
int bAutoResponse(int ModeID, CString str)
{
	int nLength = str.GetLength();
	int i;//���ڷ���

	if (nLength == 2)
	{
		if ((str.Mid(0,1)=="1") && (str.Mid(1,1)=="#"))
		{
			if (nFlag == 0)
			{
				PlaySound(".//Wav//InputID.wav", NULL, SND_FILENAME|SND_ASYNC);
				nFlag = 1;
				nCheckCount = 0;
				i = 2;
			} 
			else
			{
				i = OtherCmdDeal(str);
			}

		} 
		else
		{
			i = OtherCmdDeal(str);
		}
	} 
	else if(nLength == 7)
	{
		nCheckCount++;
			if (nFlag == 1) //�������˺���֤
			{
				if (nCheckCount<=2)//ǰ������֤�˺�
				{
					//------�˴���֤���ݿ��е��û�--------//
					//���ݿ����
						if ((str.Mid(0,1)=="1") && (str.Mid(1,1)=="2") && (str.Mid(2,1)=="3") && (str.Mid(3,1)=="4")
							&& (str.Mid(4,1)=="5") && (str.Mid(5,1)=="6") && (str.Mid(6,1)=="#"))
						{
							PlaySound(".//Wav//InputPassword.wav", NULL, SND_FILENAME|SND_ASYNC);//����������#�Ž���
							nFlag = 2;
							nCheckCount = 0; //����������֤ 
							i = 2;
						} 
						else
						{
							PlaySound(".//Wav//IDError.wav", NULL, SND_FILENAME|SND_ASYNC);//�˺Ŵ��������������˺�
							i = 2;
						}
				}
				else if (nCheckCount == 3)//��������֤�˺�
				{
					//------�˴���֤���ݿ��е��û�--------//
					//���ݿ����
						if ((str.Mid(0,1)=="1") && (str.Mid(1,1)=="2") && (str.Mid(2,1)=="3") && (str.Mid(3,1)=="4")
							&& (str.Mid(4,1)=="5") && (str.Mid(5,1)=="6") && (str.Mid(6,1)=="#"))
						{
							PlaySound(".//Wav//InputPassword.wav", NULL, SND_FILENAME|SND_ASYNC);//����������#�Ž���
							nFlag = 2;
							nCheckCount = 0; //�������˺���֤�ɹ�������������֤ 
							i = 2;
						} 
						else
						{
							PlaySound(".//Wav//MultiIDError.wav", NULL, SND_FILENAME|SND_ASYNC);//���Ѿ��������˺ţ��Բ�����˶Ժ�����
							nFlag = 0;
							nCheckCount = 0;
							i = 0;						//������Ӧ�һ�
						}
				} 
				else
				{
					AfxMessageBox("nCheckCount error");
					nFlag = 0;
					nCheckCount = 0;
					i = 0;
				}				
			}
			else if(nFlag == 2)
			{
				//�˴���֤���ݿ������
				if (nCheckCount<=2)//ǰ������֤����
				{
					//------�˴���֤���ݿ��е�����--------//
						if ((str.Mid(0,1)=="1") && (str.Mid(1,1)=="2") && (str.Mid(2,1)=="3") && (str.Mid(3,1)=="4")
							&& (str.Mid(4,1)=="5") && (str.Mid(5,1)=="6") && (str.Mid(6,1)=="#"))
						{
							PlaySound(".//Wav//BroadPrepare.wav", NULL, SND_FILENAME|SND_ASYNC);//�㲥����׼���У����Ժ�

							nFlag = 0;
							nCheckCount = 0;
							i = 1;
						} 
						else
						{
							PlaySound(".//Wav//PasswordError.wav", NULL, SND_FILENAME|SND_ASYNC);//������������������˺�
							i = 2;
						}
				}
				else if (nCheckCount == 3)//��������֤�˺�
				{
					//------�˴���֤���ݿ��е�����--------//
						if ((str.Mid(0,1)=="1") && (str.Mid(1,1)=="2") && (str.Mid(2,1)=="3") && (str.Mid(3,1)=="4")
							&& (str.Mid(4,1)=="5") && (str.Mid(5,1)=="6") && (str.Mid(6,1)=="#"))
						{
							PlaySound(".//Wav//BroadPrepare.wav", NULL, SND_FILENAME|SND_ASYNC);//�㲥����׼���У����Ժ�
							nFlag = 0;
							nCheckCount = 0;
							i = 1;
						} 
						else
						{
							PlaySound(".//Wav//MultiPasswordError.wav", NULL, SND_FILENAME|SND_ASYNC);//���Ѿ����������룬�Բ�����˶Ժ�����
							nFlag = 0;
							nCheckCount = 0;
							i = 0;						//������Ӧ�һ�
						}
				} 
				else
				{
					AfxMessageBox("nCheckCount error");
					nFlag = 0;
					nCheckCount = 0;
					i = 0;
				}				
			}
	}
	else
	{
			i = OtherCmdDeal(str);
	}
	return i;
}
//�����ж�һЩ����ķǷ�ָ���Ҫ��Ե���ָ��Ȳ�һ��
int OtherCmdDeal(CString str)
{
	int i;
		if (nFlag == 0)
			{
				nCheckCount++;
				if (nCheckCount == 3)//�����ο�ʼ����
				{
						PlaySound(".//Wav//OperationError.wav", NULL, SND_FILENAME|SND_ASYNC);//���Ѿ���β���ʧ��
						nFlag = 0;
						nCheckCount = 0;
						i = 0;						//������Ӧ�һ�
				}
				else
				{
					PlaySound(".//Wav//ErrorCmd.wav", NULL, SND_FILENAME|SND_ASYNC);//ָ���������������
					i = 2;					
				}
			}
			else if (nFlag == 1)
			{
				nCheckCount++;
				if (nCheckCount == 3)//��������֤�˺�
				{
						PlaySound(".//Wav//MultiIDError.wav", NULL, SND_FILENAME|SND_ASYNC);//���Ѿ��������˺ţ��Բ�����˶Ժ�����
						nFlag = 0;
						nCheckCount = 0;
						i = 0;						//������Ӧ�һ�
				}
				else
				{
					PlaySound(".//Wav//IDError.wav", NULL, SND_FILENAME|SND_ASYNC);//�˺Ŵ��������������˺�
					i = 2;				
				}
			} 
			else if (nFlag == 2)
			{
				nCheckCount++;

				if (nCheckCount == 3)//��������֤�˺�
				{
					PlaySound(".//Wav//MultiPasswordError.wav", NULL, SND_FILENAME|SND_ASYNC);//���Ѿ����������룬�Բ�����˶Ժ�����
					nFlag = 0;
					nCheckCount = 0;
					i = 0;						//������Ӧ�һ�
				} 
				else
				{
					PlaySound(".//Wav//PasswordError.wav", NULL, SND_FILENAME|SND_ASYNC);//������������������˺�
					i = 2;					
				}
			}
		return i;
}


