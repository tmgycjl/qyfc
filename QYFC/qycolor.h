#pragma  once

typedef struct tagQYThemeStruct
{
	TCHAR DLG_FONT_TYPE[256];
	TCHAR   MS_YAHEI_FONT_TYPE[256];
	TCHAR   CAPTION_FONT_TYPE[256];
	TCHAR   CAPTION_FONT_TYPE_2[256];

	int  DEFAULT_FONT_WIDTH;
	int  CAPTION_FONT_WIDTH;
	int  CAPTION_FONT_HEIGHT;
	int  BUTTON_FONT_SIZE;
	int  DLG_FONT_SIZE;

	BOOL CAPTION_BK_GRADIENT;

	int MS_YAHEI_FONT_WEIGHT;
	int MS_YAHEI_FONT_SIZE;
	int CAPTION_FONT_SIZE;
	int CAPTION_FONT_SIZE_2;
	int  TITLE_FONT_SIZE;
	int  DEFAULT_CTRL_HEIGHT;
	int  DEFAULT_CAPTION_HEIGHT;
	int   DIALOG_CAPTION_HEIGHT;

	int   WINDOW_BORDER_WIDTH;
	int   WINDOW_SHADOW_SIDE_LEN_LEFT_TOP;
	int  WINDOW_SHADOW_SIDE_LEN_RIGHT_BOTTOM;
	int   DEFAULT_TAB_HEIGHT;
	int     SYS_BOX_PEN_WIDTH = 1;

	int  SMALL_ARROW_PEN_WIDTH;
	int   LARGE_ARROW_PEN_WIDTH;

	int  CHECKBOX_WIDTH;
	int   CHECKBOX_HEIGHT;
	int   ID_CHECK_SELECT_ALL;
	int   SCROLLBAR_WIDTH;
	int   PAGE_PANE_HEIGHT;
	COLORREF   DLG_CAPTION_COLOR;
	COLORREF   CLIENT_COLOR;
	COLORREF   DLG_CLIENT_COLOR;
	COLORREF   CAPTION_COLOR;

	COLORREF  BORDER_PEN_COLOR_ACTIVE;
	COLORREF   BORDER_PEN_COLOR_UNACTIVE;
	COLORREF  DLG_BORDER_PEN_COLOR_ACTIVE;
	COLORREF   DLG_BORDER_PEN_COLOR_UNACTIVE;

	COLORREF   SPLITTER_COLOR_NORMAL;
	COLORREF   SPLITTER_COLOR_HOVER;
	COLORREF   SPLITTER_COLOR_DOWN;

	COLORREF   CAPTION_TEXT_COLOR_ACTIVE;
	COLORREF   CAPTION_TEXT_COLOR;
	COLORREF   DLG_BUTTON_PANE_BK_COLOR;
	COLORREF   BUTTON_TEXT_COLOR_NORMAL;
	COLORREF   BUTTON_TEXT_COLOR_DISABLE;

	COLORREF   BUTTON_BK_COLOR_NORMAL;
	COLORREF   BUTTON_BK_COLOR_HOVER;
	COLORREF   BUTTON_BK_COLOR_DOWN;
	COLORREF  BUTTON_BK_COLOR_FOCUS;
	COLORREF  BUTTON_BK_COLOR_DISABLE;
	COLORREF   BUTTON_BORDER_COLOR_NORMAL;
	COLORREF   BUTTON_BORDER_COLOR_HOVER;
	COLORREF   BUTTON_BORDER_COLOR_FOCUS;
	COLORREF   BUTTON_BORDER_COLOR_DOWN;
	COLORREF   BUTTON_BORDER_COLOR_DISABLE;
	COLORREF  EDIT_SELECT_COLOR;
	COLORREF   EDIT_BK_COLOR_NORMAL;
	COLORREF  EDIT_BK_COLOR_HOVER;
	COLORREF   EDIT_BK_COLOR_FOCUS;
	COLORREF  EDIT_BK_COLOR_READONLY;

	COLORREF   EDIT_BORDER_COLOR_HOVER;

	COLORREF   CLOSE_BOX_BK_COLOR_NORMAL;
	COLORREF   CLOSE_BOX_BK_COLOR_DOWN;
	COLORREF   CLOSE_BOX_BK_COLOR_HOT;

	COLORREF   SYS_BOX_PEN_COLOR_HOT;
	COLORREF   SYS_BOX_PEN_COLOR_NORMAL;
	COLORREF   SYS_BOX_PEN_COLOR_NORMAL_ACTIVE;
	
	COLORREF   SYS_BOX_PEN_COLOR_DISABLE;

	COLORREF   SYS_BOX_BK_COLOR_NORMAL;
	COLORREF   SYS_BOX_BK_COLOR_DOWN;
	COLORREF   SYS_BOX_BK_COLOR_HOT;


	COLORREF   EDIT_BORDER_COLOR_FOCUS;
	COLORREF   EDIT_BORDER_COLOR_NORMAL;

	COLORREF   COMBOBOX_ARROW_PEN_COLOR_NORMAL;
	COLORREF   COMBOBOX_ARROW_BK_COLOR_HOVER;
	COLORREF   COMBOBOX_ARROW_BK_COLOR_DOWN;


	COLORREF   LISTCTRL_BK_COLOR_NORMAL;
	COLORREF   LISTCTRL_BK_COLOR_NORMAL2;
	COLORREF   LISTCTRL_BK_COLOR_HOVER;
	COLORREF   LISTCTRL_BK_COLOR_SELECT;
	COLORREF   LISTCTRL_BK_COLOR_LOSE_FOCUS;
	COLORREF   LISTCTRL_BORDER_COLOR;
	COLORREF   LISTCTRL_SEPARATED_COLOR;

	COLORREF   DROPLIST_BORDER_COLOR;

	COLORREF   LISTBOX_BK_COLOR_NORMAL;
	COLORREF   LISTBOX_BK_COLOR_SELECT;
	COLORREF   LISTBOX_BORDER_COLOR;

	COLORREF   MENU_BK_COLOR_NORMAL;
	COLORREF   MENU_BK_COLOR_SELECT;

	COLORREF   TOOLTIP_BK_COLOR_NORMAL;
	COLORREF   TOOLTIP_BORDER_COLOR;

	COLORREF   HEADERCTRL_BK_COLOR_NORMAL;
	COLORREF   HEADERCTRL_BK_COLOR_HOVER;
	COLORREF   HEADERCTRL_BK_COLOR_DOWN;
	COLORREF   HEADERCTRL_BORDER_COLOR;
	COLORREF  HEADERCTRL_SEPARATED_COLOR;

	COLORREF   PAGEPANE_BK_COLOR_NORMAL;

	COLORREF   TAB_BOTTOM_SPLIT_COLOR;
	COLORREF   TAB_BK_COLOR_HOT;
	COLORREF   TAB_BK_COLOR_SEL;
	COLORREF   TAB_BK_COLOR_NORMAL;
	COLORREF   BUTTON_BK_COLOR_SEL;

	COLORREF  ITEM_BK_COLOR_HOT;
	COLORREF  ITEM_BK_COLOR_SEL;
	COLORREF  ITEM_BORBER_COLOR_SEL;

	COLORREF   DATETABLE_CURRENT_DAY_BK_COLOR;
	COLORREF  DATETABLE_CURRENT_DAY_BORDER_COLOR;
	COLORREF  DATETABLE_MARK_DAY_BK_COLOR;
	COLORREF  DATETABLE_HOVER_DAY_BORDER_COLOR;
	COLORREF  DATETABLE_CHECK_DAY_BORDER_COLOR;

	COLORREF   SCROLLBAR_COLOR_NORMAL;
	COLORREF  SCROLLBAR_COLOR_SELECT;
	COLORREF  SCROLLBAR_COLOR_HOVER;
	COLORREF  SCROLLBAR_ARROW_COLOR_HOVER;
	COLORREF  SCROLLBAR_ARROW_COLOR_NORMAL;
	COLORREF   SCROLLBAR_ARROW_COLOR_DOWN;

	COLORREF   TIMETABLE_HEADER_COLOR;
	COLORREF   TIMETABLE_ITEM_COLOR;

	COLORREF   PROGRESS_BK_COLOR;
	COLORREF   PROGRESS_PROCESS_COLOR;

	COLORREF   TEXT_COLOR_NORMAL;
	COLORREF  TEXT_COLOR_HOVER;
	COLORREF  TEXT_COLOR_SELECT;
	COLORREF   TEXT_COLOR_DISABLE;

	COLORREF  TAB_TEXT_COLOR_SELECT;
	COLORREF   TAB_TEXT_COLOR_NORMAL;

	struct tagQYThemeStruct()
	{
		SafeStrcpy(DLG_FONT_TYPE, L"MS Shell Dlg 2");
		SafeStrcpy(MS_YAHEI_FONT_TYPE, L"΢���ź�");
		SafeStrcpy(CAPTION_FONT_TYPE, MS_YAHEI_FONT_TYPE);
		SafeStrcpy(CAPTION_FONT_TYPE_2, L"Terminal");

		CAPTION_BK_GRADIENT = TRUE;

		DEFAULT_TAB_HEIGHT = 22;
		DEFAULT_FONT_WIDTH = 6;
		CAPTION_FONT_WIDTH = 6;
		CAPTION_FONT_HEIGHT = 12;
		BUTTON_FONT_SIZE = 16;
		DLG_FONT_SIZE = 16;

		MS_YAHEI_FONT_WEIGHT = FW_NORMAL;
		MS_YAHEI_FONT_SIZE = 17;
		CAPTION_FONT_SIZE = MS_YAHEI_FONT_SIZE;
		CAPTION_FONT_SIZE_2 = 17;
		TITLE_FONT_SIZE = 18;
		DEFAULT_CTRL_HEIGHT = 24;
		DEFAULT_CAPTION_HEIGHT = 30;

		BORDER_PEN_COLOR_UNACTIVE = RGB(200, 200, 200);
		BORDER_PEN_COLOR_ACTIVE = BORDER_PEN_COLOR_UNACTIVE;

		CAPTION_COLOR = RGB(255, 255, 255);
		CLIENT_COLOR = RGB(255, 255, 255);

		DIALOG_CAPTION_HEIGHT = 33;
		WINDOW_BORDER_WIDTH = 1;
		WINDOW_SHADOW_SIDE_LEN_LEFT_TOP = 10;
		WINDOW_SHADOW_SIDE_LEN_RIGHT_BOTTOM = 10;

		DLG_BORDER_PEN_COLOR_ACTIVE = RGB(200, 200, 200);
		DLG_BORDER_PEN_COLOR_UNACTIVE = DLG_BORDER_PEN_COLOR_ACTIVE;

		
		CAPTION_TEXT_COLOR_ACTIVE = RGB(255, 255, 255);
		CAPTION_TEXT_COLOR = RGB(160, 163, 172);
		DLG_BUTTON_PANE_BK_COLOR = RGB(246, 247, 250);
		BUTTON_TEXT_COLOR_NORMAL = TEXT_COLOR_NORMAL;
		BUTTON_TEXT_COLOR_DISABLE = RGB(122, 122, 122);

		BUTTON_BORDER_COLOR_NORMAL = RGB(34, 97, 183);
		BUTTON_BORDER_COLOR_HOVER = BUTTON_BORDER_COLOR_NORMAL;
		BUTTON_BORDER_COLOR_FOCUS = BUTTON_BORDER_COLOR_HOVER;
		BUTTON_BORDER_COLOR_DOWN = BUTTON_BORDER_COLOR_HOVER;
		BUTTON_BORDER_COLOR_DISABLE = RGB(246, 247, 250);

		BUTTON_BK_COLOR_NORMAL = RGB(240, 245, 249);
		BUTTON_BK_COLOR_HOVER = RGB(233, 241, 253);
		BUTTON_BK_COLOR_DOWN = RGB(223, 234, 246);
		BUTTON_BK_COLOR_FOCUS = BUTTON_BK_COLOR_NORMAL;
		BUTTON_BK_COLOR_DISABLE = RGB(204, 204, 204);

		EDIT_SELECT_COLOR = RGB(122, 200, 255);
		EDIT_BK_COLOR_NORMAL = RGB(255, 255, 255);
		EDIT_BK_COLOR_HOVER = EDIT_BK_COLOR_NORMAL;
		EDIT_BK_COLOR_FOCUS = EDIT_BK_COLOR_NORMAL;
		EDIT_BK_COLOR_READONLY = CLIENT_COLOR;

		//   EDIT_BORDER_COLOR_NORMAL = BUTTON_BORDER_COLOR_NORMAL;
		EDIT_BORDER_COLOR_HOVER = RGB(0, 0, 0);
		//   EDIT_BORDER_COLOR_FOCUS = BORDER_PEN_COLOR_ACTIVE;

		CLOSE_BOX_BK_COLOR_NORMAL = CAPTION_COLOR;
		CLOSE_BOX_BK_COLOR_DOWN = RGB(166, 42, 21);
		CLOSE_BOX_BK_COLOR_HOT = RGB(212, 64, 39);

		SYS_BOX_PEN_COLOR_HOT = RGB(255, 255, 255);
		SYS_BOX_PEN_COLOR_NORMAL = RGB(160, 163, 172);
		SYS_BOX_PEN_COLOR_NORMAL_ACTIVE = RGB(255, 255, 255);
		SYS_BOX_PEN_WIDTH = 1;
		SYS_BOX_PEN_COLOR_DISABLE = RGB(222, 222, 222);

		SYS_BOX_BK_COLOR_NORMAL = CAPTION_COLOR;
		SYS_BOX_BK_COLOR_DOWN = RGB(38, 124, 194);
		SYS_BOX_BK_COLOR_HOT = RGB(58, 149, 222);


		EDIT_BORDER_COLOR_FOCUS = BORDER_PEN_COLOR_ACTIVE;
		EDIT_BORDER_COLOR_NORMAL = BORDER_PEN_COLOR_UNACTIVE;

		COMBOBOX_ARROW_PEN_COLOR_NORMAL = RGB(132, 132, 132);
		COMBOBOX_ARROW_BK_COLOR_HOVER = RGB(201, 222, 245);
		COMBOBOX_ARROW_BK_COLOR_DOWN = RGB(201, 222, 255);

		LISTCTRL_BK_COLOR_NORMAL2 = RGB(237, 243, 254);
		LISTCTRL_BK_COLOR_NORMAL = RGB(255, 255, 255);
		LISTCTRL_BK_COLOR_HOVER = RGB(244, 244, 244);
		LISTCTRL_BK_COLOR_SELECT = RGB(51, 153, 255);
		LISTCTRL_BK_COLOR_LOSE_FOCUS = RGB(204, 206, 219);
		LISTCTRL_BORDER_COLOR = EDIT_BORDER_COLOR_NORMAL;
		LISTCTRL_SEPARATED_COLOR = RGB(222, 222, 222);

		DROPLIST_BORDER_COLOR = EDIT_BORDER_COLOR_FOCUS;

		LISTBOX_BK_COLOR_NORMAL = LISTCTRL_BK_COLOR_NORMAL;
		LISTBOX_BK_COLOR_SELECT = LISTCTRL_BK_COLOR_SELECT;
		LISTBOX_BORDER_COLOR = LISTCTRL_BORDER_COLOR;

		MENU_BK_COLOR_NORMAL = RGB(255, 255, 255);
		MENU_BK_COLOR_SELECT = RGB(204, 232, 255);

		TOOLTIP_BK_COLOR_NORMAL = RGB(240, 240, 240);
		TOOLTIP_BORDER_COLOR = RGB(24, 131, 235);

		HEADERCTRL_BK_COLOR_NORMAL = RGB(255, 255, 255);//RGB(233,233,233) RGB(107,165,215)//
		HEADERCTRL_BK_COLOR_HOVER = RGB(217, 235, 249);
		HEADERCTRL_BK_COLOR_DOWN = RGB(188, 220, 244);;
		HEADERCTRL_BORDER_COLOR = EDIT_BORDER_COLOR_NORMAL;
		HEADERCTRL_SEPARATED_COLOR = RGB(222, 222, 222);

		PAGEPANE_BK_COLOR_NORMAL = RGB(255, 255, 255);

		TAB_BOTTOM_SPLIT_COLOR = RGB(235, 235, 235);
		TAB_BK_COLOR_HOT = RGB(50, 150, 250);
		TAB_BK_COLOR_SEL = RGB(50, 150, 250);
		TAB_BK_COLOR_NORMAL = RGB(238, 236, 239);
		BUTTON_BK_COLOR_SEL = RGB(186, 212, 255);

		ITEM_BK_COLOR_HOT = RGB(240, 240, 240);
		ITEM_BK_COLOR_SEL = RGB(186, 212, 255);
		ITEM_BORBER_COLOR_SEL = RGB(28, 151, 234);

		DATETABLE_CURRENT_DAY_BK_COLOR = RGB(200, 220, 253);
		DATETABLE_CURRENT_DAY_BORDER_COLOR = EDIT_BORDER_COLOR_FOCUS;
		DATETABLE_HOVER_DAY_BORDER_COLOR = RGB(255, 120, 0);
		DATETABLE_CHECK_DAY_BORDER_COLOR = RGB(255, 0, 0);
		DATETABLE_MARK_DAY_BK_COLOR = RGB(42, 161, 70);

		SCROLLBAR_COLOR_NORMAL = RGB(222, 223, 231);
		SCROLLBAR_COLOR_SELECT = RGB(106, 106, 106);
		SCROLLBAR_COLOR_HOVER = RGB(136, 136, 136);
		SCROLLBAR_ARROW_COLOR_HOVER = RGB(28, 151, 234);
		SCROLLBAR_ARROW_COLOR_NORMAL = RGB(134, 137, 153);
		SCROLLBAR_ARROW_COLOR_DOWN = RGB(0, 122, 204);

		TIMETABLE_HEADER_COLOR = RGB(219, 222, 223);
		TIMETABLE_ITEM_COLOR = RGB(255, 255, 255);

		PROGRESS_BK_COLOR = BUTTON_BK_COLOR_NORMAL;
		PROGRESS_PROCESS_COLOR = RGB(0, 122, 204);

		TEXT_COLOR_SELECT = RGB(255, 255, 255);
		TEXT_COLOR_HOVER = RGB(28, 151, 234);
		TEXT_COLOR_NORMAL = RGB(0, 0, 0);
		TEXT_COLOR_DISABLE = RGB(127, 127, 127);

		TAB_TEXT_COLOR_SELECT = RGB(50, 150, 250);
		TAB_TEXT_COLOR_NORMAL = RGB(0, 0, 0);

		SMALL_ARROW_PEN_WIDTH = 1;
		LARGE_ARROW_PEN_WIDTH = 2;

		CHECKBOX_WIDTH = 18;
		CHECKBOX_HEIGHT = 18;
		ID_CHECK_SELECT_ALL = 11000;
		SCROLLBAR_WIDTH = 16;
		PAGE_PANE_HEIGHT = 24;
	};
}QYThemeStruct;
