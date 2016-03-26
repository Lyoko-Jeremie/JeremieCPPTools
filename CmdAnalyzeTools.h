#ifndef CMDANALYZETOOLS_H
#define CMDANALYZETOOLS_H

#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <algorithm>
#include <map>
//using namespace std;

#define CAT_MainFunctionComand const int CAT_CmdV, const char ** CAT_CmdS
#define CAT_CmdAnalyzeToolsInitFunctionComand CAT_CmdV, CAT_CmdS

class CmdAnalyzeTools
{
public:
	CmdAnalyzeTools() = default;
	CmdAnalyzeTools(const int CmdV, const char ** CmdS)
	{
		init(CmdV, CmdS);
	}
	void init(const int CmdV, const char ** CmdS)
	{
		for (auto i = 0; i != CmdV; ++i)
		{
			Cmds.emplace_back(CmdS[i]);
		}
	}
	/// ��ȡָ���±���Ŀ
	template < typename T >
	T at(size_t i)
	{
		T temp;
		std::stringstream ss;
		ss << Cmds.at(i);
		ss >> temp;
		return temp;
	}
	/// ��ȡָ���±���Ŀ��string�ػ��汾
	std::string at(size_t i)
	{	// stringstream ��ת����string����ʱ���ڿհ��ַ����ض�
		return Cmds.at(i);
	}
	/// ��ȡָ���±���Ŀ��ֱ�����ô��ΰ汾
	template < typename T >
	void get(size_t i, T &temp)
	{
		std::stringstream ss;
		ss << Cmds.at(i);
		ss >> temp;
	}
	/// ��ȡ��С
	size_t size() const
	{
		return Cmds.size();
	}
	bool empty() const
	{
		return Cmds.empty();
	}
	/// ����
	std::function< void(std::string) > traversal(std::function< void(const std::string&) > func) const
	{
		for (const std::string &a : Cmds)
		{
			func(a);
		}
		return func;
	}
#ifndef _MSC_VER		// vs2015��֧�ִ˴���
	/// ���±����
	std::function< void(size_t, std::string) > traversal(std::function< void(size_t, const std::string&) > func) const
	{
		for (size_t i = 0; i != Cmds.size(); ++i)
		{
			func(i, Cmds.at(i));
		}
		return func;
	}
#endif
	/// ��ȡָ��ǰ׺��Ŀ��ʣ������
	std::vector<std::string> GetByPerfix(const std::string &perfix) const
	{
		std::vector<std::string> temp;
		traversal([&temp, perfix](const std::string &s)
		{
			if (perfix.size() > s.size())
			{
				return;
			}
			size_t i = 0;
			for (; i != perfix.size(); ++i)
			{
				if (perfix.at(i) != s.at(i))
				{
					return;
				}
			}
			std::string temps;
			for (; i != s.size(); ++i)
			{
				temps.push_back(s.at(i));
			}
			temp.push_back(temps);
		});
		return temp;
	}
	/// ��ݻ�
	template <typename T>
	bool GetByPerfixOne(T &out, const std::string &perfix, const T &DefaultVat) const
	{
		return TranslateString2Type(out, GetByPerfix(perfix), DefaultVat);
	}
	/// ��ݻ� �ַ���������
	bool GetByPerfixOne(std::string &out, const std::string &perfix, const char *DefaultVat) const
	{
		return TranslateString2Type(out, GetByPerfix(perfix), DefaultVat);
	}
	/// ��ݻ� Ϊ�ղ��޸�
	template <typename T>
	bool GetByPerfixOne(T &out, const std::string &perfix) const
	{
		return TranslateString2Type(out, GetByPerfix(perfix));
	}
	/// ���ҷָ��� ���Էָ�����ֻ�ȡ
	std::multimap<std::string, std::string> GetBySeparator(const std::string &s) const
	{
		std::multimap<std::string, std::string> temp;
		if (s.empty())
		{
			return temp;
		}
		std::for_each(Cmds.begin(), Cmds.end(), [&](const std::string &cs)
		{
			size_t i = 0;
			for (; i != cs.size(); ++i)
			{
				if (i + s.size() <= cs.size())
				{
					size_t j = 0;
					for (; j != s.size(); ++j)
					{
						if (cs.at(i) != s.at(j))
						{
							break;
						}
					}
					if (j != s.size())
					{
						continue;
					}
					{
						std::string befor;
						for (size_t k = 0; k != i; ++k)
						{
							befor.push_back(cs.at(k));
						}
						std::string after;
						for (auto l = i + s.size(); l != cs.size(); ++l)
						{
							after.push_back(cs.at(l));
						}
						temp.emplace(befor, after);
						return;
					}
				}
				else {
					return;
				}
			}
		});
		return temp;
	}
	virtual ~CmdAnalyzeTools() = default;


	/// �����Ǹ���С����
public:
	/// ����std::stringstreamת���ַ���������   ��ȱʡֵ���ַ���ת������
	template <typename T>
	T TranslateString2Type(const std::string &s, const T &DefaultVar = T()) const
	{
		if (s.empty())
		{
			return DefaultVar;
		}
		T temp;
		std::stringstream ss;
		ss << s;
		ss >> temp;
		return temp;
	}
	/// ����
	template <typename T>
	T TranslateString2Type(const std::vector<std::string> &vs, const T &DefaultVar = T()) const
	{
		return TranslateString2Type(GetFirstEElement(vs), DefaultVar);
	}
	/// �ػ��汾
	bool TranslateString2Type(std::string &out, const std::string &s, const char * const DefaultVar) const
	{
		return TranslateString2Type(out, s, std::string(DefaultVar));
	}
	/// ����
	bool TranslateString2Type(std::string &out, const std::vector<std::string> &vs, const char * const DefaultVar) const
	{
		return TranslateString2Type(out, GetFirstEElement(vs), DefaultVar);
	}
	/// �����β�ģʽ�Ĵ�Ĭ��ֵ���ַ���ת������
	template <typename T>
	bool TranslateString2Type(T &out, const std::string &s, const T &DefaultVar) const
	{
		if (s.empty())
		{
			out = DefaultVar;
			return false;
		}
		std::stringstream ss;
		ss << s;
		ss >> out;
		return true;
	}
	/// ����
	template <typename T>
	bool TranslateString2Type(T &out, const std::vector<std::string> &vs, const T &DefaultVar) const
	{
		return TranslateString2Type(out, GetFirstEElement(vs), DefaultVar);
	}
	/// �����β�ģʽ�Ĳ���Ĭ��ֵ���ַ���ת������
	template <typename T>
	bool TranslateString2Type(T &out, const std::string &s) const
	{
		if (s.empty())
		{
			return false;
		}
		std::stringstream ss;
		ss << s;
		ss >> out;
		return true;
	}
	/// ����
	template <typename T>
	bool TranslateString2Type(T &out, const std::vector<std::string> &vs) const
	{
		return TranslateString2Type(out, GetFirstEElement(vs));
	}
	/// �쳣�޹صػ�ȡvector�׸�Ԫ�ص�С���� ��ȡ����ʱʹ��Ĭ��ֵ
	static inline const std::string &GetFirstEElement(const std::vector<std::string> &vs, const std::string &DefaultVar = std::string())
	{
		if (vs.empty())
		{
			return DefaultVar;
		}
		return vs.at(0);
	}
protected:
private:
	std::vector<std::string> Cmds;
};

#endif // CMDANALYZETOOLS_H
