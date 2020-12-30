#include "QYImageList.h"


QYImageList::QYImageList()
{
}


QYImageList::~QYImageList()
{
	for (auto it = m_mapImage.begin(); it != m_mapImage.end(); it++)
	{
		SAFE_DELETE((*it).second);
	}
	
	m_mapImage.clear();
}

#if 0

int QYImageList::AddImage(int image, UINT resID, BOOL bSubImage, HMODULE hModule)
{
	auto it = m_mapImage.find(image);
	if (it != m_mapImage.end())
	{
		return -1;
	}

	QYPicture *pic = new QYPicture(bSubImage);
	assert(nullptr != pic);
	if (!pic->Load(resID, QY_IMAGE_PNG, hModule))
	{
		SAFE_DELETE(pic);
		return -1;
	}
	m_mapImage.insert(std::map<int, QYPicture*>::value_type(image, pic));

	return image;
}

#endif

int  QYImageList::AddImage(int image, const char* imageFilePath, BOOL bSubImage)
{
	if (0 >= strlen(imageFilePath))
	{
		return image;
	}

	auto it = m_mapImage.find(image);
	if (it != m_mapImage.end())
	{
		return -1;
	}

    QYPicture *pic = new QYPicture(bSubImage ? IMAGE_STATUS_NORMAL | IMAGE_STATUS_HOVER : IMAGE_STATUS_HOVER);
	assert(nullptr != pic);
	if (!pic->load(imageFilePath))
	{
		SAFE_DELETE(pic);
		return -1;
	}
	m_mapImage.insert(std::map<int, QYPicture*>::value_type(image, pic));

	return image;
}


int  QYImageList::AddImage(int image, LPCTSTR imageFilePath, BOOL bSubImage)
{
	if (0 >= SafeStrlen(imageFilePath))
	{
		return image;
	}

	auto it = m_mapImage.find(image);
	if (it != m_mapImage.end())
	{
		return -1;
	}

	QYPicture *pic = new QYPicture(bSubImage ? IMAGE_STATUS_NORMAL | IMAGE_STATUS_HOVER : IMAGE_STATUS_HOVER);
	assert(nullptr != pic);
	if (!pic->Load(imageFilePath))
	{
		SAFE_DELETE(pic);
		return -1;
	}
	m_mapImage.insert(std::map<int, QYPicture*>::value_type(image, pic));

	return image;
}

int  QYImageList::AddImage(const char* imageFilePath, BOOL bSubImage)
{
    if (0 >= strlen(imageFilePath))
    {
        return -1;
    }

    int image = 0;
    for (auto it = m_mapImage.begin(); it != m_mapImage.end(); it++)
    {
        image = std::fmax(it->first, image) + 1;
    }

    auto it = m_mapImage.find(image);
    if (it != m_mapImage.end())
    {
        return -1;
    }

    QYPicture *pic = new QYPicture(bSubImage ? IMAGE_STATUS_NORMAL | IMAGE_STATUS_HOVER : IMAGE_STATUS_HOVER);
    assert(nullptr != pic);
    if (!pic->load(imageFilePath))
    {
        SAFE_DELETE(pic);
        return -1;
    }
    m_mapImage.insert(std::map<int, QYPicture*>::value_type(image, pic));

    return image;
}



int  QYImageList::AddImage(int image, UINT resID, BOOL bSubImage /* = FALSE */)
{
	auto it = m_mapImage.find(image);
	if (it != m_mapImage.end())
	{
		return -1;
	}

	QYPicture *pic = new QYPicture(bSubImage ? IMAGE_STATUS_NORMAL | IMAGE_STATUS_HOVER : IMAGE_STATUS_HOVER);
	assert(nullptr != pic);
	if (!pic->Load(resID))
	{
		SAFE_DELETE(pic);
		return -1;
	}
	m_mapImage.insert(std::map<int, QYPicture*>::value_type(image, pic));

	return image;
}



QYPicture* QYImageList::GetImage(int image)
{
	auto it = m_mapImage.find(image);
	return (it != m_mapImage.end()) ? (*it).second : nullptr;
}