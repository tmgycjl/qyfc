// win-xserver.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Mainframe.h"
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

	CoInitialize(nullptr);

	app = CApp::instance();

	app->InitTheme();
	app->InitInstance(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	CApp::bindLayoutPath(std::string(CApp::getAppPath() + "../../layout\\").c_str());
	CApp::bindImagePath(std::string(CApp::getAppPath() + "../../image\\").c_str());

	QYLanguage::instance()->setLangDir(std::string(CApp::getAppPath() + "../../lan").c_str());
	QYLanguage::instance()->addLang("en");
	QYLanguage::instance()->addLang("zh");
	QYLanguage::instance()->setLang("zh");

#if _DEBUG
	QYImageManager::instance()->packImages(QYApp::getAppPath() + "../../image", std::string("image.dat"));
#endif
	QYImageManager::instance()->loadImages(CApp::m_imagePath + "image.dat");

	CMainframe *pMainWindow = CMainframe::instance();

	app->SetMainWindow(pMainWindow);
	pMainWindow->ShowShadow(TRUE);
	pMainWindow->SetIcon(IDI_SMALL);
	pMainWindow->DoModal();

	SAFE_DESTROY_WINDOW_PTR(pMainWindow);

	app->ExitInstance();
	SAFE_DELETE(app);

	CoUninitialize();


	return 0;
}
