# qyfc
This is a windows C++ library, which is used to make desktop programs.

# Background

In the past, I used MFC to develop desktop applications, but MFC has some trouble. We need to do a lot of work. In character conversion, there is no good multilingual solution, and can not display PNG images well, and so on.

So I made a simple library in my free time, it does not depend on MFC, and is smaller. 


# Install

Minimum Version：Visual studio 2013

# Usage

## Start up


  CApp  is inherited from QYApp
	
	CApp *app = nullptr;
	
  Create a unique CApp instance
  
	app = CApp::instance();
  
  You can change the default theme, such as color, font, size
  
	app->InitTheme();
  
  Initialize global variables
	
  	app->InitInstance(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

  Set layout file root directory

	CApp::bindLayoutPath(std::string(CApp::getAppPath() + "layout\\").c_str());
  
  Set image file root directory
  
	CApp::bindImagePath(std::string(CApp::getAppPath() + "image\\").c_str());
  
  Set language file root directory
  
	QYLanguage::instance()->setLangDir(std::string(CApp::getAppPath() + "lan").c_str());
  
  Add language id
  
	QYLanguage::instance()->addLang("en");
	QYLanguage::instance()->addLang("zh");
  
  Set current language id
  
	QYLanguage::instance()->setLang("zh");

  It should be necessary to package the picture file and use it,

	#if _DEBUG
		QYImageManager::instance()->packImages(QYApp::getAppPath() + "image", std::string("image.dat"));
	#endif
		QYImageManager::instance()->loadImages(CApp::m_imagePath + "image.dat");

  MainDlg  is inherited from QYDialog 
  
	MainDlg *dlg = new MainDlg;
	if (1 != dlg->DoModal(nullptr))
	{
		goto EXIT_MAIN;
	}

EXIT_MAIN:
	
	SAFE_DESTROY_WINDOW_PTR(dlg);

	app->ExitInstance();
	SAFE_DELETE(app);

## Configure layout in XML file

	  <?xml version="1.0" encoding="utf-8"?>
	<UI>
	  <group id="demo" text="QYFC-DEMO" size="1024,760">
	    <static  id="text" text="STATIC_TEXT" rel1="0.0,0" rel2="0.0,0.0" offset1="20,20" offset2="100,44" />
	    <edit  id="edit1" leftto="text RIGHT" offset1="20,20" offset2="200,44" />
	    <edit  id="edit2"   leftto="edit1 RIGHT" offset1="20,20" offset2="200,44" />
	</group> 
	</UI>

## Show window by  XML file

	BOOL MainDlg::OnInitDialog()
	{
		QYDialog::OnInitDialog();

		if (!loadLayout("demo.xml", "demo"))
		{
			EndDialog(IDCANCEL);
			return false;
		}

		QYEdit *ed = (QYEdit*)getObjectPart("edit1");
		ed->setText("");

		ed = (QYEdit*)getObjectPart("edit2");
		ed->SetReadOnly(TRUE);
		ed->setText("readonly editbox");

		return TRUE;
	}


# win-ffplay

	This is a media player, GUI for QYFC and media engine for ffmpeg.
	
	We also can play  private media file for myself for a small change.
	
	For  example "readMyMediaFile".

 
	
# Author

Tom Chan - tmgycjl@163.com


