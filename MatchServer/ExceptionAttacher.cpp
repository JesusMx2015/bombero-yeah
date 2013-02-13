#include "stdafx.h"
#include "MDebug.h"
#include "Shlwapi.h"
#include "MCrashDump.h"
#include "MMatchStatus.h"
#include "MMatchServer.h"
#include "MMatchConfig.h"
#include "UnitTest.h"
#include "CrashRpt.h"

#ifndef _DEBUG
#define SUPPORT_EXCEPTIONHANDLING
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static bool GetRecommandLogFileName(char* pszBuf)
{
	if (PathIsDirectory("Log") == FALSE)
		CreateDirectory("Log", NULL);

	time_t		tClock;
	struct tm*	ptmTime;

	time(&tClock);
	ptmTime = localtime(&tClock);

	char szFileName[_MAX_DIR];

	int nFooter = 1;
	while(TRUE) {
		sprintf(szFileName, "Log/MatchLog_%02d-%02d-%02d-%d.txt", 
			ptmTime->tm_year+1900, ptmTime->tm_mon+1, ptmTime->tm_mday, nFooter);

		if (PathFileExists(szFileName) == FALSE)
			break;

		nFooter++;
		if (nFooter > 100) return false;
	}
	strcpy(pszBuf, szFileName);
	return true;
}



//int AFXAPI AfxWinMain(HINSTANCE hInstance, 
//					  HINSTANCE hPrevInstance,	
//					  LPTSTR lpCmdLine, 
//					  int nCmdShow)
//{
//	// Current Directory�� �����.
//	char szModuleFileName[_MAX_DIR] = {0,};
//	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
//	PathRemoveFileSpec(szModuleFileName);
//	SetCurrentDirectory(szModuleFileName);
//
//	char szLogFileName[_MAX_DIR];
//	if (GetRecommandLogFileName(szLogFileName) == false) 
//		return FALSE;
//
//	InitLog(
//		MLOGSTYLE_DEBUGSTRING|
//		MLOGSTYLE_FILE, szLogFileName);
//
//#ifdef _DEBUG
//	if(!RunUnitTest())
//		return 0;
//#endif
////	return 0;	//TDD �۾��Ҷ� �̰ɷ� ���������Ű�� ����
//
//
//	// Wrap WinMain in a structured exception handler (different from C++
//	// exception handling) in order to make sure that all access violations
//	// and other exceptions are displayed - regardless of when they happen.
//	// This should be done for each thread, if at all possible, so that exceptions
//	// will be reliably caught, even inside the debugger.
//#ifdef SUPPORT_EXCEPTIONHANDLING
//	char szDumpFileName[_MAX_DIR]; 
//	strcpy(szDumpFileName, szLogFileName);
//	strcat(szDumpFileName, ".dmp");
//
//	char szDmpFileName[MAX_PATH] = {0, };
//	strcpy(szDmpFileName, &szDumpFileName[4]);
//
//	char szTxtFileName[MAX_PATH] = {0, };
//	strcpy(szTxtFileName, &szLogFileName[4]);
//	__try {
//#endif
//		// The code inside the __try block is the MFC version of AfxWinMain(),
//		// copied verbatim from the MFC source code.
//		ASSERT(hPrevInstance == NULL);
//
//		int nReturnCode = -1;
//		CWinApp* pApp = AfxGetApp();
//
//		// AFX internal initialization
//		if (!AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
//			goto InitFailure;
//
//		// App global initializations (rare)
//		ASSERT_VALID(pApp);
//		if (!pApp->InitApplication())
//			goto InitFailure;
//		ASSERT_VALID(pApp);
//
//		// Perform specific initializations
//		if (!pApp->InitInstance())
//		{
//			if (pApp->m_pMainWnd != NULL)
//			{
//				TRACE(_T("Warning: Destroying non-NULL m_pMainWnd\n"));
//				pApp->m_pMainWnd->DestroyWindow();
//			}
//			nReturnCode = pApp->ExitInstance();
//			goto InitFailure;
//		}
//		ASSERT_VALID(pApp);
//
//		nReturnCode = pApp->Run();
//		ASSERT_VALID(pApp);
//
//InitFailure:
//#ifdef _DEBUG
//		// Check for missing AfxLockTempMap calls
//		if (AfxGetModuleThreadState()->m_nTempMapLock != 0)
//		{
//			TRACE(_T("Warning: Temp map lock count non-zero (%ld).\n"),
//				AfxGetModuleThreadState()->m_nTempMapLock);
//		}
//		AfxLockTempMaps();
//		AfxUnlockTempMaps(-1);
//#endif
//
//		AfxWinTerm();
//		return nReturnCode;
//
//#ifdef SUPPORT_EXCEPTIONHANDLING
//	}
////	__except(MFilterException(GetExceptionInformation())){
//	__except(CrashExceptionDump(GetExceptionInformation(), szDumpFileName, true))
//	{
////		char szFileName[_MAX_DIR];
////		GetModuleFileName(NULL, szFileName, _MAX_DIR);
////		WinExec(szFileName, SW_SHOW);	// Launch again
//		MMatchServer::GetInstance()->CheckMemoryTest();
//		MGetServerStatusSingleton()->Dump();
//
//		char szScriptName[MAX_PATH] = {0, };
//		wsprintf(szScriptName, "Log\\UpLoadScript.txt");
//		FILE* fp = fopen(szScriptName, "w+");
//		if (fp != NULL)
//		{
//			char szServerID[MAX_PATH] = {0, };
//			sprintf(szServerID, "%d", MGetServerConfig()->GetServerID());
//			char szLogDir[MAX_PATH] = {0, };
//			strcpy(szLogDir, "C:\\GunzServer\\MatchServer\\Log");
//
//			// ��ũ��Ʈ �ۼ�
//			fprintf(fp, "user\nserver\nshsrhfxkd\nbinary\ncd LogFile\n");
//#ifdef LOCALE_KOREA		
//			fprintf(fp, "cd KOREA\n");			
//#endif
//#ifdef LOCALE_JAPAN
//			fprintf(fp, "cd JAPAN\n");
//#endif
//#ifdef LOCALE_NHNUSA
//			fprintf(fp, "cd USA\n");
//#endif
//#ifdef LOCALE_BRAZIL
//			fprintf(fp, "cd BRAZIL\n");
//#endif
//#ifdef LOCALE_INDIA
//			fprintf(fp, "cd INDIA\n");
//#endif
//#ifdef LOCALE_US
//			fprintf(fp, "cd INTERNATIONAL\n");
//#endif
//			fprintf(fp, "lcd %s\n", szLogDir);
//			fprintf(fp, "put %s_%s\n", szServerID, szDmpFileName);
//			fprintf(fp, "put %s_%s\nbye\n", szServerID, szTxtFileName);
//			fclose(fp);
//
//			char temp_arg[256] = {0, };
//			wsprintf(temp_arg, "%s %s %s", szServerID, szDmpFileName, szTxtFileName);
//
//			ShellExecute(NULL, _T("open"), _T("UpLoadLogFile.bat"), _T(temp_arg), NULL, SW_HIDE);
//		}
//	}
//#endif
//	return 0;
//}
//
//


