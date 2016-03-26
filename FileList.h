#ifndef FILELIST_H
#define FILELIST_H

#include <string>
#include <vector>
using namespace std;

#include <windows.h>
#include <winbase.h>    // 核心API

class FileList
{
    public:
        FileList( string path);     // 文件搜索路径   包含掩码
        virtual ~FileList();
        bool ReFlash();
        string GetFileOnlyName( const unsigned int i);
        // FullPath只有在不使用通配符模式下才返回有效数据
        string GetFileFullPath( const unsigned int i);
        size_t GetFiletSize();
        string GetDirectoryOnlyName( const unsigned int i);
        string GetDirectoryFullPath( const unsigned int i);
        size_t GetDirectorySize();
        size_t GetAllSize();
    protected:
        bool AllReady;
        string MainPath;
        vector<string> MainFileList;
        vector<string> MainDirectoryList;
        HANDLE ReadHandle;
        WIN32_FIND_DATA ReadStruct;
        LPWIN32_FIND_DATA pReadStruct;
        bool CtorList();
    private:
};

bool extern FileListAnalyze( const string &AnalyzeName, vector<string> &AppendList);

// 拆分路径
string GetPath(string arg0);

// 获取文件完整名 包括后缀
string GetFileFullName(string arg0);

// 获取文件名 无后缀
string GetFileName(string FileFullName);



#endif // FILELIST_H
