#include "MyMediaFile.h"

#include <windows.h>
#include <vector>

const char AV_FILE_HEADER_STRING2[] = "00000000000";


#define  M   (1024*1024)



enum AV_TYPE{
	AV_VIDEO = 0,
	AV_AUDIO = 1,
};


typedef struct
{
	BYTE	videoCoder;//��Ƶ������
	BYTE	audioCoder;//��Ƶ������
	USHORT	videoWidth;
	USHORT	videoHeight;
	BYTE	vcoderType;//��ʶʹ�ò�ͬ�Ľ���264�����
	BYTE	reserved[20];//����
	BYTE	frameRate;//֡��
}AV_FILE_HEADER2;//����28���ֽڣ���֮ǰ���ļ�ͷ��Ӧ

typedef struct
{
	unsigned long pos;
	unsigned long ts;
}INDEX_PACKET;

typedef struct tagINDEX_PACKET2
{
	uint64_t pos;
	unsigned int ts;
	unsigned int	 reserved;
	void operator = (const INDEX_PACKET &pack)
	{
		pos = pack.pos;
		ts = pack.ts;
		reserved = 0;
	}
}INDEX_PACKET2;

typedef struct AV_HEADER2
{
	unsigned int type : 2;   //0 video, 1 audio
	unsigned int coder : 4;   //audio coder or video nal type
	unsigned int reserved : 6;
	unsigned int len : 20;

	unsigned short videoWidth;//��Ƶ���
	unsigned short videoHeight;//��Ƶ�߶�

	unsigned int timestamp;

	void operator = (const AV_HEADER2 &header)
	{
		type = header.type;
		coder = header.coder;
		reserved = header.reserved;
		len = header.len;
		timestamp = header.timestamp;
		videoWidth = 0;
		videoHeight = 0;
	}
}AV_HEADER2;



int myMediaFileOpen(MyMediaFile *myMediaFile,const char *filename)
{
	BYTE frame_rate;
	BOOL bRepair = FALSE;
	myMediaFile->m_pFile = fopen(filename, "rb");

	if (NULL == myMediaFile->m_pFile)
	{
		return -1;
	}
		

	fseek(myMediaFile->m_pFile, 0, SEEK_SET);
	//CHECK FILE HEADER
	size_t len;
	char file_header[256];
	if (1)
	{
		fseek(myMediaFile->m_pFile, 0, SEEK_SET);
		len = fread(file_header, 1, strlen(AV_FILE_HEADER_STRING2), myMediaFile->m_pFile);
		if (len != strlen(AV_FILE_HEADER_STRING2))
		{
			return -2;
		}

			fseek(myMediaFile->m_pFile, strlen(AV_FILE_HEADER_STRING2), SEEK_SET);
			AV_FILE_HEADER2 file_header2;
			if (1 != fread(&file_header2, sizeof(file_header2), 1, myMediaFile->m_pFile))
			{
				return -4;
			}
		
	}
	else
	{
		if (1 != fread(&frame_rate, 1, 1, myMediaFile->m_pFile))
			return -5;
	}

	//READ INDEX
	char szIdxFlag[256];
	unsigned int idx_count;
	if (0 != fseek(myMediaFile->m_pFile, -8, SEEK_END))
	{
		return -6;
	}
	fread(szIdxFlag, 1, strlen("ENDINDEX"), myMediaFile->m_pFile);
	szIdxFlag[8] = '\0';
	if (strcmp(szIdxFlag, "ENDINDEX"))
	{
		return -7;
	}
	if (0 != fseek(myMediaFile->m_pFile, -12, SEEK_END))
		return -8;
	fread(&idx_count, 4, 1, myMediaFile->m_pFile);
	if (idx_count == 0)
		return -9;

	long nOffset;

	int size = sizeof(INDEX_PACKET2);
	nOffset = sizeof(INDEX_PACKET2)*idx_count + strlen("BEGININDEX") + strlen("ENDINDEX") + 4;

	if (0 != fseek(myMediaFile->m_pFile, -nOffset, SEEK_END))
	{
		return -10;
	}

	fread(szIdxFlag, 1, strlen("BEGININDEX"), myMediaFile->m_pFile);
	szIdxFlag[10] = '\0';
	if (strcmp(szIdxFlag, "BEGININDEX"))
	{
		return -11;
	}


	while (idx_count-- > 0)
	{
		INDEX_PACKET2 idx_packet2;

		fread(&idx_packet2, sizeof(INDEX_PACKET2), 1, myMediaFile->m_pFile);

		myMediaFile->pos[myMediaFile->frames] = idx_packet2.pos;
		myMediaFile->ts[myMediaFile->frames] = idx_packet2.ts;

		myMediaFile->frames++;
	}

	INDEX_PACKET2 idx_front, idx_back;
	if (myMediaFile->frames > 0)
	{
		myMediaFile->duration = (myMediaFile->ts[myMediaFile->frames - 1] - myMediaFile->ts[0]) / 1000;

		myMediaFile->m_pBuffer = (char*)malloc(M + sizeof(AV_HEADER2));


		fseek(myMediaFile->m_pFile, myMediaFile->pos[0], SEEK_SET);

		return 0;
	}

	return -12;
	
}

void myMediaFileClose(MyMediaFile *myMediaFile)
{
	if (nullptr != myMediaFile->m_pFile)
	{
		fclose(myMediaFile->m_pFile);
	}
}


int myMediaFileReadFrame(MyMediaFile *myMediaFile,AVPacket *pkt)
{
    LONG len;

    BYTE  audioRead[1024] = {0};

    short OutLen;
    int   ret;

    BOOL			bFileEnd = FALSE;

    int	  h264Flag;

    long last_time = 0;
    int  off = 0;

        AV_HEADER2		av_header2;

        AV_HEADER2 last_av_header2;
       
		len = fread(&av_header2, sizeof(av_header2), 1, myMediaFile->m_pFile);
            if(len != 1)
            {
                bFileEnd = TRUE;
				return -1;
            }
            if(av_header2.type == 0)
            {
				

                if (av_header2.len > M)
                {
                    bFileEnd = TRUE;
					return -2;
                }

				len = fread(pkt->data, 1, av_header2.len, myMediaFile->m_pFile);
                if (len != av_header2.len)
                {
                    bFileEnd = TRUE;
					return -3;
                }
                for (int i = 0; i < len;i++)
                {
					pkt->data[i] = pkt->data[i] ^ g_mask[i % 8];
                }

				pkt->pts = pkt->dts = av_header2.timestamp;
				pkt->size = av_header2.len;
				pkt->stream_index = 0;
				return 0;
            }
            else if(av_header2.type == 1)
            {
				

                if (1024 >= av_header2.len)
                {
					
					len = fread(pkt->data, 1, av_header2.len, myMediaFile->m_pFile);
                    if (len == av_header2.len)
                    {
                        for (int i = 0; i < len;i++)
                        {
							pkt->data[i] = pkt->data[i] ^ g_mask[i % 8];
                        }

						pkt->pts = pkt->dts = av_header2.timestamp;
						pkt->size = av_header2.len;
						pkt->stream_index = 1;

						return 0;
                    }

                }
                else
                {
					return -4;
                }

            }
            else
            {
				return -5;
            }



	return -6;
}
