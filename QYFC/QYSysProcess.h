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
	BOOL    IsWow64();//�ж��Ƿ�Ϊ64λ����ϵͳ 
	int     GetInterFaceCount();
	void    GetInterFaceName(QYString &InterfaceName, int pNum);
	/****��ȡ�Կ���Ϣ*****/
	void    GetDisplayCardInfo(DWORD &dwNum, QYString chCardName[]);
	BOOL  IsInSysProcess(QYString strExeFile, DWORD &proID);
private:
	typedef LONG(WINAPI *PROCNTQSI)(UINT, PVOID, ULONG, PULONG);
	LARGE_INTEGER   m_liOldIdleTime;
	LARGE_INTEGER   m_liOldSystemTime;
	PROCNTQSI       m_NtQuerySystemInformation;
	std::vector<QYString> Interfaces;
	//CStringList Interfaces;                       //������������������   
	std::vector < DWORD >       Bandwidths;   //�������Ĵ���   
	std::vector <  DWORD >       TotalTraffics;    //��������������   
	static __int64 m_dwidleTime;
	static __int64 m_dwkernelTime;
	static __int64 m_dwuserTime;
	static __int64 m_dwTotalOutOctets;
	static __int64 m_dwTotalIntOctets;
};

