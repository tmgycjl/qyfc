#pragma once
#ifdef _DEBUG
//#include "vld.h"
#endif
#include <Windows.h>
#include <stdio.h>


#if defined QYUI_EXPORTS
#define QYUI_EXPORT_CLASS		 __declspec(dllexport)
#define QYFC_EXPORT_CLASS		 __declspec(dllexport)
#elif defined __BUILD_LIB
#define QYUI_EXPORT_CLASS
#define QYFC_EXPORT_CLASS		 
#else
#define QYFC_EXPORT_CLASS	 __declspec(dllimport)
#define QYUI_EXPORT_CLASS		 __declspec(dllimport)
#endif

typedef BOOL(WINAPI *PQY_REFLEX)(WPARAM wparam,LPARAM lparam);

#define  QY_MSG_CALLBACK      static BOOL WINAPI


#ifdef UNICODE
#define SafeStrcpy	wcscpy_s
#define SafeStrcat	wcscat_s
#define SafeSprintf swprintf_s
#define SafeA2I		_wtoi
#define SafeA2F		_wtof
#define SafeA2I64		_wtoi64
#define SafeI2A		_itow_s
#define	SafeStrcmp  wcscmp
#define SafeStrlen  wcslen
#define SafeSscanf  swscanf_s
#define SafeStrChr   wcschr
#define SafeStrrChr   wcsrchr
#define SafeStrStr   wcsstr
#else
#define SafeStrcpy  strcpy_s
#define SafeStrcat	strcat_s
#define SafeSprintf sprintf_s
#define SafeA2I		atoi
#define SafeA2F		atof
#define SafeA2I64		_atoi64
#define SafeI2A		itoa

#define	SafeStrcmp  strcmp
#define SafeStrlen  strlen
#define SafeSscanf  sscanf
#define SafeStrChr   strchr
#define SafeStrrChr   strrchr
#define SafeStrStr   strstr
#endif


#define IS_VALID(p)		(NULL != p)

#define SAFE_DELETE(p)		if(p != NULL){  delete p;	    p = NULL; }

#define SAFE_DELETE_ARRAY(p)		if(p != NULL){  delete[]p;	    p = NULL; }

#define SAFE_RELEASE(p)		if(p != NULL){	p->Release();   p = NULL; }

#define SAFE_RELEASE_DELETE(p)		if(p != NULL){	p->Release();  delete p;  p = NULL; }

#define SAFE_CLOSE(p)		if(p != NULL){  ::CloseHandle(p); p = NULL; }

#define SAFE_CLOSE_HANDLE SAFE_CLOSE

#define  LOGMSG  Log

#define SAFE_DELETE_PTR(ptr)		if((ptr) != NULL){  delete[] (ptr);	    (ptr) = NULL; }

#define SAFE_CLOSESOCKET(s) if(s != INVALID_SOCKET){closesocket(s);s = INVALID_SOCKET;}


#define SAFE_CLOSE_FILE(f)	if ( f != NULL ){fclose(f);f=NULL;}

#define		MAX(a, b)			((a)>(b))?(a):(b)

#define		MIN(a, b)			((a)>(b))?(b):(a)

#define     MB        1024 * 1024

#define  SAFE_DESTROY_WINDOW(hwnd)     if(nullptr != hwnd){::DestroyWindow(hwnd);hwnd = nullptr;}
#define  SAFE_DESTROY_WINDOW_PTR(p)     if(nullptr != p){::DestroyWindow(p->m_hWnd);delete p; p = nullptr;}
#define  SAFE_DELETE_OBJECT(obj)  if(nullptr != obj){int ret = ::DeleteObject(obj);obj = nullptr;return ret;}

#define  LOSHORT(l)   (short)(l&0xffff)
#define  HISHORT(l)   (short)((l>>16)&0xffff)

