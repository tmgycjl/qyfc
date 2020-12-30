#pragma once
#include "QYListCtrl.h"
#include "QYDateTime.h"
#include "QYDropList.h"
#include "QYSize.h"

#define  NO_DATE     -1

class QYUI_EXPORT_CLASS  QYDateTableWeekItem
{
public:
	QYDateTableWeekItem(const TCHAR *text)
	{
		value = -1;
		strText = text;
	}
	QYDateTableWeekItem() 
	{
		value = -1;
	}
	~QYDateTableWeekItem() { ; }

	 int value;
	
	QYString strText;
};

class QYUI_EXPORT_CLASS  QYDateTableDayItem
{
public:
	QYDateTableDayItem(TCHAR *text)
	{
		nValue = -1;
		color = -1;
		strValue = text;
	}
	QYDateTableDayItem()
	{
		color = -1;
		nValue = -1;
		strValue = L"1";
	}
	~QYDateTableDayItem() { ; }

	  int nValue;
	  QYDate date;
	 QYString strValue;
	 COLORREF color;
	 COLORREF textColor;
};

struct DayOfWeekStruct
{
	std::string   lang_id;
	TCHAR desc[DAYS_IN_WEEK][20];
};

class QYUI_EXPORT_CLASS QYDateTable :public QYWindow
{
public:
	QYDateTable(BOOL bPopFlag = FALSE);
	virtual ~QYDateTable();

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL Show(int nCmdShow);
	void SetTiltle(LPCTSTR strTitle);
	QYSize GetAppropriateSize(int width,int height);
	QYSize GetDefaultSize();
	void SetDate(WORD year = 0, WORD month = 0, WORD day = 0);
	BOOL SetDateColor(int year,int month,int day,COLORREF color);
	BOOL SetDateData(int year, int month, int day, DWORD_PTR data);
	QYDate* GetCheckDate(){ return &m_checkDate; }
	void AddMarkDate(QYDate *date);
	void ClearMarkDate();
	BOOL Create(DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWndName,
		DWORD dwStyle,
		QYRect rect,
		HWND hParentWnd = nullptr);
	void Reset();
	void TrackPopup(QYPoint point, int width, int pos, QYWindow *pWnd,QYDate date);
	static void SetLanguge(std::string &langID);
	virtual LRESULT OnPaint();
	virtual LRESULT OnLButtonUp(UINT nFlags, QYPoint point);
	virtual LRESULT OnMouseMove(UINT nFlags, QYPoint point);
	virtual LRESULT OnMouseLeave(WPARAM, LPARAM);
	virtual LRESULT OnSize(UINT nType, int cx, int cy);
	virtual LRESULT OnSetCursor(WPARAM wparam,LPARAM lparam);
	virtual LRESULT OnLButtonDown(UINT nFlags, QYPoint point);
	virtual void updateText();
private:
	void DrawWeekPane(QYDC *pDC, QYRect *rect);
	void DrawDayPane(QYDC *pDC, QYRect *rect);
	void DrawNowDatePane(QYDC *pDC, QYRect *rect);
	void UpdateDate();
	void DrawTopPane(HDC hDC,QYRect *rectTopPane);
	void GotoLastMonth();
	void GotoNextMonth();
	enum 
	{
		LAST_MONTH_BUTTON = 0,
		NEXT_MONTH_BUTTON,
		TITLE_BUTTON
	};
	BOOL   m_bPopFlag;
	QYDate m_SelectDate;
	QYDate m_checkDate;
	QYDate m_NowDate;
	int  m_nOperPaneHeight;
	int  m_nWeekPaneHeight;
	QYRect m_rectLastMonth;
	QYRect m_rectNextMonth;
	QYRect m_rectTitleButton;
	QYRect m_rectItem;
	int   m_nDownButtonID;
	int   m_nHoverButtonID;
	unsigned int m_CurDateTime;
	QYFont  *m_pTitleFont;
	int  m_titleButtonWidth;
	QYString m_TitleText;
	QYDropList  *m_pDropList;
	short       m_HoverItem;
	short       m_CheckItem;
	QYDateTableWeekItem m_WeekArray[DAYS_IN_WEEK];
	QYDateTableDayItem m_DayArray[WEEK_NUMS_IN_MONTH][DAYS_IN_WEEK];
	QYSize m_sizeItem;
	QYWindow  *m_pSourceWnd;
	QYWindowShadow *m_pShadow;
	std::vector<QYDate*> m_vectorMarkDate;
	static int m_LangugeIndex;
	static DayOfWeekStruct m_arrayDayOfWeek[];
	static const char* m_textIDDayOfWeek[DAYS_IN_WEEK];
	static QYString m_textDayOfWeek[DAYS_IN_WEEK];
};

