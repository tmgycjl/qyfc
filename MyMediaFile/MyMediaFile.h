#pragma once

#include <stdio.h>


#ifdef __cplusplus
extern "C"{
#endif


#include <libavcodec/avcodec.h>

	typedef struct
	{
		unsigned long pos;
	}frame_t;

typedef struct  
{
	FILE	     *m_pFile;

	char		m_filename[260];
	char	     *m_pBuffer;
	unsigned long *pos;
	unsigned long *ts;
	unsigned long frames;
	unsigned long startTime;
	unsigned long duration;
	float  frameRate;
}MyMediaFile;


int myMediaFileOpen(MyMediaFile *myMediaFile, const char *filename);

int myMediaFileReadFrame(MyMediaFile *myMediaFile, AVPacket *pkt);

int myMediaFileSeekFrame(MyMediaFile *myMediaFile, unsigned long pos);

void myMediaFileClose(MyMediaFile *myMediaFile);



#ifdef __cplusplus
}
#endif
