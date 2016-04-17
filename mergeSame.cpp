//windres demo.rc demo.o
//g++ mergeSame.cpp icon.o -o mergeSame --std=c++11 -static
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include "sha1.h"
#include "traverse.h"
using namespace std;

void collectHash(const string &fileName, const WIN32_FIND_DATA &fileInfo, void* para)
{
	if(fileInfo.nFileSizeLow + fileInfo.nFileSizeHigh == 0)
	{
		return;
	}
	auto dic = (unordered_map<string, vector<string>>*)para;
	int err;
	string has = SHA1(fileName, err);
	if(err)
	{
		cout << "Get SHA1 Err #" << err << " :" << fileName << endl;
	}
	else
	{
		(*dic)[has].push_back(fileName);
	}
}

void rebuild(unordered_map<string, vector<string>> &dic, string root)
{
	root += "\\mergeSame";

	string mkdirCmd = "mkdir \"" + root + "\"";
	system(mkdirCmd.c_str());
	ofstream flog((root + "\\log.txt").c_str());

	ofstream fext((root + "\\extract.bat").c_str());
	mkdirCmd = "mkdir \"" + (root.front() == '.' ? "." + root : root) + "\\" + "extract" + "\"";
	fext << mkdirCmd << endl;
	for(auto i : dic)
	{
		if(i.second.size() < 2)
		{
			continue;
		}
		mkdirCmd = "mkdir \"" + root + "\\" + i.first + "\"";
		system(mkdirCmd.c_str());
		for(int j = 0; j < i.second.size(); ++j)
		{
			string fileName = i.second[j].substr(i.second[j].find_last_of('\\') + 1);
			string newName = root + '\\' + i.first + '\\' + (char)(j + '0') + '_' + fileName;
			string moveCmd = "move \"" + i.second[j] + "\" \"" + newName + "\"";
			cout << moveCmd << endl;
			system(moveCmd.c_str());
			flog << moveCmd << endl;
			if(j == 0)
			{
				string extractName = root + "\\extract\\" + fileName;
				if(newName.front() == '.')
				{
					newName = "." + newName;
					extractName = "." + extractName;
				}
				moveCmd = "move \"" + newName + "\" \"" + extractName + "\"";
				fext << moveCmd << endl;
			}
		}
	}
	fext << "del extract.bat" << endl;
	fext.close();
	flog.close();
}

int main(int argc, char *argv[])
{
	string root = (argc == 2 ? argv[1] : ".");
	unordered_map<string, vector<string>> dic;
	traverse(root, collectHash, &dic);
	rebuild(dic, root);
	system("pause");
	return 0;
}
