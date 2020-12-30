#include "QYSysProcess.h"


// DiskProcess.cpp: implementation of the CSysProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "QYSysProcess.h"
#include <atlbase.h>
#include "winperf.h"   
#include <iphlpapi.h> 
#include <Tlhelp32.h>
#include "QYFileStudio.h"

#pragma  comment(lib,"iphlpapi.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define SystemBasicInformation       0
#define SystemPerformanceInformation 2
#define SystemTimeInformation        3
#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

__int64 QYSysProcess::m_dwidleTime = 0;
__int64 QYSysProcess::m_dwkernelTime = 0;
__int64 QYSysProcess::m_dwuserTime = 0;
__int64 QYSysProcess::m_dwTotalOutOctets = 0;
__int64 QYSysProcess::m_dwTotalIntOctets = 0;

typedef struct
{
	DWORD   dwUnknown1;
	ULONG   uKeMaximumIncrement;
	ULONG   uPageSize;
	ULONG   uMmNumberOfPhysicalPages;
	ULONG   uMmLowestPhysicalPage;
	ULONG   uMmHighestPhysicalPage;
	ULONG   uAllocationGranularity;
	PVOID   pLowestUserAddress;
	PVOID   pMmHighestUserAddress;
	ULONG   uKeActiveProcessors;
	BYTE    bKeNumberProcessors;
	BYTE    bUnknown2;
	WORD    wUnknown3;
} SYSTEM_BASIC_INFORMATION;

typedef struct
{
	LARGE_INTEGER   liIdleTime;
	DWORD           dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
	LARGE_INTEGER liKeBootTime;
	LARGE_INTEGER liKeSystemTime;
	LARGE_INTEGER liExpTimeZoneBias;
	ULONG         uCurrentTimeZoneId;
	DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;

QYSysProcess::QYSysProcess()
{

	m_NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle(_T("ntdll")),
		"NtQuerySystemInformation"
		);

}

QYSysProcess::~QYSysProcess()
{

}

int QYSysProcess::GetAllAvailableDisk(QYString &strAllAvalableDisk)
{
	TCHAR chDrive[3] = { 0 };
	int nDriveType;
	int nRet = 0;
	strAllAvalableDisk = L"";

	DWORD dwDriveMask = GetLogicalDrives();
	for (int i = 0; i < 'Z' - 'C'; i++)
	{
		if ((dwDriveMask & (1 << (i + 2))) != 0)
		{
			chDrive[0] = (TCHAR)('C' + i);
			chDrive[1] = ':';
			chDrive[2] = '\0';
			TCHAR szDriver[3] = { 0 };
			//QYString::SafeA2W(szDriver, sizeof(szDriver) / sizeof(TCHAR), chDrive);
			nDriveType = GetDriveType(szDriver);
			if (((nDriveType == DRIVE_FIXED) || (nDriveType == DRIVE_REMOVABLE) || nDriveType == DRIVE_REMOTE) && (chDrive[0] != 'A') && (chDrive[0] != 'B'))
			{
				strAllAvalableDisk += chDrive;
				nRet++;
			}
		}
	}
	return nRet;
}

