#ifndef CMDANALYZETOOLS_H
#define CMDANALYZETOOLS_H

#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <algorithm>
#include <map>
using namespace std;

#define CAT_MainFunctionComand const int CAT_CmdV, const char ** CAT_CmdS
#define CAT_CmdAnalyzeToolsInitFunctionComand CAT_CmdV, CAT_CmdS

class CmdAnalyzeTools
{
    public:
        CmdAnalyzeTools() = default;
        CmdAnalyzeTools( const int CmdV, const char ** CmdS )
        {
            init( CmdV, CmdS );
        }
        void init( const int CmdV, const char ** CmdS )
        {
            for ( int i = 0; i != CmdV; ++i)
            {
                Cmds.emplace_back( CmdS[i] );
            }
        }
        /// 获取指定下标项目
        template < typename T >
        T at( size_t i )
        {
            T temp;
            stringstream ss;
            ss << Cmds.at(i);
            ss >> temp;
            return temp;
        }
        /// 获取指定下标项目的string特化版本
        string at( size_t i )
        {
            return at<string>(i);
        }
        /// 获取指定下标项目的直接引用传参版本
        template < typename T >
        void get( size_t i, T &temp)
        {
            stringstream ss;
            ss << Cmds.at(i);
            ss >> temp;
        }
        /// 获取大小
        size_t size()
        {
            return Cmds.size();
        }
        bool empty()
        {
            return Cmds.empty();
        }
        /// 遍历
        function< void(string) > traversal( function< void(const string&) > func ) const
        {
            for ( const string &a: Cmds )
            {
                func(a);
            }
            return func;
        }
#ifndef _MSC_VER		// vs2015不支持此代码
		/// 带下标遍历
		function< void(size_t, string) > traversal(function< void(size_t, const string&) > func) const
		{
			for (size_t i = 0; i != Cmds.size(); ++i)
			{
				func(i, Cmds.at(i));
			}
			return func;
		}
#endif
        /// 获取指定前缀项目的剩余内容
        vector<string> GetByPerfix( const string &perfix ) const
        {
            vector<string> temp;
            traversal( [&temp, perfix]( const string &s )
                    {
                        if ( perfix.size() > s.size() )
                        {
                            return;
                        }
                        size_t i = 0;
                        for ( ; i != perfix.size(); ++i)
                        {
                            if ( perfix.at(i)!=s.at(i) )
                            {
                                return;
                            }
                        }
                        string temps;
                        for ( ; i != s.size(); ++i)
                        {
                            temps.push_back( s.at(i) );
                        }
                        temp.push_back( temps );
                    } );
            return temp;
        }
        /// 便捷化
        template <typename T>
        bool GetByPerfixOne( T &out, const string &perfix, const T &DefaultVat) const
        {
            return TranslateString2Type( out, GetByPerfix(perfix), DefaultVat);
        }
        /// 便捷化 字符串特例化
        bool GetByPerfixOne( string &out, const string &perfix, const char *DefaultVat) const
        {
            return TranslateString2Type( out, GetByPerfix(perfix), DefaultVat);
        }
        /// 便捷化 为空不修改
        template <typename T>
        bool GetByPerfixOne( T &out, const string &perfix) const
        {
            return TranslateString2Type( out, GetByPerfix(perfix));
        }
        /// 查找分隔符 并以分隔符拆分获取
        multimap<string,string> GetBySeparator( const string &s ) const
        {
            multimap<string,string> temp;
            if (s.empty())
            {
                return temp;
            }
            for_each( Cmds.begin(), Cmds.end(), [&]( const string &cs )
                    {
                        size_t i = 0;
                        for ( ; i != cs.size(); ++i)
                        {
                            if ( i + s.size() <= cs.size() )
                            {
                                size_t j = 0;
                                for ( ; j != s.size(); ++j)
                                {
                                    if ( cs.at(i) != s.at(j) )
                                    {
                                        break;
                                    }
                                }
                                if ( j != s.size() )
                                {
                                    continue;
                                }
                                {
                                    string befor;
                                    for ( size_t j = 0; j != i; ++j)
                                    {
                                        befor.push_back( cs.at(j) );
                                    }
                                    string after;
                                    for ( size_t j = i+s.size(); j != cs.size(); ++j)
                                    {
                                        after.push_back( cs.at(j) );
                                    }
                                    temp.emplace( befor, after );
                                    return;
                                }
                            } else {
                                return;
                            }
                        }
                    });
            return temp;
        }
        virtual ~CmdAnalyzeTools() = default;


        /// 下面是各种小工具
    public:
        /// 利用stringstream转换字符串到类型   带缺省值的字符串转换函数
        template <typename T>
        T TranslateString2Type( const string &s, const T &DefaultVar = T() ) const
        {
            if ( s.empty() )
            {
                return DefaultVar;
            }
            T temp;
            stringstream ss;
            ss << s;
            ss >> temp;
            return temp;
        }
        /// 包覆
        template <typename T>
        T TranslateString2Type(  const vector<string> &vs, const T &DefaultVar = T() ) const
        {
            return TranslateString2Type( GetFirstEElement(vs), DefaultVar);
        }
        /// 特化版本
        bool TranslateString2Type( string &out, const string &s, const char * const DefaultVar ) const
        {
            return TranslateString2Type( out, s, string(DefaultVar));
        }
        /// 包覆
        bool TranslateString2Type( string &out, const vector<string> &vs, const char * const DefaultVar ) const
        {
            return TranslateString2Type( out, GetFirstEElement(vs), DefaultVar);
        }
        /// 引用形参模式的带默认值的字符串转换函数
        template <typename T>
        bool TranslateString2Type( T &out, const string &s, const T &DefaultVar ) const
        {
            if ( s.empty() )
            {
                out = DefaultVar;
                return false;
            }
            stringstream ss;
            ss << s;
            ss >> out;
            return true;
        }
        /// 包覆
        template <typename T>
        bool TranslateString2Type( T &out, const vector<string> &vs, const T &DefaultVar ) const
        {
            return TranslateString2Type( out, GetFirstEElement(vs), DefaultVar);
        }
        /// 引用形参模式的不带默认值的字符串转换函数
        template <typename T>
        bool TranslateString2Type( T &out, const string &s) const
        {
            if ( s.empty() )
            {
                return false;
            }
            stringstream ss;
            ss << s;
            ss >> out;
            return true;
        }
        /// 包覆
        template <typename T>
        bool TranslateString2Type( T &out, const vector<string> &vs) const
        {
            return TranslateString2Type( out, GetFirstEElement(vs));
        }
        /// 异常无关地获取vector首个元素的小工具 获取不到时使用默认值
        inline const string &GetFirstEElement( const vector<string> &vs, const string &DefaultVar = string()) const
        {
            if ( vs.empty() )
            {
                return DefaultVar;
            }
            return vs.at(0);
        }
    protected:
    private:
        vector<string> Cmds;
};

#endif // CMDANALYZETOOLS_H
