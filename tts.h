#ifndef  TTS_H
#define TTS_H

#include "afx.h"
#include <sapi.h>
#include <sphelper.h>
#include <spuihelp.h>

//#pragma comment(lib,"ole32.lib")  //CoInitialize CoCreateInstance需要调用ole32.dll  
//#pragma comment(lib,"sapi.lib")   //sapi.lib在SDK的lib目录,必需正确配置   

//字符串strSpeak转化为WCHAR型的wChar
void  wStrToWChar(CString strSpeak,WCHAR *wChar,int length);

//tts 中文转语音
BOOL ttsSpeak(CString str);

//检测string中是否含有""   // 并检验是否要将文本广播出去，@@@@表示true，要广播
bool CheckString(CString str);


void TTS_Realease( void );


#endif



