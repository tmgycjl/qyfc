
#include "QYImageManage.h"

unsigned char QYImageManage::m_maskingKey[] = { 0xfc, 0x76, 0xe1, 0x18 };

QYImageManage* QYImageManage::m_pInstance = nullptr;

QYImageManage* QYImageManage::instance()
{
	if (nullptr == m_pInstance)
	{
        m_pInstance = new QYImageManage;
	}

	return m_pInstance;
}

void  QYImageManage::getImage(UINT uResID, BYTE **ppStream, unsigned int *len)
{
	auto it = m_mapIndex.find(uResID);
	if (it == m_mapIndex.end())
	{
		return;
	}

	FILE *pFile = nullptr;
	char outfilePath[MAX_PATH] = { 0 };
	QYString::SafeW2A(outfilePath, MAX_PATH, m_sImageFileName);
	if (0 != fopen_s(&pFile, outfilePath, "rb"))
	{
		return;
	}
	
	unsigned int pos = it->second>> 32;
	unsigned int size = (unsigned int)it->second;
	if (MB < size || 0 >= size)
	{
		return;
	}

	*ppStream = new BYTE[size];

	fseek(pFile, pos, SEEK_SET);
	*len = fread(*ppStream, 1, size,pFile);
	BYTE *pStart = *ppStream;
	for (int i = 0; i < *len; i++)
	{
		*pStart = *pStart ^ m_maskingKey[i % 4];
		pStart++;
	}

	fclose(pFile);
}

void QYImageManage::GenerateImage()
{
	unsigned int indexPos = 0;
	FILE *pOutFile = nullptr;
	char outfilePath[MAX_PATH] = { 0 };
	QYString::SafeW2A(outfilePath, MAX_PATH, m_sImageFileName);
#if 0
	
	if (0 != fopen_s(&pOutFile, outfilePath, "wb"))
	{
		return;
	}

	
	fwrite(&indexPos, sizeof(unsigned int), 1, pOutFile);

	std::vector<std::pair<UINT,UINT64>> vectorIndex;
	int fileCount = 0;
	for (int i = IMAGE_NONE + 1; i < IMAGE_COUNT; i++)
	{
		char fileName[MAX_PATH] = { 0 };
		QYString sFileName = GetFileName(i);
		QYString::SafeW2A(fileName, MAX_PATH, sFileName);

		FILE *pFile = nullptr;

		if (0 != fopen_s(&pFile, fileName, "rb"))
		{
			//g_ConsoleLog.Print("no file:%s\n", fileCount++, fileName);
			continue;
		}

		unsigned int pos = ftell(pOutFile);
		std::pair<UINT, UINT64> index;
		index.first = i;
		index.second = pos;

		char readBuf[1024] = { 0 };
		unsigned int readLen = 0;
		unsigned int fileSize = 0;
		do
		{
			readLen = fread(readBuf, 1, 1024, pFile);
			for (int j = 0; j < readLen; j++)
			{
				readBuf[j] = readBuf[j] ^ m_maskingKey[j % 4];
			}
			fwrite(readBuf, readLen, 1, pOutFile);
			fileSize += readLen;
		} while (0 < readLen);

		fclose(pFile);

		index.second <<= 32;
		index.second |= fileSize;
		vectorIndex.push_back(index);
		//g_ConsoleLog.Print("index:%d,file:%s\n", fileCount++, fileName);
	}

	indexPos = ftell(pOutFile);

	for (std::pair<UINT, UINT64> index : vectorIndex)
	{
		fwrite(&index.first, sizeof(UINT), 1, pOutFile);
		fwrite(&index.second, sizeof(UINT64), 1, pOutFile);
	}

	fseek(pOutFile, 0, SEEK_SET);
	fwrite(&indexPos, sizeof(unsigned int), 1, pOutFile);

	fclose(pOutFile);
#endif
	FILE *pFile = nullptr;

	if (0 != fopen_s(&pOutFile, outfilePath, "rb"))
	{
		return;
	}
	
	fread(&indexPos, sizeof(unsigned int), 1, pOutFile);

	fseek(pOutFile, indexPos, SEEK_SET);
	UINT64 index = 0;
	UINT resID = 0;
	while (1 == fread(&resID, sizeof(UINT), 1, pOutFile))
	{
		if (1 == fread(&index, sizeof(UINT64), 1, pOutFile))
		{
			m_mapIndex.insert(std::map<UINT, UINT64>::value_type(resID, index));
		}
		
	} 
	fclose(pOutFile);
}
