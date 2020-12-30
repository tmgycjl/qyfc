#include "QYWaitCursor.h"


QYWaitCursor::QYWaitCursor()
{
	::SetCursor(::LoadCursor(nullptr, IDC_WAIT));
}


QYWaitCursor::~QYWaitCursor()
{
	::SetCursor(::LoadCursor(nullptr, IDC_ARROW));
}
