#pragma once

#include "qydefine.h"


#ifndef WM_DPICHANGED
#   define WM_DPICHANGED       0x02E0
#endif


#ifndef _SHELLSCALINGAPI_H_
typedef enum _PROCESS_DPI_AWARENESS {
	PROCESS_DPI_UNAWARE = 0,
	PROCESS_SYSTEM_DPI_AWARE = 1,
	PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWARENESS;

typedef enum _MONITOR_DPI_TYPE {
	MDT_EFFECTIVE_DPI = 0,
	MDT_ANGULAR_DPI = 1,
	MDT_RAW_DPI = 2,
	MDT_DEFAULT = MDT_EFFECTIVE_DPI
} Monitor_DPI_Type;
#endif // _SHELLSCALINGAPI_H_

class QYUI_EXPORT_CLASS QYDPI
{
public:
	typedef BOOL(WINAPI *FNSETPROCESSDPIAWARE)(VOID);
	typedef HRESULT(WINAPI *FNSETPROCESSDPIAWARENESS)(PROCESS_DPI_AWARENESS);
	typedef HRESULT(WINAPI *FNGETDPIFORMONITOR)(HMONITOR, Monitor_DPI_Type, UINT*, UINT*);

	FNSETPROCESSDPIAWARE fnSetProcessDPIAware; // vista,win7
	FNSETPROCESSDPIAWARENESS fnSetProcessDpiAwareness; // win8+
	FNGETDPIFORMONITOR fnGetDpiForMonitor; //

	QYDPI();

	PROCESS_DPI_AWARENESS GetAwareness(){return m_Awareness;}

	int  Scale(int x);

	UINT GetScale();

	void ScaleRect(__inout RECT *pRect);

	void ScalePoint(__inout POINT *pPoint);

	void OnDPIChanged(HWND hWnd, WPARAM wParam, LPARAM lParam);
private:
	UINT m_nScaleFactor;
	UINT m_nScaleFactorSDA;
	PROCESS_DPI_AWARENESS m_Awareness;

	BOOL SetAwareness(PROCESS_DPI_AWARENESS value);

	void SetScale(__in UINT iDPI);
};
