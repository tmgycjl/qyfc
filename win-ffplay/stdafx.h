// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�: 
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "../QYFC/qyfc.h"
#pragma comment(lib,"qyfc")


//#include <vld.h>


#include <SDL.h>
#include <SDL_thread.h>

#pragma  comment(lib,"SDL2")


#include "../ffplay/ffplay.h"

#pragma comment(lib,"ffplay")

#include <vector>

#define  CONFIG_INI   L"config.ini"

static std::vector<std::string> _vecRender = { "direct3d", "directdraw" };


