#pragma once

#include "qydefine.h"

#include <vector>
#include <algorithm>
#include "QYString.h"
#include <list>

class QYUI_EXPORT_CLASS QYSysProcess
{
public:
	QYSysProcess();
	virtual ~QYSysProcess();
	int GetAllAvailableDisk(QYString &strAllAvalableDisk);
	BOOL GetAvailableRecDisk(QYString strCurRecDisk, int nSwitchDiskFreeSpace, int nSysDiskFreeSpace, QYString &strAvailabeRecDisk);
	float GetDiskFreeSpace(TCHAR szDisk);
	float GetDiskFreeSpace(QYString strDisk);
	QYString  GetSysDisk();
	BOOL IsDiskAvailable(QYString strCurRecDisk, int nSwitchDiskFreeSpace, int nSysDiskFreeSpace);
	int   GetDriverCount();
	float GetTotalDiskSpace();
	float GetUsedDiskSpace();
	BOOL	FindDir(QYString strDir, QYString strCompareFolder, std::vector<QYString>&vecFolderNeedDelete);
	BOOL	FindDir(QYString strDir, std::vector<QYString>&vecFolderCannotDelete, QYString &strFolder);
	BOOL	DeleteDirectory(TCHAR *pDirName);
	BOOL	GetOsInfo(QYString &strOSInfo);
	BOOL	GetMemInfo(DWORD &dwTotalPhyMem, DWORD &dwFreePhyMem, DWORD &dwVirtualMem);
	BOOL    GetMemUsage(DWORD &dwMemryUsage);
	BOOL	GetCpuInfo(QYString &strCPUInfo);
	BOOL	GetCpuUsage(int &nCpuUsage);
	BOOL	GetCpuUsage2(int &nCpuUsage);
	BOOL    GetNetworkUsage(int &nNetUsage);
	BOOL    IsWow64();//判断是否为64位操作系统 
	int     GetInterFaceCount();
	void    GetInterFaceName(QYString &InterfaceName, int pNum);
	/****获取显卡信息*****/
	void    GetDisplayCardInfo(DWORD &dwNum, QYString chCardName[]);
	BOOL  IsInSysProcess(QYString strExeFile, DWORD &proID);
private:
	typedef LONG(WINAPI *PROCNTQSI)(UINT, PVOID, ULONG, PULONG);
	LARGE_INTEGER   m_liOldIdleTime;
	LARGE_INTEGER   m_liOldSystemTime;
	PROCNTQSI       m_NtQuerySystemInformation;
	std::vector<QYString> Interfaces;
	//CStringList Interfaces;                       //保存所有网卡的名字   
	std::vector < DWORD >       Bandwidths;   //各网卡的带宽   
	std::vector <  DWORD >       TotalTraffics;    //各网卡的总流量   
	static __int64 m_dwidleTime;
	static __int64 m_dwkernelTime;
	static __int64 m_dwuserTime;
	static __int64 m_dwTotalOutOctets;
	static __int64 m_dwTotalIntOctets;
};