//#define  SAFE_DELETE_OBJECT_PTR(obj)  if(nullptr != obj){obj.DeleteObject();}
#define  CLASS_NAME_BUTTON            L"Button"
#define  CLASS_NAME_EDIT              L"Edit"
#define  CLASS_NAME_COMBOBOX            L"ComboBox"
#define  CLASS_NAME_COMBOLBOX            L"ComboLBox"
#define  CLASS_NAME_STATIC            L"Static"
#define  CLASS_NAME_LISTBOX           L"ListBox"
#define  CLASS_NAME_LISTCTRL          L"SysListView32"
#define  CLASS_NAME_PROGRESS          L"msctls_progress32"
#define  QY_WM_FISRT    WM_USER + 20000
#define  QY_WM_DATETABLE_DOWN_ITEM    (QY_WM_FISRT + 1)
#define  QY_WM_DATETABLE_CHANGED    (QY_WM_FISRT + 2)
#define  QY_WM_LISTCTRL_CHECK_ALL    (QY_WM_FISRT + 3)
#define  QY_WM_COLMUN_WIDTH_CHANGED    (QY_WM_FISRT + 4)
#define  QY_WM_LISTCTRL_ITEM_DBCLICK    (QY_WM_FISRT + 5)
#define  QY_WM_TREECTRL_CHECK_ALL    (QY_WM_FISRT + 6)
#define  QY_WM_TREECTRL_ITEM_DBCLICK    (QY_WM_FISRT + 7)
#define  QY_WM_TREECTRL_ITEM_CLICK    (QY_WM_FISRT + 8)
#define  QY_WM_LISTCTRL_ITEM_CLICK    (QY_WM_FISRT + 9)
#define  QY_WM_LISTCTRL_ITEM_RCLICK    (QY_WM_FISRT + 10)
#define  QY_WM_TREECTRL_ITEM_RCLICK    (QY_WM_FISRT + 11)
#define  QY_WM_TABCTRL_ITEM_SELECTED    (QY_WM_FISRT + 12)
#define  QY_WM_LAST_PAGE                (QY_WM_FISRT + 13)  
#define  QY_WM_NEXT_PAGE                (QY_WM_FISRT + 14)
#define  QY_WM_FIRST_PAGE               (QY_WM_FISRT + 15)
#define  QY_WM_END_PAGE                 (QY_WM_FISRT + 16)
#define  QY_WM_LISTCTRL_CLICK_COLUMN      (QY_WM_FISRT + 17)
#define  QY_WM_TOOLBAR_ITEM_SELECTED    (QY_WM_FISRT + 18)
#define  QY_WM_DROPLIST_DELETE_STRING    (QY_WM_FISRT + 19)
#define  QY_WM_TIMETABLE_EDIT    (QY_WM_FISRT + 20)
#define  QY_WM_SUBMENU_HIDED    (QY_WM_FISRT + 21)
#define  QY_WM_DROPDATA         (QY_WM_FISRT + 22)
#define  QY_WM_DATETABLE_SELECTED    (QY_WM_FISRT + 23)
#define  QY_WM_TREECTRL_ITEM_CHECKED    (QY_WM_FISRT + 24)
#define  QY_WM_FOLDEREDIT_CHANGED        (QY_WM_FISRT + 25)
#define   QY_WM_BUTTON_DOWN    (QY_WM_FISRT + 26)
#define   QY_WM_BUTTON_UP       WM_COMMAND
#define   QY_WM_EDIT_CHANGED       (QY_WM_FISRT + 27)
#define   	QY_WM_CLOSE_POP_WINDOW (QY_WM_FISRT + 28)
#define  QY_BUTTON_FIRST_ID           100
class QYApp;


#define QY_IMAGE_PNG          L"PNG"
#define  QY_IMAGE_JPG          L"JPG"
#define  QY_IMAGE_BMP          RT_BITMAP
#define  QY_IMAGE_GIF          L"GIF"

enum enumImageStatus
{
	IMAGE_STATUS_NORMAL = 0,
	IMAGE_STATUS_HOVER ,
	IMAGE_STATUS_PRESS ,
	IMAGE_STATUS_FOCUS ,
	IMAGE_STATUS_MAX
};

enum enumRGBFormat
{
	QY_RGB = 0,
	QY_RGBA,
	QY_BGR,
	QY_BGRA
};


enum QY_CONTROL_TYPE
{
	QY_CONTROL_TYPE_INVALID = -1,
	QY_CONTROL_TYPE_WINDOW ,
	QY_CONTROL_TYPE_STATIC,
	QY_CONTROL_TYPE_EDIT,
	QY_CONTROL_TYPE_BUTTON,
	QY_CONTROL_TYPE_CHECK_BUTTON,
	QY_CONTROL_TYPE_SWITCH_BUTTON,
	QY_CONTROL_TYPE_RADIO_BUTTON,
	QY_CONTROL_TYPE_COMBOBOX,
	QY_CONTROL_TYPE_DATETIME,
	QY_CONTROL_TYPE_DATETABLE,
	QY_CONTROL_TYPE_IPADDRESS,
	QY_CONTROL_TYPE_FOLDER_EDIT,
	QY_CONTROL_TYPE_FOLDER_LISTCTRL,
	QY_CONTROL_TYPE_LISTCTRL,
    QY_CONTROL_TYPE_WIDGET,
	QY_CONTROL_TYPE_WIDGETLIST,
	QY_CONTROL_TYPE_TOOLBAR,
	QY_CONTROL_TYPE_TREE,
	QY_CONTROL_TYPE_RECT,
	QY_CONTROL_TYPE_SWALLOW,
	QY_CONTROL_TYPE_OTHERS
};

#define MAX_IPV4_ADDRESS_BLOCK_SIZE    4
#define MAX_IPV4_ADDRESS_SIZE    16
#define MAX_IPV4_BLOCK_COUNT    4
#define MAX_WINDOW_TEXT_LENGTH         1024
#define MAX_WEEK_TEXT_LENGTH         128
#define BOTTOM_TO_PARENT        0xffff
#define QY_CALLBACK_EVENT             "event"

