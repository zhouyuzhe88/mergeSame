#include <windows.h>
#include <string>

using namespace std;

typedef void(delegate)(const string &fileName, const WIN32_FIND_DATA &fileInfo, void *para);
bool traverse(string path, delegate dFile, void *paraForFile = NULL, 
		delegate dDir = NULL,  void *paraForDir = NULL)
{
	WIN32_FIND_DATA fileInfo;
	HANDLE fFind = FindFirstFile((path + "\\*.*").c_str(), &fileInfo);
	if(fFind == INVALID_HANDLE_VALUE)
	{
		FindClose(fFind);
		return false;
	}
	do
	{
		string fileName = fileInfo.cFileName;
		if(fileName == "." || fileName == "..")
		{
			continue;
		}
		fileName = path + "\\" + fileName;
		if(fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(dDir)
			{
				dDir(fileName, fileInfo, paraForDir);
			}
			traverse(fileName, dFile, paraForFile, dDir, paraForDir);
		}
		else
		{
			if(dFile)
			{
				dFile(fileName, fileInfo, paraForFile);
			}
		}
	}while(FindNextFile(fFind, &fileInfo));
	FindClose(fFind);
	return true;
}
