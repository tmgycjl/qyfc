#include "QYImageManager.h"
#include "QYpicture.h"
#include "QYFileStudio.h"
#include "QYApp.h"
#include <vector>

unsigned char QYImageManager::m_maskingKey[] = { 0xfc, 0x76, 0xe1, 0x18 };

QYImageManager* QYImageManager::m_pInstance = nullptr;

QYImageManager* QYImageManager::instance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new QYImageManager;
		QYPicture::SetPictureCallback(QYPictureCallback, m_pInstance);
	}

	return m_pInstance;
}

QYImageManager::~QYImageManager()
{
	m_mapIndex.clear();
}

void  QYImageManager::QYPictureCallback(void *data, const char *source, BYTE **ppStream, unsigned int *len)
{
	QYImageManager *pThis = (QYImageManager*)data;
	if (nullptr != pThis)
	{
		pThis->OnPictureCallback(source, ppStream, len);
	}
}

void  QYImageManager::OnPictureCallback(const char *source, BYTE **ppStream, unsigned int *len)
{
	auto it = m_mapIndex.find(source);
	if (it == m_mapIndex.end())
	{
		return;
	}

	FILE *pFile = nullptr;
    if (0 != fopen_s(&pFile, m_sImageFileName.c_str(), "rb"))
	{
		return;
	}

	unsigned int pos = it->second >> 32;
	unsigned int size = (unsigned int)it->second;
	if (MB < size || 0 >= size)
	{
		return;
	}

	*ppStream = new BYTE[size];

	fseek(pFile, pos, SEEK_SET);
	*len = fread(*ppStream, 1, size, pFile);
	BYTE *pStart = *ppStream;
	for (int i = 0; i < *len; i++)
	{
		*pStart = *pStart ^ m_maskingKey[i % 4];
		pStart++;
	}

	fclose(pFile);
}


void QYImageManager::findImage(LPCTSTR w_imageDir, FILE *outFile, std::vector<std::pair<std::string, UINT64>> *vectorIndex)
{
    TCHAR find_imageDirBuf[MAX_PATH] = { 0 };
    SafeSprintf(find_imageDirBuf, MAX_PATH, L"%s\\*.png*", w_imageDir);

	QYFileStudio fdRoot;
    if (!fdRoot.FindFirst(find_imageDirBuf))
	{
		return;
	}

	do
	{
        if (0 == SafeStrcmp(L".", fdRoot.GetFileName()) || 0 == SafeStrcmp(L"..", fdRoot.GetFileName()))
        {
            continue;
        }

        QYString str = w_imageDir;
        str += L"\\";
        str += fdRoot.GetFileName();

		if (fdRoot.IsDirectory())
		{
			findImage(str, outFile, vectorIndex);
			continue;
		}

        
		char filePath[MAX_PATH] = { 0 };
        QYString::SafeW2A(filePath, MAX_PATH, str);

		FILE *pFile = nullptr;

        if (0 != fopen_s(&pFile, filePath, "rb"))
		{
			continue;
		}

		unsigned int pos = ftell(outFile);
		std::pair<std::string, UINT64> index;
        index.first = filePath;
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
			fwrite(readBuf, readLen, 1, outFile);
			fileSize += readLen;
		} while (0 < readLen);

		fclose(pFile);

		index.second <<= 32;
		index.second |= fileSize;
		vectorIndex->push_back(index);
    }while (fdRoot.FindNext());

}

void QYImageManager::packImages(std::string &imageDir, std::string &dstFileName)
{
	unsigned int indexPos = 0;
	FILE *pOutFile = nullptr;

	std::string dstImageFileName = QYApp::m_imagePath + dstFileName;
	if (0 != fopen_s(&pOutFile, dstImageFileName.c_str(), "wb"))
	{
		return;
	}

	fwrite(&indexPos, sizeof(unsigned int), 1, pOutFile);
	std::vector<std::pair<std::string, UINT64>> vectorIndex;

	
	TCHAR w_imageDirBuf[MAX_PATH] = { 0 };
    QYString::SafeA2W(w_imageDirBuf, MAX_PATH, imageDir.c_str());

	findImage(w_imageDirBuf, pOutFile, &vectorIndex);

#if 0
	int fileCount = 0;
	for (int i = IMAGE_NONE + 1; i < IMAGE_COUNT; i++)
	{
		char fileName[MAX_PATH] = { 0 };
		QYString sFileName = GetFileName(i);
		QYString::SafeW2A(fileName, MAX_PATH, sFileName);

		FILE *pFile = nullptr;

		if (0 != fopen_s(&pFile, fileName, "rb"))
		{
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
#endif

	indexPos = ftell(pOutFile);

	for (std::pair<std::string, UINT64> index : vectorIndex)
	{
        char name[MAX_PATH] = { 0 };
        const char *pName = index.first.c_str() +  QYApp::m_imagePath.length();
        if (nullptr != pName)
        {
            strcpy_s(name, pName);
            fwrite(name, MAX_PATH, 1, pOutFile);
            fwrite(&index.second, sizeof(UINT64), 1, pOutFile);
        }
	}

	fseek(pOutFile, 0, SEEK_SET);
	fwrite(&indexPos, sizeof(unsigned int), 1, pOutFile);

	fclose(pOutFile);
	
}


void QYImageManager::loadImages(std::string &imageFile)
{
	unsigned int indexPos = 0;
	FILE *pOutFile = nullptr;
	FILE *pFile = nullptr;

    m_sImageFileName = imageFile;

	if (0 != fopen_s(&pOutFile, imageFile.c_str(), "rb"))
	{
		return;
	}

	fread(&indexPos, sizeof(unsigned int), 1, pOutFile);

	fseek(pOutFile, indexPos, SEEK_SET);
	UINT64 index = 0;
	std::string  source;
	char sourceBuf[MAX_PATH] = { 0 };
	while (1 == fread(sourceBuf, MAX_PATH, 1, pOutFile))
	{
		source = sourceBuf;
		if (1 == fread(&index, sizeof(UINT64), 1, pOutFile))
		{
			m_mapIndex.insert(std::map<std::string, UINT64>::value_type(source, index));
		}

	}
	fclose(pOutFile);
}