#ifndef CHARARRAY_H
#define CHARARRAY_H

#include <cstddef>
#include <string>
#include <iostream>

/**
 *|一个脆弱的char数组包装类
 *|为winsocketpack包装
 *|适用于send&recv
 *|
 *|Note：dlength 为真实数据长度
 *|Warning： 一定要在 GetPtr 并写入内容后 GetString 前设置 dlength 为保存的真实数据值
 *|
 */

class CharArray
{
    public:
        CharArray() = delete;
        CharArray(size_t length);
        virtual ~CharArray();
        CharArray(const CharArray& other);
        CharArray& operator=(const CharArray& other);
        CharArray(CharArray&& other);
        CharArray& operator=( CharArray&& other);
        CharArray& operator=( std::string& other);
        void Clear( char f = '\0');
        size_t GetSize() const;
        char *GetPtr();
        std::string GetString() const;

        /// Note 真实数据长度      Warning 一定要在 GetPtr 并写入内容后 GetString 前设置为真实数据值
        size_t dlength;

    protected:
    private:
        char *pcdata;
        size_t length;
};

inline
std::ostream & operator<< ( std::ostream &os, CharArray const &ca)
{
    os << ca.GetString();
    return os;
}

#endif // CHARARRAY_H
