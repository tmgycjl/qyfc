#include "QYDwm.h"


QYDwm::QYDwm()
	{
		static HINSTANCE hDwmInstance = ::LoadLibrary(L"dwmapi.dll");
		if (hDwmInstance != NULL) {
// 			fnDwmEnableComposition = (FNDWMENABLECOMPOSITION) ::GetProcAddress(hDwmInstance, "DwmEnableComposition");
// 			fnDwmIsCompositionEnabled = (FNDWNISCOMPOSITIONENABLED) ::GetProcAddress(hDwmInstance, "DwmIsCompositionEnabled");
// 			fnDwmEnableBlurBehindWindow = (FNENABLEBLURBEHINDWINDOW) ::GetProcAddress(hDwmInstance, "DwmEnableBlurBehindWindow");
// 			fnDwmExtendFrameIntoClientArea = (FNDWMEXTENDFRAMEINTOCLIENTAREA) ::GetProcAddress(hDwmInstance, "DwmExtendFrameIntoClientArea");
			fnDwmSetWindowAttribute = (FNDWMSETWINDOWATTRIBUTE) ::GetProcAddress(hDwmInstance, "DwmSetWindowAttribute");
		}
		else 
		{
// 			fnDwmEnableComposition = NULL;
// 			fnDwmIsCompositionEnabled = NULL;
// 			fnDwmEnableBlurBehindWindow = NULL;
// 			fnDwmExtendFrameIntoClientArea = NULL;
			fnDwmSetWindowAttribute = NULL;
		}
	}

#if 0


BOOL QYDwm::IsCompositionEnabled() const
	{
		HRESULT Hr = E_NOTIMPL;
		BOOL bRes = FALSE;
		if (fnDwmIsCompositionEnabled != NULL) Hr = fnDwmIsCompositionEnabled(&bRes);
		return SUCCEEDED(Hr) && bRes;
	}

BOOL QYDwm::EnableComposition(UINT fEnable)
	{
		BOOL bRes = FALSE;
		if (fnDwmEnableComposition != NULL) bRes = SUCCEEDED(fnDwmEnableComposition(fEnable));
		return bRes;
	}

BOOL QYDwm::EnableBlurBehindWindow(HWND hWnd)
	{
		BOOL bRes = FALSE;
		if (fnDwmEnableBlurBehindWindow != NULL) {
			DWM_BLURBEHIND bb = { 0 };
			bb.dwFlags = DWM_BB_ENABLE;
			bb.fEnable = TRUE;
			bRes = SUCCEEDED(fnDwmEnableBlurBehindWindow(hWnd, &bb));
		}
		return bRes;
	}

BOOL QYDwm::EnableBlurBehindWindow(HWND hWnd, CONST DWM_BLURBEHIND& bb)
	{
		BOOL bRes = FALSE;
		if (fnDwmEnableBlurBehindWindow != NULL) {
			bRes = SUCCEEDED(fnDwmEnableBlurBehindWindow(hWnd, &bb));
		}
		return bRes;
	}

BOOL QYDwm::ExtendFrameIntoClientArea(HWND hWnd, CONST DWM_MARGINS& Margins)
	{
		BOOL bRes = FALSE;
		if (fnDwmEnableComposition != NULL) bRes = SUCCEEDED(fnDwmExtendFrameIntoClientArea(hWnd, &Margins));
		return bRes;
	}
#endif
BOOL QYDwm::SetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
	{
		BOOL bRes = FALSE;
		if (fnDwmSetWindowAttribute != NULL) bRes = SUCCEEDED(fnDwmSetWindowAttribute(hwnd, dwAttribute, pvAttribute, cbAttribute));
		return bRes;
	}