#ifndef  TTS_H
#define TTS_H

#include "afx.h"
#include <sapi.h>
#include <sphelper.h>
#include <spuihelp.h>

//#pragma comment(lib,"ole32.lib")  //CoInitialize CoCreateInstance��Ҫ����ole32.dll  
//#pragma comment(lib,"sapi.lib")   //sapi.lib��SDK��libĿ¼,������ȷ����   

//�ַ���strSpeakת��ΪWCHAR�͵�wChar
void  wStrToWChar(CString strSpeak,WCHAR *wChar,int length);

//tts ����ת����
BOOL ttsSpeak(CString str);

//���string���Ƿ���""   // �������Ƿ�Ҫ���ı��㲥��ȥ��@@@@��ʾtrue��Ҫ�㲥
bool CheckString(CString str);


void TTS_Realease( void );


#endif