int AFXAPI AfxWinMain(HINSTANCE hInstance, 
					  HINSTANCE hPrevInstance,	
					  LPTSTR lpCmdLine, 
					  int nCmdShow)
{
	// Install crash reporting
 
  CR_INSTALL_INFO info;
  memset(&info, 0, sizeof(CR_INSTALL_INFO));
  info.cb = sizeof(CR_INSTALL_INFO);  
  info.pszAppName = _T("MatchServer"); // Define application name.

  //info.pszSmtpLogin
  info.pszAppVersion = _T("1.0.0.1"); // Define application version.
  // URL for sending error reports over HTTP.
  info.pszUrl = _T("http://gunz.abbygamerz.net/webgunz/test/upload/upload2.php");                    
  info.pszSmtpProxy=_T("smtp.gmail.com:465"); 
  info.pszSmtpLogin =_T("asdasd@gmail.com");
  info.pszSmtpPassword =_T("asdasd");
  info.pszEmailTo = _T("asdasd@gmail.com");    

  // Install all available exception handlers.
  info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS; 
  // Provide privacy policy URL
 // info.pszPrivacyPolicyURL = _T("http://someserver.com/privacy.html");

  // Restart the app on crash 
  info.dwFlags |= CR_INST_NO_GUI;  
  info.dwFlags |= CR_INST_APP_RESTART; 
  info.dwFlags |= CR_INST_SEND_QUEUED_REPORTS; 
  info.pszRestartCmdLine = _T("/restart");


  int nResult = crInstall(&info);
  if(nResult!=0)
  {
    TCHAR buff[256];
    crGetLastErrorMsg(buff, 256);
    MessageBox(NULL, buff, _T("crInstall error"), MB_OK);
    return 1;
  }

  // Take screenshot of the app window at the moment of crash
  crAddScreenshot2(CR_AS_MAIN_WINDOW|CR_AS_USE_JPEG_FORMAT, 95);
  	



	// Current Directory�� �����.
	char szModuleFileName[_MAX_DIR] = {0,};
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);
	SetCurrentDirectory(szModuleFileName);

	char szLogFileName[_MAX_DIR];
	if (GetRecommandLogFileName(szLogFileName) == false) 
		return FALSE;

	InitLog(
		MLOGSTYLE_DEBUGSTRING|
		MLOGSTYLE_FILE, szLogFileName);



		// The code inside the __try block is the MFC version of AfxWinMain(),
		// copied verbatim from the MFC source code.
		ASSERT(hPrevInstance == NULL);

		int nReturnCode = -1;
		CWinApp* pApp = AfxGetApp();

		// AFX internal initialization
		if (!AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
			goto InitFailure;

		// App global initializations (rare)
		ASSERT_VALID(pApp);
		if (!pApp->InitApplication())
			goto InitFailure;
		ASSERT_VALID(pApp);

		// Perform specific initializations
		if (!pApp->InitInstance())
		{
			if (pApp->m_pMainWnd != NULL)
			{
				TRACE(_T("Warning: Destroying non-NULL m_pMainWnd\n"));
				pApp->m_pMainWnd->DestroyWindow();
			}
			nReturnCode = pApp->ExitInstance();
			goto InitFailure;
		}
		ASSERT_VALID(pApp);

	/*	 int* p = NULL;
		*p = 13;*/
		nReturnCode = pApp->Run();

	
		 // Uninstall crash reporting
		crUninstall();

		ASSERT_VALID(pApp);

InitFailure:


		AfxWinTerm();
		return nReturnCode;


	return 0;
}
