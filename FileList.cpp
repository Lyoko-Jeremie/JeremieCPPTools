#include "FileList.h"

#include <string>
#include <vector>
//#include <iostream>
#include <stdexcept>
using namespace std;

// �����������װ��
//#include "CLogErr.h"    // ����
//using namespace CLogErr::clog;
// ���滻Ϊ����
// #define clogerr std::clog

#include <windows.h>
#include <winbase.h>    // ����API

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
            // �����ļ���
//            clogerr << "�ļ���\t\t" << this->ReadStruct.cFileName << endl;
            this->MainFileList.push_back( this->ReadStruct.cFileName );
        }else{
            // ���ļ���
            // ȥ��.&..
            string Name(this->ReadStruct.cFileName);
            if ( Name == "." || Name == ".." )
            {
//                clogerr << "��ʶ����\t" << Name << endl;
            }else{
//                clogerr << "�ļ��У�\t" << Name << endl;
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
    vector<string> FList;   // �ļ��б�
    vector<string> DList;   // �ļ����б�
//    clogerr << "�����Ա�׼��ʽ������" << endl;   // ��׼��ʽֻ�����ļ�
//    clogerr << "��ʼ������" << endl;
    FileList TB( AnalyzeName );

/*
////    clogerr << "��׼��ʽ�����ɹ���" << endl;
//    clogerr << "��������ļ�������" << TB.GeFiletSize() << endl;
//    for ( size_t i = 0; i != TB.GeFiletSize(); ++i)
//    {
//        clogerr << TB.GetFileFullPath(i)  << endl;
//        FList.push_back( TB.GetFileFullPath(i) );
//    }
//    clogerr << "��������ļ���������" << TB.GeFiletSize() << endl;
//    for ( size_t i = 0; i != TB.GeDirectorySize(); ++i)
//    {
//        clogerr << TB.GetDirectoryFullPath(i) << endl;
//        DList.push_back( TB.GetDirectoryFullPath(i) );
//    }
*/


    if ( TB.GetFiletSize() != 0 )
    {   // ��׼��ʽֻ����һ���ļ�
//        clogerr << "��׼��ʽ�����ɹ���" << endl;
//        clogerr << "��׼��ʽ��������ļ�������" << TB.GetFiletSize() << endl;
        if ( TB.GetFiletSize() > 1 )
        {
//            cout << " !!!!!!!!!!!!!! ʹ���˲�֧�ֵı��ʽ�� !!!!!!!!!!!!!! " << endl;
//            cout << AnalyzeName << endl;
        }
//        clogerr << TB.GetFileOnlyName(0) << endl;
        FList.push_back( AnalyzeName );

//        for ( size_t i = 0; i != TB.GetFiletSize(); ++i)    // ��׼��ʽֻ�����ļ�
//        {
//            clogerr << TB.GetFileOnlyName(i) << endl;
//            FList.push_back( TB.GetFileOnlyName(i) );
//        }
//        clogerr << "��׼��ʽ��������ļ���������" << TB.GetFiletSize() << endl;
//        for ( size_t i = 0; i != TB.GetDirectorySize(); ++i)
//        {
//            DList.push_back(AnalyzeName + "\\" + TB.GetDirectoryOnlyName(i));
//        }

    }else{
//        clogerr << "��׼��ʽ����ʧ�ܣ��������ļ��з�ʽ������" << endl;
        FileList TD( AnalyzeName + "\\*" );     // ����ļ���ƥ���
        if ( TD.GetFiletSize() != 0 || TD.GetDirectorySize() != 0 )
        {
//            clogerr << "�ļ��з�ʽ�����ɹ���" << endl;
//            clogerr << "�ļ��з�ʽ��������ļ�������" << TD.GetFiletSize() << endl;
            for ( size_t i = 0; i != TD.GetFiletSize(); ++i)
            {
//                clogerr <<  AnalyzeName + "\\" + TD.GetFileOnlyName(i) << endl;
                FList.push_back( AnalyzeName + "\\" + TD.GetFileOnlyName(i) );
            }
//            clogerr << "�ļ��з�ʽ��������ļ���������" << TB.GetDirectorySize() << endl;
            for ( size_t i = 0; i != TD.GetDirectorySize(); ++i)
            {
//                clogerr << AnalyzeName + "\\" + TD.GetDirectoryOnlyName(i) << endl;
                DList.push_back(AnalyzeName + "\\" + TD.GetDirectoryOnlyName(i));
            }
        }else{
//            cout << " !!!!!!!!!!!!!! ʹ���˲�֧�ֵ��ַ�����ʽ �� �˴�û���ļ����ļ��� !!!!!!!!!!!!!! " << endl;
//            cout << AnalyzeName << endl;
//            clogerr << "�ļ��з�ʽ����ʧ�ܡ�" << endl;
        }
    }

    // �������ļ���    �ݹ�
    if ( DList.size() != 0 )
    {
//        clogerr << "DList.size():" << DList.size() << endl;
//        clogerr << "���������������ļ��С�" << endl;
        for ( string &a: DList)
        {
            FileListAnalyze( a, FList);
        }
    }

    // �ϲ�
    AppendList.insert( AppendList.end(), FList.begin(), FList.end() );
    return true;
}











// ���·��
string GetPath(string arg0)
{
#ifndef NDEBUG
	size_t pos = arg0.rfind('\\');
	//clogerr << " \" \\ \" ���һ�γ��ֵ�λ�ã�" << pos << endl;
	string temp(arg0.substr(0, pos) + "\\");
	//clogerr << "��ȡ�����" << temp << endl << endl << endl;
	return temp;
#else
	return arg0.substr(0, arg0.rfind('\\')) + "\\";
#endif // NDEBUG
}





// ��ȡ�ļ������� ������׺
string GetFileFullName(string arg0)
{
#ifndef NDEBUG
	size_t pos = arg0.rfind('\\');
	//clogerr << " \" \\ \" ���һ�γ��ֵ�λ�ã�" << pos << endl;
	string temp(arg0.substr(pos + 1, arg0.size()));
	//clogerr << "��ȡ�����" << temp << endl << endl << endl;
	return temp;
#else
	return arg0.substr(arg0.rfind('\\') + 1, arg0.size());
#endif // NDEBUG
}



// ��ȡ�ļ��� �޺�׺
string GetFileName(string FileFullName)
{
#ifndef NDEBUG
	size_t pos = FileFullName.rfind('.');
	//clogerr << " \" . \" ���һ�γ��ֵ�λ�ã�" << pos << endl;
	string temp(FileFullName.substr(0, pos));
	//clogerr << "��ȡ�����" << temp << endl << endl << endl;
	return temp;
#else
	return FileFullName.substr(0, FileFullName.rfind('.'));
#endif // NDEBUG
}