BOOL QYSysProcess::GetAvailableRecDisk(QYString strCurRecDisk, int nSwitchDiskFreeSpace, int nSysDiskFreeSpace, QYString &strAvailabeRecDisk)
{
	int i;
	int k = 0;
	if (nSwitchDiskFreeSpace < 200)
	{
		nSwitchDiskFreeSpace = 200;
	}
	if (nSysDiskFreeSpace < 1500)
	{
		nSysDiskFreeSpace = 1500;
	}
	ULARGE_INTEGER result_freespace, result_totalspace, result_g;
	float   fFreeSpace = 0.00;
	QYString strDisk;

	QYString strAllAvalableDisk;
	GetAllAvailableDisk(strAllAvalableDisk);

	for (i = 0; i<strAllAvalableDisk.Length(); i += 2)
	{
		strDisk = strAllAvalableDisk.Mid(i, 2);
		if (strDisk == strCurRecDisk)
		{
			k = i + 2;
			if (k >= strAllAvalableDisk.Length())
			{
				k = 0;
			}
			break;
		}
	}
	for (i = k; i<strAllAvalableDisk.Length(); i += 2)
	{
		strDisk = strAllAvalableDisk.Mid(i, 2);
		GetDiskFreeSpaceEx(strDisk, &result_freespace, &result_totalspace, &result_g);
		fFreeSpace = (__int64)result_freespace.QuadPart / 1024 / 1024;
		if (strDisk == GetSysDisk() && nSwitchDiskFreeSpace < nSysDiskFreeSpace)
		{
			if (fFreeSpace > nSysDiskFreeSpace + 100)
			{
				strAvailabeRecDisk = strDisk;
				return TRUE;
			}

		}
		else
		{
			if (fFreeSpace > nSwitchDiskFreeSpace + 100)
			{
				strAvailabeRecDisk = strDisk;
				return TRUE;
			}
		}
	}
	for (i = 0; i<k; i += 2)
	{
		strDisk = strAllAvalableDisk.Mid(i, 2);
		GetDiskFreeSpaceEx(strDisk, &result_freespace, &result_totalspace, &result_g);
		fFreeSpace = (__int64)result_freespace.QuadPart / 1024 / 1024;
		if (strDisk == GetSysDisk() && nSwitchDiskFreeSpace < nSysDiskFreeSpace)
		{
			if (fFreeSpace > nSysDiskFreeSpace + 100)
			{
				strAvailabeRecDisk = strDisk;
				return TRUE;
			}
		}
		else
		{
			if (fFreeSpace > nSwitchDiskFreeSpace + 100)
			{
				strAvailabeRecDisk = strDisk;
				return TRUE;
			}
		}
	}
	return FALSE;
}

float QYSysProcess::GetDiskFreeSpace(TCHAR szDisk)
{
	QYString strDisk;
	strDisk.Format(_T("%C:"), szDisk);
	return GetDiskFreeSpace(strDisk);
}

float QYSysProcess::GetDiskFreeSpace(QYString strDisk)
{
	ULARGE_INTEGER result_freespace, result_totalspace, result_g;
	float fFreeSpace = 0.00;
	GetDiskFreeSpaceEx(strDisk, &result_freespace, &result_totalspace, &result_g);
	fFreeSpace = (__int64)result_freespace.QuadPart / 1024 / 1024;
	return fFreeSpace;
}

QYString QYSysProcess::GetSysDisk()
{
	TCHAR chSysDisk[MAX_PATH];
	QYString strSysDisk;
	GetWindowsDirectory(chSysDisk, MAX_PATH);
	strSysDisk = chSysDisk;
	return strSysDisk.Left(2);
}

