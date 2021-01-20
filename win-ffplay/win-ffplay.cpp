// win-xserver.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "win-ffplay-dlg.h"
#include "App.h"

#if _DEBUG
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#endif


CApp *app = nullptr;


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
#if 0
	_CrtSetBreakAlloc(116);

	_CrtMemState s1, s2, s3;

	_CrtMemCheckpoint(&s1);

#endif // DEBUG

	CoInitialize(nullptr);

	app = CApp::instance();


	app->InitTheme();
	app->InitInstance(hInstance, hPrevInstance, lpCmdLine, nCmdShow);


	CApp::bindLayoutPath(std::string(CApp::getAppPath() + "layout\\").c_str());
	CApp::bindImagePath(std::string(CApp::getAppPath() + "image\\").c_str());

	QYLanguage::instance()->setLangDir(std::string(CApp::getAppPath() + "lan").c_str());
	QYLanguage::instance()->addLang("en");
	QYLanguage::instance()->addLang("zh");
	QYLanguage::instance()->setLang("zh");

#if _DEBUG
	QYImageManager::instance()->packImages(QYApp::getAppPath() + "image", std::string("image.dat"));
#endif
	QYImageManager::instance()->loadImages(CApp::m_imagePath + "image.dat");


	MainDlg *dlg = new MainDlg;

	if (1 != dlg->DoModal(nullptr))
	{
		goto EXIT_MAIN;
	}

EXIT_MAIN:
	
	SAFE_DESTROY_WINDOW_PTR(dlg);

	app->ExitInstance();
	SAFE_DELETE(app);

	CoUninitialize();

#if 0
	_CrtMemCheckpoint(&s2);
	//8680 bytes in 182 Normal Blocks.4170 bytes in 4 CRT Blocks.

	if (_CrtMemDifference(&s3, &s1, &s2))
	{
		_CrtMemDumpStatistics(&s3);
	}
#endif

	return 0;
}
