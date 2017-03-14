#include "StdAfx.h"
#include "tts.h"

#pragma comment(lib,"ole32.lib")  //CoInitialize CoCreateInstance��Ҫ����ole32.dll  
#pragma comment(lib,"sapi.lib")   //sapi.lib��SDK��libĿ¼,������ȷ���� 

void  wStrToWChar(CString strSpeak,WCHAR *wChar,int length)
{
	MultiByteToWideChar( CP_ACP , 0 , strSpeak , length , wChar ,length);
	//�����ͽ��ı����е��ַ���strSpeakת��ΪWCHAR�͵�wChar��������.
}

BOOL ttsSpeak(CString str)
{
	HRESULT hr = S_OK;
    CComPtr<IEnumSpObjectTokens> cpEnum;
    ULONG ulCount = 0;
    CComPtr<ISpObjectToken> cpToken;


	ISpVoice * pVoice = NULL;   //COM��ʼ����   
	if (FAILED(CoInitialize(NULL)))    
		return FALSE; 
	//��ȡISpVoice�ӿڣ�  
	hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);  
//	hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_INPROC_SERVER, IID_ISpVoice, (void **)&pVoice);  
	if( FAILED( hr ) )  
	{ 
		return FALSE;
	}
	
	if (FAILED(SpEnumTokens(SPCAT_VOICES, L"Name=VW Liang", NULL, &cpEnum))) {
        return FALSE;
    }
	
	if (FAILED(cpEnum->Next(1, &cpToken, NULL))) {
        return FALSE;
    }	
    //set the voice 
    if (FAILED(pVoice->SetVoice( cpToken))) {
        return FALSE;
    }
	
		hr = pVoice->SetVolume(100);
		hr = pVoice->SetRate(-3);
		
		int	length = str.GetLength();
		WCHAR *wtx = NULL;
		wtx = (WCHAR *)malloc(sizeof(WCHAR)*(1024));
		wStrToWChar(str,wtx,length);
		wtx[length] = 0;
		/***** start to speak******/
		hr = pVoice->Speak(wtx,SPF_DEFAULT|SPF_IS_XML, NULL);
//		pVoice->Speak(wtx,SPF_ASYNC | SPF_IS_XML, NULL);
		
		
		free(wtx);
		pVoice->Release();  
		pVoice = NULL;  
	
	//ǧ��Ҫ���ǣ�   
		CoUninitialize();    
	return TRUE;  
}
bool CheckString(CString str)
{
	if (str.Find('\"')!=-1)
	{
		for(int i=0;i<str.GetLength();i++)
		{
			if(str.GetAt(i)=='\"')//˫����
			{
				str.Delete(i,1);
			}		
		}
	}
	
	if (str.Find("@@@@") != -1)
		return true;
	else
		return false;
}

