/*
    Image Uploader - program for uploading images/files to Internet
    Copyright (C) 2007-2011 ZendeN <zenden2k@gmail.com>
	 
    HomePage:    http://zenden.ws/imageuploader

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "FileDownloader.h"
#include "../myutils.h"
#include <algorithm>
#include "../Common.h"

CFileDownloader::CFileDownloader()
{
	m_nThreadCount = 4;
	m_NeedStop = false;
	m_IsRunning = false;
	m_nRunningThreads = 0;
}

void CFileDownloader::AddFile(const std::string& url, void* id)
{
	m_CS.Lock();
	DownloadFileListItem newItem;
	newItem.url = url;
	newItem.id = reinterpret_cast<void*>(id);
	m_fileList.push_back(newItem);
	m_CS.Unlock();
}

void CFileDownloader::setThreadCount(int n)
{
	m_nThreadCount = n;
}

bool CFileDownloader::start()
{
	m_NeedStop = false;
	
	m_CS.Lock();
	size_t numThreads = min(m_nThreadCount - m_nRunningThreads, m_fileList.size());

	for(size_t i=0; i<numThreads; i++)
	{
		m_nRunningThreads++;
		m_IsRunning = true;
		m_hThreads.push_back((HANDLE)_beginthreadex(0,0,thread_func, this,0,0));
	}
	m_CS.Unlock();
	return 0;
}
unsigned int __stdcall CFileDownloader::thread_func(void * param)
{
	CFileDownloader * p = reinterpret_cast<CFileDownloader*>(param);
	p->memberThreadFunc();
	return 0;
}

void CFileDownloader::memberThreadFunc()
{
	NetworkManager nm;
	// Providing callback function to stop downloading
	nm.setProgressCallback(CFileDownloader::ProgressFunc, this);
	m_CS.Lock();
		if(onConfigureNetworkManager)
			onConfigureNetworkManager(&nm);
	m_CS.Unlock();

	for(;;)
	{
		DownloadFileListItem curItem;
		if(!getNextJob(curItem)) break;
		std::string url = curItem.url;
		if(url.empty()) break; 

		nm.setOutputFile( curItem.fileName);
		nm.doGet(url);
		if(m_NeedStop) break;
		m_CS.Lock();
		if(nm.responseCode()>=200 && nm.responseCode()<=299)
		{
			
			std::string name = IuCoreUtils::ExtractFileName(url);
			if(!onFileFinished.empty())
				onFileFinished(true, curItem); // delegate call
		}
		else
		{
			if(!onFileFinished.empty())
				onFileFinished(false, curItem); // delegate call
		}
		m_CS.Unlock();

	}
	if(m_NeedStop) return ;
	m_CS.Lock();
	m_nRunningThreads--;
	if(!m_nRunningThreads)
	{
		m_IsRunning = false;
		if(onQueueFinished)
			onQueueFinished(); // delegate call
	}
	HANDLE hThread = GetCurrentThread();
	for(size_t i=0; i<m_hThreads.size(); i++)
	{
		if(m_hThreads[i] == hThread)
		{
			m_hThreads.erase(m_hThreads.begin() + i);
			break;
		}
	}
	m_CS.Unlock();
	return ;

}
bool CFileDownloader::getNextJob(DownloadFileListItem& item)
{
	bool result = false;
	
	m_CS.Lock();
	if(!m_fileList.empty() && !m_NeedStop)
	{
		item = *m_fileList.begin();
		std::string url;
		url = item.url;
		m_fileList.erase(m_fileList.begin());
		std::string ext = IuCoreUtils::ExtractFileExt(url);
		std::string fileName =  IuCoreUtils::ExtractFileName(url);
		CString wFileName = GetUniqFileName(IUTempFolder+Utf8ToWstring(fileName.c_str()).c_str());
		std::string filePath= WCstringToUtf8(wFileName);
		// Creating file
		FILE *f = _tfopen(wFileName, L"wb");
		if(f) fclose(f);
		 item.fileName = filePath;
		result = true;
	}
	
	m_CS.Unlock();
	return result;
}

void CFileDownloader::stop()
{
	m_NeedStop = true;
}

bool CFileDownloader::IsRunning()
{
	return m_IsRunning;
}

bool CFileDownloader::waitForFinished(unsigned int msec)
{
	if(!m_IsRunning) return true;
	int nCount = m_hThreads.size();
	if(!nCount) return true;
	m_CS.Lock();
	HANDLE* threads = new HANDLE[m_hThreads.size()];
	memcpy(threads, &m_hThreads[0], sizeof(HANDLE) * nCount);
	m_CS.Unlock();

	DWORD res = WaitForMultipleObjects(nCount, threads, FALSE, msec);
	if(res == WAIT_TIMEOUT || WAIT_FAILED)
		return false;
	else return true;
}

void CFileDownloader::kill()
{
	if(!m_IsRunning) return;
	m_CS.Lock();

	HANDLE hThread = 0;
	for(size_t i=0; i<m_hThreads.size(); i++)
	{
		hThread = m_hThreads[i];
		TerminateThread(hThread, -1);
		CloseHandle(hThread);
		break;
	}
	m_hThreads.clear();
	
	m_CS.Unlock();
}

int CFileDownloader::ProgressFunc (void* userData, double dltotal,double dlnow,double ultotal, double ulnow)
{
	CFileDownloader * fd = reinterpret_cast<CFileDownloader*>(userData);
	if(fd->m_NeedStop) return -1;
	return 0;
}
