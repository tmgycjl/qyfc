#pragma once

#include <stdio.h>


#ifdef __cplusplus
extern "C"{
#endif


#include <libavcodec/avcodec.h>



typedef struct  
{
	int write;
	FILE	     *m_pFile;
	char		m_filename[260];
	char	     *m_pBuffer;
	int         vStreamIndex;
	int         aStreamIndex;
	int         vCodec;
	int         aCodec;
	int         width;
	int         height;
	unsigned long *pos;
	unsigned long *ts;
	unsigned long frames;
	unsigned long startTime;
	unsigned long duration;
	unsigned long recBufferPos;
	float  frameRate;
	float         sampleRate;
	char          sps[100];
	unsigned short           spsLen;
	char          pps[32];
	unsigned   short        ppsLen;
	char *frameBuffer;
	int extradata_size;
	uint8_t extradata[200];
	
}MyMediaFile;


int myMediaFileOpen(MyMediaFile *myMediaFile, const char *filename, int write);

int myMediaFileReadFrame(MyMediaFile *myMediaFile, AVPacket *pkt);

int myMediaFileWriteFrame(MyMediaFile *myMediaFile, AVPacket *pkt);

int myMediaFileSeekFrame(MyMediaFile *myMediaFile, unsigned long pos);

void myMediaFileClose(MyMediaFile *myMediaFile);

int myMediaPicWrite(const char *inFile,const char *outFile);

int myMediaPicRead(const char *inFile,char **streamBuffer);

#ifdef __cplusplus
}
#endif
