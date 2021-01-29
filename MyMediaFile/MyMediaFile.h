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
	unsigned long pos[1024];
	unsigned long ts[1024];
	unsigned long frames;
	
	unsigned long duration;
}MyMediaFile;


int myMediaFileOpen(MyMediaFile *myMediaFile, const char *filename);

int myMediaFileReadFrame(MyMediaFile *myMediaFile, AVPacket *pkt);

void myMediaFileClose(MyMediaFile *myMediaFile);



#ifdef __cplusplus
}
#endif
