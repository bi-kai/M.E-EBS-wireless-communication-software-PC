#include "stdafx.h"
#include "autoResponse.h"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"WINMM.LIB")


int nCheckCount;//用于计算验证用户或密码(包括开始的1#和2#)的次数，最多三次
int nFlag;//0表示其他，1表示正在用户名验证，2表示正在密码验证 
//自动应答函数
//ModeID,各个模块识别标志 （暂未用）
//str, 上层接受到的字符串，如用户名，密码等
//返回值:
//各种验证成功后进行广播，1
//操作失败 0 （后续应该要挂机）
//自动应答过程正在进行中 2
//注意点：
//a、每个播放声音都要加个定时器,时间到要挂机，而且要把设定的全局变量初始化（变为0）（NNNNNNNNN重要，否则超时就不能自动挂机）
//定时器可以设置为15S
//b、（全局变量）int nFlag = 0; //0表示其他，1表示正在用户名验证，2表示正在密码验证 
//c、（全局变量）int nCheckCount = 0;//用于计算验证用户或密码的次数，最多三次
//d、现在用户和密码均简单设为123456，后续工作可以加入数据库。此时nLength判断条件就要改变
 

//int nFlag = 0; //0表示其他，1表示正在用户名验证，2表示正在密码验证 （全局变量）
//int nCheckCount = 0;//用于计算验证用户或密码的次数，最多三次 （全局变量）
int bAutoResponse(int ModeID, CString str)
{
	int nLength = str.GetLength();
	int i;//用于返回

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
			if (nFlag == 1) //正处于账号验证
			{
				if (nCheckCount<=2)//前两次验证账号
				{
					//------此处验证数据库中的用户--------//
					//数据库操作
						if ((str.Mid(0,1)=="1") && (str.Mid(1,1)=="2") && (str.Mid(2,1)=="3") && (str.Mid(3,1)=="4")
							&& (str.Mid(4,1)=="5") && (str.Mid(5,1)=="6") && (str.Mid(6,1)=="#"))
						{
							PlaySound(".//Wav//InputPassword.wav", NULL, SND_FILENAME|SND_ASYNC);//请输入密码#号结束
							nFlag = 2;
							nCheckCount = 0; //进入密码验证 
							i = 2;
						} 
						else
						{
							PlaySound(".//Wav//IDError.wav", NULL, SND_FILENAME|SND_ASYNC);//账号错误，请重新输入账号
							i = 2;
						}
				}
				else if (nCheckCount == 3)//第三次验证账号
				{
					//------此处验证数据库中的用户--------//
					//数据库操作
						if ((str.Mid(0,1)=="1") && (str.Mid(1,1)=="2") && (str.Mid(2,1)=="3") && (str.Mid(3,1)=="4")
							&& (str.Mid(4,1)=="5") && (str.Mid(5,1)=="6") && (str.Mid(6,1)=="#"))
						{
							PlaySound(".//Wav//InputPassword.wav", NULL, SND_FILENAME|SND_ASYNC);//请输入密码#号结束
							nFlag = 2;
							nCheckCount = 0; //第三次账号验证成功，进入密码验证 
							i = 2;
						} 
						else
						{
							PlaySound(".//Wav//MultiIDError.wav", NULL, SND_FILENAME|SND_ASYNC);//您已经多次输错账号，对不起，请核对后再试
							nFlag = 0;
							nCheckCount = 0;
							i = 0;						//接下来应挂机
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
				//此处验证数据库的密码
				if (nCheckCount<=2)//前两次验证密码
				{
					//------此处验证数据库中的密码--------//
						if ((str.Mid(0,1)=="1") && (str.Mid(1,1)=="2") && (str.Mid(2,1)=="3") && (str.Mid(3,1)=="4")
							&& (str.Mid(4,1)=="5") && (str.Mid(5,1)=="6") && (str.Mid(6,1)=="#"))
						{
							PlaySound(".//Wav//BroadPrepare.wav", NULL, SND_FILENAME|SND_ASYNC);//广播正在准备中，请稍候

							nFlag = 0;
							nCheckCount = 0;
							i = 1;
						} 
						else
						{
							PlaySound(".//Wav//PasswordError.wav", NULL, SND_FILENAME|SND_ASYNC);//密码错误，请重新输入账号
							i = 2;
						}
				}
				else if (nCheckCount == 3)//第三次验证账号
				{
					//------此处验证数据库中的密码--------//
						if ((str.Mid(0,1)=="1") && (str.Mid(1,1)=="2") && (str.Mid(2,1)=="3") && (str.Mid(3,1)=="4")
							&& (str.Mid(4,1)=="5") && (str.Mid(5,1)=="6") && (str.Mid(6,1)=="#"))
						{
							PlaySound(".//Wav//BroadPrepare.wav", NULL, SND_FILENAME|SND_ASYNC);//广播正在准备中，请稍候
							nFlag = 0;
							nCheckCount = 0;
							i = 1;
						} 
						else
						{
							PlaySound(".//Wav//MultiPasswordError.wav", NULL, SND_FILENAME|SND_ASYNC);//您已经多次输错密码，对不起，请核对后再试
							nFlag = 0;
							nCheckCount = 0;
							i = 0;						//接下来应挂机
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
//用于判断一些输入的非法指令，主要针对的是指令长度不一样
int OtherCmdDeal(CString str)
{
	int i;
		if (nFlag == 0)
			{
				nCheckCount++;
				if (nCheckCount == 3)//第三次开始操作
				{
						PlaySound(".//Wav//OperationError.wav", NULL, SND_FILENAME|SND_ASYNC);//您已经多次操作失误
						nFlag = 0;
						nCheckCount = 0;
						i = 0;						//接下来应挂机
				}
				else
				{
					PlaySound(".//Wav//ErrorCmd.wav", NULL, SND_FILENAME|SND_ASYNC);//指令出错，请重新输入
					i = 2;					
				}
			}
			else if (nFlag == 1)
			{
				nCheckCount++;
				if (nCheckCount == 3)//第三次验证账号
				{
						PlaySound(".//Wav//MultiIDError.wav", NULL, SND_FILENAME|SND_ASYNC);//您已经多次输错账号，对不起，请核对后再试
						nFlag = 0;
						nCheckCount = 0;
						i = 0;						//接下来应挂机
				}
				else
				{
					PlaySound(".//Wav//IDError.wav", NULL, SND_FILENAME|SND_ASYNC);//账号错误，请重新输入账号
					i = 2;				
				}
			} 
			else if (nFlag == 2)
			{
				nCheckCount++;

				if (nCheckCount == 3)//第三次验证账号
				{
					PlaySound(".//Wav//MultiPasswordError.wav", NULL, SND_FILENAME|SND_ASYNC);//您已经多次输错密码，对不起，请核对后再试
					nFlag = 0;
					nCheckCount = 0;
					i = 0;						//接下来应挂机
				} 
				else
				{
					PlaySound(".//Wav//PasswordError.wav", NULL, SND_FILENAME|SND_ASYNC);//密码错误，请重新输入账号
					i = 2;					
				}
			}
		return i;
}


