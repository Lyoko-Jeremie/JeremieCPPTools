#include "FileList.h"

#include <string>
#include <vector>
//#include <iostream>
#include <stdexcept>
using namespace std;

// 测试用输出包装类
//#include "CLogErr.h"    // 依赖
//using namespace CLogErr::clog;
// 可替换为如下
// #define clogerr std::clog

#include <windows.h>
#include <winbase.h>    // 核心API

FileList::FileList( string path ):
    AllReady(false),
    MainPath(path),
    MainFileList(),
    MainDirectoryList(),
    ReadHandle(nullptr),
    ReadStruct(),
    pReadStruct(&ReadStruct)
{
    this->CtorList();
}

FileList::~FileList()
{
    this->AllReady = false;
}

bool FileList::ReFlash()
{
    this->MainFileList.clear();
    return this->CtorList();
}

string FileList::GetFileOnlyName(const unsigned int i)
{
//    clogerr << "FileListSize: " << this->GetSize() << endl;
    if ( i < this->GetFiletSize() )
    {
        return this->MainFileList.at(i);
    }
//    clogerr << "FileListSize: " << this->GetFiletSize() << " < " << i << endl;
    throw out_of_range("FileList:GetFileOnlyName:OutOfRange: " + i );
    return "";
}

string FileList::GetFileFullPath(const unsigned int i)
{
    return this->MainPath + this->GetFileOnlyName(i);
}

size_t FileList::GetFiletSize()
{
    return this->MainFileList.size();
}


