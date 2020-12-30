#include "QYDPI.h"


QYDPI::QYDPI()
{
		m_nScaleFactor = 0;
		m_nScaleFactorSDA = 0;
		m_Awareness = PROCESS_DPI_UNAWARE;

		static HINSTANCE hUser32Instance = ::LoadLibrary(L"User32.dll");
		static HINSTANCE hShcoreInstance = ::LoadLibrary(L"Shcore.dll");
		if (hUser32Instance != NULL) {
			fnSetProcessDPIAware = (FNSETPROCESSDPIAWARE) ::GetProcAddress(hUser32Instance, "SetProcessDPIAware");
		}
		else {
			fnSetProcessDPIAware = NULL;
		}

		if (hShcoreInstance != NULL) {
			fnSetProcessDpiAwareness = (FNSETPROCESSDPIAWARENESS) ::GetProcAddress(hShcoreInstance, "SetProcessDpiAwareness");
			fnGetDpiForMonitor = (FNGETDPIFORMONITOR) ::GetProcAddress(hShcoreInstance, "GetDpiForMonitor");
		}
		else {
			fnSetProcessDpiAwareness = NULL;
			fnGetDpiForMonitor = NULL;
		}

		if (fnGetDpiForMonitor != NULL) {
			UINT     dpix = 0, dpiy = 0;
			HRESULT  hr = E_FAIL;
			POINT pt = { 1, 1 };
			HMONITOR hMonitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
			hr = fnGetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpix, &dpiy);
			SetScale(dpix);
		}
		else {
			UINT     dpix = 0;
			HDC hDC = ::GetDC(::GetDesktopWindow());
			dpix = GetDeviceCaps(hDC, LOGPIXELSX);
			::ReleaseDC(::GetDesktopWindow(), hDC);
			SetScale(dpix);
		}

		SetAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	}

int  QYDPI::Scale(int x)
	{
		if (m_Awareness == PROCESS_DPI_UNAWARE) return x;
		if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE) return MulDiv(x, m_nScaleFactorSDA, 100);
		return MulDiv(x, m_nScaleFactor, 100); // PROCESS_PER_MONITOR_DPI_AWARE
	}

UINT QYDPI::GetScale()
	{
		if (m_Awareness == PROCESS_DPI_UNAWARE) return 100;
		if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE) return m_nScaleFactorSDA;
		return m_nScaleFactor;
	}

void QYDPI::ScaleRect(__inout RECT *pRect)
	{
		pRect->left = Scale(pRect->left);
		pRect->right = Scale(pRect->right);
		pRect->top = Scale(pRect->top);
		pRect->bottom = Scale(pRect->bottom);
	}

void QYDPI::ScalePoint(__inout POINT *pPoint)
	{
		pPoint->x = Scale(pPoint->x);
		pPoint->y = Scale(pPoint->y);
	}

void QYDPI::OnDPIChanged(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		SetScale(LOWORD(wParam));
		RECT* const prcNewWindow = (RECT*)lParam;
		::SetWindowPos(hWnd,
			NULL,
			prcNewWindow->left,
			prcNewWindow->top,
			prcNewWindow->right - prcNewWindow->left,
			prcNewWindow->bottom - prcNewWindow->top,
			SWP_NOZORDER | SWP_NOACTIVATE);
	}

BOOL QYDPI::SetAwareness(PROCESS_DPI_AWARENESS value)
	{
		if (fnSetProcessDpiAwareness != NULL) {
			HRESULT Hr = E_NOTIMPL;
			Hr = fnSetProcessDpiAwareness(value);
			if (Hr == S_OK) {
				m_Awareness = value;
				return TRUE;
			}
			else {
				return FALSE;
			}
		}
		else {
			if (fnSetProcessDPIAware) {
				BOOL bRet = fnSetProcessDPIAware();
				if (bRet) m_Awareness = PROCESS_SYSTEM_DPI_AWARE;
				return bRet;
			}
		}
		return FALSE;
	}


void QYDPI::SetScale(__in UINT iDPI)
	{
		m_nScaleFactor = MulDiv(iDPI, 100, 96);
		if (m_nScaleFactorSDA == 0) m_nScaleFactorSDA = m_nScaleFactor;
	}

