#pragma once



#define  WM_FFPLAY_VIDEO_RESIZE   WM_USER + 1000


#ifdef __cplusplus
extern "C"{
#endif
	int ffplayStartUp(HWND hMsgWnd);

	int ffplayPlay(HWND hWnd, const char *cmd,...);

	int ffplayIsPlaying();

	int ffplayPause();

	int ffplayIsPause();


	void ffplaySeekRight();

	void ffplaySeekLeft();

	void ffplaySeekUp();

	void ffplaySeekDown();

	void ffplaySeekTime(int time);

	int  ffplayGetTime(int *totalTime,int *playTime);

	void ffplayStop();

#ifdef __cplusplus
}
#endif
