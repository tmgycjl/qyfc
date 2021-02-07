#pragma once



#define  WM_FFPLAY_VIDEO_RESIZE   WM_USER + 1000


#ifdef __cplusplus
#if defined __BUILD_DLL
#define FFPLAY_DLLAPI		extern "C" __declspec(dllexport)
#elif defined __BUILD_LIB
#define FFPLAY_DLLAPI
#else
#define FFPLAY_DLLAPI		extern "C" __declspec(dllimport)
#endif
#else
#if defined FFPLAY_EXPORTS
#define FFPLAY_DLLAPI	 __declspec(dllexport)
#elif defined __BUILD_LIB
#define FFPLAY_DLLAPI
#else
#define FFPLAY_DLLAPI	 __declspec(dllimport)
#endif
#endif

	FFPLAY_DLLAPI int ffplayStartUp(HWND hMsgWnd);

	FFPLAY_DLLAPI int ffplayPlay(HWND hWnd, const char *cmd, ...);

	FFPLAY_DLLAPI int ffplayIsPlaying();

	FFPLAY_DLLAPI int ffplayPause();

	FFPLAY_DLLAPI int ffplayIsPause();


	FFPLAY_DLLAPI void ffplaySeekRight();

	FFPLAY_DLLAPI void ffplaySeekLeft();

	FFPLAY_DLLAPI void ffplaySeekUp();

	FFPLAY_DLLAPI void ffplaySeekDown();

	FFPLAY_DLLAPI void ffplaySeekTime(int time);

	FFPLAY_DLLAPI int  ffplayGetTime(int *totalTime, int *playTime);

	FFPLAY_DLLAPI void ffplayStop();

	FFPLAY_DLLAPI void ffplayVideoResize(RECT *rect);

	FFPLAY_DLLAPI void ffplayUseSDLRender(int use);

	FFPLAY_DLLAPI int ffplayIsSDLRender();

	FFPLAY_DLLAPI int ffplayWriteMyMediaFile(const char *filePath,const  char *outPath);