bool FileList::CtorList()
{
    this->ReadHandle = FindFirstFile( this->MainPath.c_str(), this->pReadStruct);
    if ( INVALID_HANDLE_VALUE == this->ReadHandle )
    {
//        clogerr << "FileList:CtorList:ReadHandle:INVALID_HANDLE_VALUE{MayBe NotFind Or PathWrong Or UnsupportedCharacters Or PathLengthMoreThan256Character}" << endl;
        this->pReadStruct = nullptr;
        this->AllReady = false;
        return false;
    }
    do{
        int t = GetLastError();
        if ( ERROR_FILE_NOT_FOUND == t )
        {
//            clogerr << "FileList:CtorList:ERROR_FILE_NOT_FOUND" << endl;
        }
        if (  !(this->ReadStruct.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  )
        {
            // 不是文件夹
//            clogerr << "文件：\t\t" << this->ReadStruct.cFileName << endl;
            this->MainFileList.push_back( this->ReadStruct.cFileName );
        }else{
            // 是文件夹
            // 去除.&..
            string Name(this->ReadStruct.cFileName);
            if ( Name == "." || Name == ".." )
            {
//                clogerr << "标识符：\t" << Name << endl;
            }else{
//                clogerr << "文件夹：\t" << Name << endl;
                this->MainDirectoryList.push_back( Name );
            }
        }

    }while ( FindNextFile( this->ReadHandle, this->pReadStruct ) );

    if ( !FindClose(this->ReadHandle) )
    {
//        clogerr << "CloseFail" << endl;
        return false;
    }
    this->ReadHandle = nullptr;
    this->AllReady = true;
    return true;
}





string FileList::GetDirectoryOnlyName(const unsigned int i)
{
    if ( i < this->GetDirectorySize() )
    {
        return this->MainDirectoryList.at(i);
    }
//    clogerr << "DirectorySize: " << this->GetDirectorySize() << " < " << i << endl;
    throw out_of_range("FileList:GetDirectoryOnlyName:OutOfRange: " + i );
    return "";
}

string FileList::GetDirectoryFullPath(const unsigned int i)
{
    return this->MainPath + this->GetDirectoryOnlyName(i);
}

size_t FileList::GetDirectorySize()
{
    return this->MainDirectoryList.size();
}

size_t FileList::GetAllSize()
{
    return this->GetDirectorySize() + this->GetFiletSize();
}



bool FileListAnalyze( const string &AnalyzeName, vector<string> &AppendList)
{
    vector<string> FList;   // 文件列表
    vector<string> DList;   // 文件夹列表
//    clogerr << "尝试以标准方式解析。" << endl;   // 标准方式只解析文件
//    clogerr << "开始解析。" << endl;
    FileList TB( AnalyzeName );

/*
////    clogerr << "标准方式解析成功。" << endl;
//    clogerr << "解析结果文件数量：" << TB.GeFiletSize() << endl;
//    for ( size_t i = 0; i != TB.GeFiletSize(); ++i)
//    {
//        clogerr << TB.GetFileFullPath(i)  << endl;
//        FList.push_back( TB.GetFileFullPath(i) );
//    }
//    clogerr << "解析结果文件夹数量：" << TB.GeFiletSize() << endl;
//    for ( size_t i = 0; i != TB.GeDirectorySize(); ++i)
//    {
//        clogerr << TB.GetDirectoryFullPath(i) << endl;
//        DList.push_back( TB.GetDirectoryFullPath(i) );
//    }
*/


    if ( TB.GetFiletSize() != 0 )
    {   // 标准方式只解析一个文件
//        clogerr << "标准方式解析成功。" << endl;
//        clogerr << "标准方式解析结果文件数量：" << TB.GetFiletSize() << endl;
        if ( TB.GetFiletSize() > 1 )
        {
//            cout << " !!!!!!!!!!!!!! 使用了不支持的表达式。 !!!!!!!!!!!!!! " << endl;
//            cout << AnalyzeName << endl;
        }
//        clogerr << TB.GetFileOnlyName(0) << endl;
        FList.push_back( AnalyzeName );

//        for ( size_t i = 0; i != TB.GetFiletSize(); ++i)    // 标准方式只解析文件
//        {
//            clogerr << TB.GetFileOnlyName(i) << endl;
//            FList.push_back( TB.GetFileOnlyName(i) );
//        }
//        clogerr << "标准方式解析结果文件夹数量：" << TB.GetFiletSize() << endl;
//        for ( size_t i = 0; i != TB.GetDirectorySize(); ++i)
//        {
//            DList.push_back(AnalyzeName + "\\" + TB.GetDirectoryOnlyName(i));
//        }

    }else{
//        clogerr << "标准方式解析失败，尝试以文件夹方式解析。" << endl;
        FileList TD( AnalyzeName + "\\*" );     // 添加文件夹匹配符
        if ( TD.GetFiletSize() != 0 || TD.GetDirectorySize() != 0 )
        {
//            clogerr << "文件夹方式解析成功。" << endl;
//            clogerr << "文件夹方式解析结果文件数量：" << TD.GetFiletSize() << endl;
            for ( size_t i = 0; i != TD.GetFiletSize(); ++i)
            {
//                clogerr <<  AnalyzeName + "\\" + TD.GetFileOnlyName(i) << endl;
                FList.push_back( AnalyzeName + "\\" + TD.GetFileOnlyName(i) );
            }
//            clogerr << "文件夹方式解析结果文件夹数量：" << TB.GetDirectorySize() << endl;
            for ( size_t i = 0; i != TD.GetDirectorySize(); ++i)
            {
//                clogerr << AnalyzeName + "\\" + TD.GetDirectoryOnlyName(i) << endl;
                DList.push_back(AnalyzeName + "\\" + TD.GetDirectoryOnlyName(i));
            }
        }else{
//            cout << " !!!!!!!!!!!!!! 使用了不支持的字符或表达式 或 此处没有文件和文件夹 !!!!!!!!!!!!!! " << endl;
//            cout << AnalyzeName << endl;
//            clogerr << "文件夹方式解析失败。" << endl;
        }
    }

    // 解析子文件夹    递归
    if ( DList.size() != 0 )
    {
//        clogerr << "DList.size():" << DList.size() << endl;
//        clogerr << "继续遍历解析子文件夹。" << endl;
        for ( string &a: DList)
        {
            FileListAnalyze( a, FList);
        }
    }

    // 合并
    AppendList.insert( AppendList.end(), FList.begin(), FList.end() );
    return true;
}











// 拆分路径
string GetPath(string arg0)
{
#ifndef NDEBUG
	size_t pos = arg0.rfind('\\');
	//clogerr << " \" \\ \" 最后一次出现的位置：" << pos << endl;
	string temp(arg0.substr(0, pos) + "\\");
	//clogerr << "截取结果：" << temp << endl << endl << endl;
	return temp;
#else
	return arg0.substr(0, arg0.rfind('\\')) + "\\";
#endif // NDEBUG
}





// 获取文件完整名 包括后缀
string GetFileFullName(string arg0)
{
#ifndef NDEBUG
	size_t pos = arg0.rfind('\\');
	//clogerr << " \" \\ \" 最后一次出现的位置：" << pos << endl;
	string temp(arg0.substr(pos + 1, arg0.size()));
	//clogerr << "截取结果：" << temp << endl << endl << endl;
	return temp;
#else
	return arg0.substr(arg0.rfind('\\') + 1, arg0.size());
#endif // NDEBUG
}



// 获取文件名 无后缀
string GetFileName(string FileFullName)
{
#ifndef NDEBUG
	size_t pos = FileFullName.rfind('.');
	//clogerr << " \" . \" 最后一次出现的位置：" << pos << endl;
	string temp(FileFullName.substr(0, pos));
	//clogerr << "截取结果：" << temp << endl << endl << endl;
	return temp;
#else
	return FileFullName.substr(0, FileFullName.rfind('.'));
#endif // NDEBUG
}








