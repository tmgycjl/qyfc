#pragma once


class CApp :
	public QYApp
{
public:
	
	~CApp();
   static  void InitTheme();
   static CApp *instance(void);
   UINT MessageBox(QYWindow* pParent, const char* strMessage, UINT nFlag);
   std::string floatArrayToString(const float *pF, int size);
   void stringToFloatArray(float *pF, int size,const std::string &str);
   QYString m_strUser;
   // QYString m_strLoginDate;
   int      m_nUserRight;

   int    _port = 8089;

private:
	CApp();
	static CApp *m_instance;
	QYMessageBox *m_msgBox = nullptr;
  
};

extern CApp *app;