BOOL QYSysProcess::IsDiskAvailable(QYString strCurRecDisk, int nSwitchDiskFreeSpace, int nSysDiskFreeSpace)
{
	QYString strCurDisk;
	QYString	strAllAvailableDisk;
	float fFreeSpace;
	GetAllAvailableDisk(strAllAvailableDisk);
	for (int i = 0; i<strAllAvailableDisk.Length(); i += 2)
	{
		strCurDisk = strAllAvailableDisk.Mid(i, 2);
		if (strCurRecDisk != strCurDisk)
		{
			fFreeSpace = GetDiskFreeSpace(strCurDisk);
			if (strCurDisk == GetSysDisk())
			{
				int nFreeSpace = (nSwitchDiskFreeSpace <= nSysDiskFreeSpace ? nSysDiskFreeSpace : nSwitchDiskFreeSpace);
				if (fFreeSpace > nFreeSpace + 100)
				{
					return TRUE;
				}
			}
			else if (fFreeSpace > nSwitchDiskFreeSpace + 100)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

int QYSysProcess::GetDriverCount()
{
	QYString strAllAvalableDisk;
	GetAllAvailableDisk(strAllAvalableDisk);
	return (strAllAvalableDisk.Length() / 2);
}

float QYSysProcess::GetTotalDiskSpace()
{
	float fTotalDiskSpace = 0.00;
	QYString strDisk;
	ULARGE_INTEGER result_freespace, result_totalspace, result_g;

	QYString strAllAvalableDisk;
	GetAllAvailableDisk(strAllAvalableDisk);
	for (int i = 0; i<strAllAvalableDisk.Length(); i += 2)
	{
		strDisk = strAllAvalableDisk.Mid(i, 2);
		GetDiskFreeSpaceEx(strDisk, &result_freespace, &result_totalspace, &result_g);
		fTotalDiskSpace += (__int64)result_totalspace.QuadPart / 1024 / 1024 / 1024;
	}
	return fTotalDiskSpace;
}

float QYSysProcess::GetUsedDiskSpace()
{
	float fTotalDiskSpace = 0.00;
	float fFreeDiskSpace = 0.00;
	QYString strDisk;
	ULARGE_INTEGER result_freespace, result_totalspace, result_g;

	QYString strAllAvalableDisk;
	GetAllAvailableDisk(strAllAvalableDisk);
	for (int i = 0; i<strAllAvalableDisk.Length(); i += 2)
	{
		strDisk = strAllAvalableDisk.Mid(i, 2);
		GetDiskFreeSpaceEx(strDisk, &result_freespace, &result_totalspace, &result_g);
		fTotalDiskSpace += (__int64)result_totalspace.QuadPart / 1024 / 1024 / 1024;
		fFreeDiskSpace += (__int64)result_freespace.QuadPart / 1024 / 1024 / 1024;
	}
	return (fTotalDiskSpace - fFreeDiskSpace);
}

BOOL QYSysProcess::FindDir(QYString strDir, QYString strCompareFolder, std::vector<QYString>&vecFolderNeedDelete)
{
	vecFolderNeedDelete.clear();
	BOOL bExist;
	BOOL bBegin = TRUE;
	TCHAR szFilePath[MAX_PATH];
	QYString strSubFolderDir;
	WIN32_FIND_DATA wfdSubFolder;//子文件夹
	WIN32_FIND_DATA wfdFolder;//用于查找文件夹是否存在
	HANDLE hSubFolder;

	HANDLE hFolder = FindFirstFile(strDir, &wfdFolder);
	if (hFolder == INVALID_HANDLE_VALUE)
	{
		bExist = FALSE; // 没有找到配备，目录肯定不存在
		return FALSE;
	}
	else
	{
		if (wfdFolder.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // 检查找到的结果是否目录
			bExist = TRUE;                                      // 是目录,目录存在
		else
			bExist = FALSE;                                     // 是目录,目录不存在
		FindClose(hFolder);
		hFolder = NULL;
	}
	int nCount = 0;
	if (bExist)
	{
		strSubFolderDir = strDir + _T("\\*.*");
		hSubFolder = ::FindFirstFile((LPCTSTR)strSubFolderDir, &wfdSubFolder);

		if (hSubFolder != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (0 != SafeStrcmp(wfdSubFolder.cFileName, L".") && 0 != SafeStrcmp(wfdSubFolder.cFileName, L".."))
				{
					SafeStrcpy(szFilePath, sizeof(szFilePath) / sizeof(TCHAR), strDir + _T("\\") + wfdSubFolder.cFileName);
					if (strCompareFolder > szFilePath)
					{
						vecFolderNeedDelete.push_back(szFilePath);
						nCount++;
					}
				}
			} while (FindNextFile(hSubFolder, &wfdSubFolder));
		}
		FindClose(hSubFolder);
		hSubFolder = NULL;
	}
	return nCount > 0;
}

BOOL QYSysProcess::FindDir(QYString strDir, std::vector<QYString>&vecFolderCannotDelete, QYString &strFolder)
{
	BOOL bExist;
	BOOL bBegin = TRUE;
	TCHAR szFilePath[MAX_PATH];
	QYString strSubFolderDir;
	WIN32_FIND_DATA wfdSubFolder;//子文件夹
	WIN32_FIND_DATA wfdFolder;//用于查找文件夹是否存在
	QYString strSubFolder;
	HANDLE hSubFolder;

	HANDLE hFolder = FindFirstFile(strDir, &wfdFolder);
	if (hFolder == INVALID_HANDLE_VALUE)
	{
		bExist = FALSE; // 没有找到配备，目录肯定不存在
		return FALSE;
	}
	else
	{
		if (wfdFolder.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // 检查找到的结果是否目录
			bExist = TRUE;                                      // 是目录,目录存在
		else
			bExist = FALSE;                                     // 是目录,目录不存在
		FindClose(hFolder);
		hFolder = NULL;
	}

	if (bExist)
	{
		strSubFolderDir = strDir + _T("\\*.*");
		hSubFolder = ::FindFirstFile((LPCTSTR)strSubFolderDir, &wfdSubFolder);

		if (hSubFolder != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (((QYString)wfdSubFolder.cFileName != _T(".")) && ((QYString)wfdSubFolder.cFileName != _T("..")))
				{
					SafeStrcpy(szFilePath, sizeof(szFilePath) / sizeof(TCHAR), strDir + _T("\\") + wfdSubFolder.cFileName);
					std::vector<QYString>::iterator Iter = find(vecFolderCannotDelete.begin(), vecFolderCannotDelete.end(), szFilePath);
					if (Iter != vecFolderCannotDelete.end())
					{
						continue;
					}

					if (bBegin)
					{
						strSubFolder = szFilePath;
					}
					else
					{
						if (strSubFolder > szFilePath)
						{
							strSubFolder = szFilePath;
						}
					}
					bBegin = FALSE;
				}
			} while (FindNextFile(hSubFolder, &wfdSubFolder));
		}
		if (strSubFolder == _T(""))
		{
			FindClose(hSubFolder);
			hSubFolder = NULL;
			return FALSE;
		}
		strFolder = strSubFolder;
		FindClose(hSubFolder);
		hSubFolder = NULL;
	}
	return TRUE;
}

BOOL QYSysProcess::DeleteDirectory(TCHAR *pDirName)
{
	if (pDirName == NULL)
	{
		return FALSE;
	}
	QYFileStudio ff;
	TCHAR tempFileFind[MAX_PATH] = { 0 };
	SafeSprintf(tempFileFind, sizeof(tempFileFind) / sizeof(TCHAR), _T("%s\\*.*"), pDirName);
	BOOL bFinded = ff.FindFirst(tempFileFind);
	while (bFinded)
	{
		bFinded = ff.FindNext();
		if (ff.IsDirectory())
		{
			TCHAR foundFileName[MAX_PATH] = { 0 };
			SafeStrcpy(foundFileName, sizeof(foundFileName) / sizeof(TCHAR), ff.GetFileName());
			if (ff.IsDirectory())
			{
				TCHAR tempDir[MAX_PATH] = { 0 };
				SafeSprintf(tempDir, sizeof(tempDir) / sizeof(TCHAR), _T("%s\\%s"), pDirName, foundFileName);
				DeleteDirectory(tempDir);
			}
			else
			{
				TCHAR tempFileName[MAX_PATH] = { 0 };
				SafeSprintf(tempFileName, sizeof(tempFileName) / sizeof(TCHAR), _T("%s\\%s"), pDirName, foundFileName);
				//if(strFileName.Right(4) == _T("h264") || strFileName.Right(5) == _T("gsidx"))//只删除本系统支持的录像文件
				//{
				DeleteFile(tempFileName);
				//}
			}
		}
	}
	
	if (!RemoveDirectory(pDirName))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL QYSysProcess::GetOsInfo(QYString &strOSInfo)
{
	QYString strInfo;
	QYString strMinorVer;
	QYString strServPack;
	QYString strBuildNumber;

	int stat = 0;
	TCHAR buffer[20];
	TCHAR data[64];
	DWORD dataSize;
	DWORD win95Info;
	OSVERSIONINFO versionInfo;
	HKEY hKey;
	LONG result;

	// set the size of OSVERSIONINFO, before calling the function

	versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	// Get the version information

	if (!::GetVersionEx(&versionInfo))
	{
		return false;
	}

	// If NT ,then check if its server or workstation.

	if (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		strInfo = (_T("Windows NT"));

		dataSize = sizeof(data);

		result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			_T("System\\CurrentControlSet\\Control\\ProductOptions"),
			0, KEY_QUERY_VALUE, &hKey);

		// If there is error in opening the registry key, return

		if (result != ERROR_SUCCESS)
		{
			SetLastError(result);
			return false;
		}

		result = ::RegQueryValueEx(hKey, _T("ProductType"), NULL, NULL,
			(LPBYTE)data, &dataSize);

		// Make sure to close the reg key

		RegCloseKey(hKey);

		if (result != ERROR_SUCCESS)
		{
			SetLastError(result);
			return false;
		}

		// Check what string has been returned

		if (lstrcmpi(data, _T("WinNT")) == 0)
		{
			strInfo = _T("Windows NT Workstation");
		}
		else if (lstrcmpi(data, _T("ServerNT")) == 0)
		{
			strInfo = _T("Windows NT Server");
		}
		else
		{
			strInfo = _T("Windows NT Server - Domain Controller");
		}

		// Check the version number
		strMinorVer.Format(_T("%d.%d"), versionInfo.dwMajorVersion, versionInfo.dwMinorVersion);
		// Get the build number.

		SafeI2A(versionInfo.dwBuildNumber, buffer, 10);
		//strBuildNumber = QYString(buffer);
	}
	else if (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		strInfo = _T("Windows 95");

		if ((versionInfo.dwMajorVersion > 4) || ((versionInfo.dwMajorVersion == 4)
			&& (versionInfo.dwMinorVersion > 0)))
		{
			strInfo = _T("Windows 98");
		}

		// For win95, build number is low order word

		win95Info = (DWORD)(LOBYTE(LOWORD(versionInfo.dwBuildNumber)));

		// Get the build number.

		SafeI2A(win95Info, buffer, 10);
		//strBuildNumber = CString(buffer);
	}
	else
	{
		strInfo = _T("Windows 3.1");
	}

	// Get service pack information.

	strServPack = QYString(versionInfo.szCSDVersion);
	//strServPack.FreeExtra();

	strOSInfo = strInfo + _T(" Ver") + strMinorVer + _T(" Build") + strBuildNumber + _T(" ") + strServPack;
	return TRUE;
}

BOOL QYSysProcess::GetMemInfo(DWORD &dwTotalPhyMem, DWORD &dwFreePhyMem, DWORD &dwVirtualMem)
{
	MEMORYSTATUS memStatus;
	memStatus.dwLength = sizeof(MEMORYSTATUS);

	GlobalMemoryStatus(&memStatus);

	dwTotalPhyMem = memStatus.dwTotalPhys / 1024 / 1024;
	dwFreePhyMem = memStatus.dwAvailPhys / 1024 / 1024;
	dwVirtualMem = memStatus.dwAvailVirtual / 1024 / 1024;

	return TRUE;
}

BOOL QYSysProcess::GetMemUsage(DWORD &dwMemryUsage)
{
	MEMORYSTATUS memStatus;
	memStatus.dwLength = sizeof(MEMORYSTATUS);

	GlobalMemoryStatus(&memStatus);

	dwMemryUsage = memStatus.dwMemoryLoad;

	return TRUE;
}


BOOL QYSysProcess::GetCpuInfo(QYString &strCPUInfo)
{
	CRegKey rk;
	DWORD pCount = 1024;
	TCHAR szValue[1024];

	LPCTSTR lp = _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");

	if (rk.Open(HKEY_LOCAL_MACHINE, lp) == ERROR_SUCCESS)
	{
		LPCTSTR lKeyName = _T("ProcessorNameString");
		if (rk.QueryStringValue(szValue, (LPTSTR)lKeyName, &pCount) == ERROR_SUCCESS)
		{
			strCPUInfo = szValue;
			//strCPUInfo.Replace(_T("   "), _T(""));
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	rk.Close();

	return TRUE;
}

BOOL QYSysProcess::GetNetworkUsage(int &nNetUsage)
{
	MIB_IFTABLE *pIfTable = NULL;
	ULONG dwSize = 0;
	DWORD dwRet = 0;
	TCHAR *pTemp = nullptr;

	dwRet = GetIfTable(pIfTable, &dwSize, TRUE);
	if (dwRet == ERROR_INSUFFICIENT_BUFFER)
	{
		pTemp = new TCHAR[dwSize];
		pIfTable = (MIB_IFTABLE*)pTemp;

		if (pIfTable != NULL)
		{
			dwRet = GetIfTable(pIfTable, &dwSize, TRUE);
			if (dwRet == NO_ERROR)
			{
				__int64 dwTotalOutOctets = 0;
				__int64 dwTotalIntOctets = 0;
				int nCount = 0;
				__int64 nNetWidth = 0;
				for (int i = 0; i<pIfTable->dwNumEntries; i++)
				{
					if (pIfTable->table[i].dwSpeed > 0
						&& pIfTable->table[i].dwOutOctets > 0
						&& pIfTable->table[i].dwInOctets > 0)
					{
						dwTotalOutOctets += pIfTable->table[i].dwOutOctets;
						dwTotalIntOctets += pIfTable->table[i].dwInOctets;

						nCount++;
						nNetWidth = pIfTable->table[i].dwSpeed;
					}

				}

				if (0 < nCount)
				{
					__int64 dwSpeed = dwTotalOutOctets + dwTotalIntOctets - m_dwTotalIntOctets - m_dwTotalOutOctets;
					dwSpeed /= nCount;

					nNetUsage = dwSpeed * 8 * 100 / nNetWidth;

					if (100 < nNetUsage || 0 > nNetUsage)
					{
						nNetUsage = 0;
					}

					m_dwTotalIntOctets = dwTotalIntOctets;
					m_dwTotalOutOctets = dwTotalOutOctets;
				}
				else
				{
					nNetUsage = 0;
					m_dwTotalIntOctets = 0;
					m_dwTotalOutOctets = 0;
				}

			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}

		delete pIfTable;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL QYSysProcess::GetCpuUsage(int &nCpuUsage)
{
	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
	SYSTEM_TIME_INFORMATION        SysTimeInfo;
	SYSTEM_BASIC_INFORMATION       SysBaseInfo;
	double                         dbIdleTime;
	double                         dbSystemTime;
	LONG                           status;


	if (!m_NtQuerySystemInformation)
	{
		return FALSE;
	}

	// get number of processors in the system
	status = m_NtQuerySystemInformation(SystemBasicInformation,
		&SysBaseInfo, sizeof(SysBaseInfo), NULL);

	if (status != NO_ERROR)
	{
		return FALSE;
	}

	status = m_NtQuerySystemInformation(SystemTimeInformation,
		&SysTimeInfo, sizeof(SysTimeInfo), 0);
	if (status != NO_ERROR)
	{
		return FALSE;
	}

	// get new CPU's idle time
	status = m_NtQuerySystemInformation(SystemPerformanceInformation,
		&SysPerfInfo, sizeof(SysPerfInfo), NULL);
	if (status != NO_ERROR)
	{
		return FALSE;
	}

	// if it's a first call - skip it
	if (m_liOldIdleTime.QuadPart != 0)
	{
		// CurrentValue = NewValue - OldValue
		dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(m_liOldIdleTime);
		dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(m_liOldSystemTime);

		// CurrentCpuIdle = IdleTime / SystemTime
		dbIdleTime = dbIdleTime / dbSystemTime;

		// CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
		dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors + 0.5;

		nCpuUsage = (UINT)dbIdleTime;
	}

	// store new CPU's idle and system time
	m_liOldIdleTime = SysPerfInfo.liIdleTime;
	m_liOldSystemTime = SysTimeInfo.liKeSystemTime;
	return TRUE;
}

BOOL QYSysProcess::GetCpuUsage2(int &nCpuUsage)
{
	__int64 idleft, kernelft, userft;

	if (!GetSystemTimes((LPFILETIME)&idleft, (LPFILETIME)&kernelft, (LPFILETIME)&userft))
	{
		return FALSE;
	}

	__int64 usr = userft - m_dwuserTime;
	__int64 ker = kernelft - m_dwkernelTime;
	__int64 idl = idleft - m_dwidleTime;
	__int64 sys = (usr + ker);

	if (sys)
		nCpuUsage = int((sys - idl) * 100 / sys); // System Idle take 100 % of cpu;
	else
		nCpuUsage = 0;

	m_dwidleTime = idleft;
	m_dwkernelTime = kernelft;
	m_dwuserTime = userft;

	return TRUE;
}

BOOL QYSysProcess::IsWow64()
{
	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process;
	BOOL bIsWow64 = FALSE;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(_T("kernel32")), "IsWow64Process");
	if (NULL != fnIsWow64Process)
	{
		fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
	}
	return bIsWow64;
}


int QYSysProcess::GetInterFaceCount()
{
	try
	{
#define DEFAULT_BUFFER_SIZE 40960L   

		unsigned char *data = new unsigned char[DEFAULT_BUFFER_SIZE];
		DWORD type;
		DWORD size = DEFAULT_BUFFER_SIZE;
		DWORD ret;

		TCHAR s_key[4096];
		SafeSprintf(s_key, 4096, L"510");
		//RegQueryValueEx的固定调用格式           
		QYString str(s_key);

		//如果RegQueryValueEx函数执行失败则进入循环   
		while ((ret = RegQueryValueEx(HKEY_PERFORMANCE_DATA, str, 0, &type, data, &size)) != ERROR_SUCCESS)
		{
			Sleep(10);
			//如果RegQueryValueEx的返回值为ERROR_MORE_DATA(申请的内存区data太小，不能容纳RegQueryValueEx返回的数据)   
			if (ret == ERROR_MORE_DATA)
			{
				Sleep(10);
				size += DEFAULT_BUFFER_SIZE;

				delete[] data;
				data = new unsigned char[size];
				//data = (unsigned char*)realloc(data, size);//重新分配足够大的内存   

				ret = RegQueryValueEx(HKEY_PERFORMANCE_DATA, str, 0, &type, data, &size);//重新执行RegQueryValueEx函数   
			}
			//如果RegQueryValueEx返回值仍旧未成功则函数返回.....(注意内存泄露“free函数”~~~)。   
			//这个if保证了这个while只能进入一次~~~避免死循环   
			if (ret != ERROR_SUCCESS)
			{
				if (NULL != data)
				{
					free(data);
					data = NULL;
				}
				return 0;//0个接口   
			}
		}

		//函数执行成功之后就是对返回的data内存中数据的解析了，这个建议去查看MSDN有关RegQueryValueEx函数参数数据结构的说明   
		//得到数据块        
		PERF_DATA_BLOCK  *dataBlockPtr = (PERF_DATA_BLOCK *)data;
		//得到第一个对象   
		PERF_OBJECT_TYPE *objectPtr = (PERF_OBJECT_TYPE *)((BYTE *)dataBlockPtr + dataBlockPtr->HeaderLength);

		for (int a = 0; a<(int)dataBlockPtr->NumObjectTypes; a++)
		{
			TCHAR nameBuffer[255] = { 0 };
			if (objectPtr->ObjectNameTitleIndex == 510)
			{
				DWORD processIdOffset = ULONG_MAX;
				PERF_COUNTER_DEFINITION *counterPtr = (PERF_COUNTER_DEFINITION *)((BYTE *)objectPtr + objectPtr->HeaderLength);

				for (int b = 0; b<(int)objectPtr->NumCounters; b++)
				{
					if (counterPtr->CounterNameTitleIndex == 520)
						processIdOffset = counterPtr->CounterOffset;

					counterPtr = (PERF_COUNTER_DEFINITION *)((BYTE *)counterPtr + counterPtr->ByteLength);
				}

				if (processIdOffset == ULONG_MAX) {
					if (data != NULL)
					{
						free(data);
						data = NULL;
					}
					return 0;
				}

				PERF_INSTANCE_DEFINITION *instancePtr = (PERF_INSTANCE_DEFINITION *)((BYTE *)objectPtr + objectPtr->DefinitionLength);

				for (int b = 0; b<objectPtr->NumInstances; b++)
				{
					wchar_t *namePtr = (wchar_t *)((BYTE *)instancePtr + instancePtr->NameOffset);
					PERF_COUNTER_BLOCK *counterBlockPtr = (PERF_COUNTER_BLOCK *)((BYTE *)instancePtr + instancePtr->ByteLength);

					//TCHAR pName[256] = { 0 };
					//WideCharToMultiByte(CP_ACP, 0, namePtr, -1, pName, sizeof(nameBuffer), 0, 0);

					DWORD bandwith = *((DWORD *)((BYTE *)counterBlockPtr + processIdOffset));
					DWORD tottraff = 0;

					Interfaces.push_back(QYString(namePtr)); //各网卡的名称   
					Bandwidths.push_back(bandwith);       //带宽   
					TotalTraffics.push_back(tottraff);    // 流量初始化为0   

					PERF_COUNTER_BLOCK  *pCtrBlk = (PERF_COUNTER_BLOCK *)((BYTE *)instancePtr + instancePtr->ByteLength);


					instancePtr = (PERF_INSTANCE_DEFINITION *)((BYTE *)instancePtr + instancePtr->ByteLength + pCtrBlk->ByteLength);
				}
			}
			objectPtr = (PERF_OBJECT_TYPE *)((BYTE *)objectPtr + objectPtr->TotalByteLength);
		}
		if (data != NULL)
		{
			free(data);
			data = NULL;
		}
	}
	catch (...)
	{
		return 0;
	}
	return Interfaces.size();
}

void QYSysProcess::GetInterFaceName(QYString &InterfaceName, int pNum)
{
	InterfaceName = Interfaces.at(pNum);

	DWORD dwBandwidth = Bandwidths.at(pNum);

	QYString str;
	str.Format(_T("%d"), dwBandwidth);

	InterfaceName = InterfaceName + str;
}


void QYSysProcess::GetDisplayCardInfo(DWORD &dwNum, QYString chCardName[])
{
	HKEY keyServ;
	HKEY keyEnum;
	HKEY key;
	HKEY key2;
	LONG lResult;//LONG型变量－保存函数返回值   

	//查询"SYSTEM\\CurrentControlSet\\Services"下的所有子键保存到keyServ   
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services"), 0, KEY_READ, &keyServ);
	if (ERROR_SUCCESS != lResult)
		return;


	//查询"SYSTEM\\CurrentControlSet\\Enum"下的所有子键保存到keyEnum   
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Enum"), 0, KEY_READ, &keyEnum);
	if (ERROR_SUCCESS != lResult)
		return;

	int i = 0, count = 0;
	DWORD size = 0, type = 0;
	for (;; ++i)
	{
		Sleep(5);
		size = 512;
		TCHAR name[512] = { 0 };//保存keyServ下各子项的字段名称   

		//逐个枚举keyServ下的各子项字段保存到name中   
		lResult = RegEnumKeyEx(keyServ, i, name, &size, NULL, NULL, NULL, NULL);

		//要读取的子项不存在，即keyServ的子项全部遍历完时跳出循环   
		if (lResult == ERROR_NO_MORE_ITEMS)
			break;

		//打开keyServ的子项字段为name所标识的字段的值保存到key   
		lResult = RegOpenKeyEx(keyServ, name, 0, KEY_READ, &key);
		if (lResult != ERROR_SUCCESS)
		{
			RegCloseKey(keyServ);
			return;
		}


		size = 512;
		//查询key下的字段为Group的子键字段名保存到name   
		lResult = RegQueryValueEx(key, TEXT("Group"), 0, &type, (LPBYTE)name, &size);
		if (lResult == ERROR_FILE_NOT_FOUND)
		{
			//?键不存在   
			RegCloseKey(key);
			continue;
		};



		//如果查询到的name不是Video则说明该键不是显卡驱动项   
		if (_tcscmp(TEXT("Video"), name) != 0)
		{
			RegCloseKey(key);
			continue;     //返回for循环   
		};

		//如果程序继续往下执行的话说明已经查到了有关显卡的信息，所以在下面的代码执行完之后要break第一个for循环，函数返回   
		lResult = RegOpenKeyEx(key, TEXT("Enum"), 0, KEY_READ, &key2);
		RegCloseKey(key);
		key = key2;
		size = sizeof(count);
		lResult = RegQueryValueEx(key, TEXT("Count"), 0, &type, (LPBYTE)&count, &size);//查询Count字段（显卡数目）   

		dwNum = count;//保存显卡数目   
		for (int j = 0; j <count; ++j)
		{
			TCHAR sz[512] = { 0 };
			TCHAR name[64] = { 0 };
			wsprintf(name, TEXT("%d"), j);
			size = sizeof(sz);
			lResult = RegQueryValueEx(key, name, 0, &type, (LPBYTE)sz, &size);


			lResult = RegOpenKeyEx(keyEnum, sz, 0, KEY_READ, &key2);
			if (ERROR_SUCCESS)
			{
				RegCloseKey(keyEnum);
				return;
			}


			size = sizeof(sz);
			lResult = RegQueryValueEx(key2, TEXT("FriendlyName"), 0, &type, (LPBYTE)sz, &size);
			if (lResult == ERROR_FILE_NOT_FOUND)
			{
				size = sizeof(sz);
				lResult = RegQueryValueEx(key2, TEXT("DeviceDesc"), 0, &type, (LPBYTE)sz, &size);
				chCardName[j] = sz;//保存显卡名称   
			};
			RegCloseKey(key2);
			key2 = NULL;
		};
		RegCloseKey(key);
		key = NULL;
		break;
	}
}


BOOL  QYSysProcess::IsInSysProcess(QYString strExeFile, DWORD &proID)	//枚举系统进程
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);

	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		//printf("CreateToolhelp32Snapshot failed! \n");
		return FALSE;
	}

	BOOL bMore = ::Process32First(hProcessSnap, &pe32);

	while (bMore)
	{
		if (strExeFile == QYString(pe32.szExeFile))
		{
			proID = pe32.th32ProcessID;
			return TRUE;
		}
		bMore = ::Process32Next(hProcessSnap, &pe32);
	}

	::CloseHandle(hProcessSnap);
	return FALSE;
}