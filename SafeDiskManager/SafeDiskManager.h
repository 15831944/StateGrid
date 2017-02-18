
// SafeDiskManager.h : SafeDiskManager Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include "ProtoManager.h"
#include "AppConfig.h"
#include "UserConfig.h"

#include "gSoap/GSoapUtils.h"
#include "EventHttpServer.h"
#include "EventHttpFileServer.h"

// CSafeDiskManagerApp:
// �йش����ʵ�֣������ SafeDiskManager.cpp
//

class CSafeDiskManagerApp : public CWinAppEx
{
public:
	CSafeDiskManagerApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	UINT m_nAppLook;
	BOOL m_bDefaultAllow;
	BOOL m_bHiColorIcons;
	CFont m_Font;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
#if !defined(FULL_VERSION)
	//BOOL VerifyDog();
#endif

public:
	CString GenDisplayName(const ClientInfo_t *pInfo);
	CString GenResponse(CString strLog, BOOL bRet, std::map<CString, CString> mapPair);
	CString GenResponse(CString strLog, BOOL bRet);

public:
	CString m_strMacId;
	UINT m_uHostCount;
	
public:
	CString m_strTmpPath;
	CString m_strVncClientPath;

public:
	CUserConfig m_userConfig;
	CUserConfig::USER_INFO m_CurUserInfo;

public:
	CGSoapServerUtils m_Soap;
	CString m_strAppPath;
	CString m_strConfigPath;
	BOOL m_bLockInfoFromServiceUpdated;
//	CEventHttpServer m_HttpServer;
	CEventHttpFileServer m_HttpFileServer;
};

extern CSafeDiskManagerApp theApp;

