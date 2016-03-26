#ifndef CHARARRAY_H
#define CHARARRAY_H

#include <cstddef>
#include <string>
#include <iostream>

/**
 *|һ��������char�����װ��
 *|Ϊwinsocketpack��װ
 *|������send&recv
 *|
 *|Note��dlength Ϊ��ʵ���ݳ���
 *|Warning�� һ��Ҫ�� GetPtr ��д�����ݺ� GetString ǰ���� dlength Ϊ�������ʵ����ֵ
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

        /// Note ��ʵ���ݳ���      Warning һ��Ҫ�� GetPtr ��д�����ݺ� GetString ǰ����Ϊ��ʵ����ֵ
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
