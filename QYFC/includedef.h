#pragma  once

#include "resource.h"
#include <assert.h>
#include <comdef.h>
#include <gdiplus.h>
#include <assert.h>

using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

#pragma warning(disable:4251)
#pragma warning(disable:4244)
#pragma warning(disable:4018)
#pragma warning(disable:4172)


#define  QY_MODULE_NAME          L"QYFC.dll"


#define SET_TEXT_ID(textID,text,strText)  textID = text; \
const char *srcText = QYLanguage::instance()->getText(textID.c_str()); \
int dstSize = (strlen(srcText) + 1)*sizeof(TCHAR); \
TCHAR *dstText = new TCHAR[dstSize]; \
memset(dstText, 0, dstSize); \
QYString::SafeUTF82W(dstText, dstSize, srcText); \
strText = dstText; \
delete[] dstText; 

#define SET_TEXT(strText,text)  int dstSize = (strlen(text) + 1)*sizeof(TCHAR); \
TCHAR *dstText = new TCHAR[dstSize]; \
memset(dstText, 0, dstSize); \
QYString::SafeUTF82W(dstText, dstSize, text); \
strText = dstText; \
delete[] dstText; 

