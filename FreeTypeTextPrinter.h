#ifndef H_FREE_TYPE_TEXT_PRINTER_H
#define H_FREE_TYPE_TEXT_PRINTER_H


#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <string>
#include <cstring>
#include <utility>
#include <tuple>

#include <opencv2/opencv.hpp>


class FreeTypeTextPrinter
{
public:
	// ���캯��  ����ָ���������ļ��Գ�ʼ��
	// �����ļ���ȡ���ʼ��ʧ��  FreeType���ʼ��ʧ��ʱ  ���׳�run_time�쳣
	FreeTypeTextPrinter(std::string font_file);
	virtual ~FreeTypeTextPrinter();
	// ����������Ⱦsize  Ӧ���ڻ�ȡ��һ������ǰ����
	// ����ʧ��ʱ����false  ��ʱ������ⲻ֧�ִ�size
	bool Set_Char_Size(FT_F26Dot6 char_width = 16 * 64, FT_F26Dot6 char_height = 0, FT_UInt horz_resolution = 300, FT_UInt vert_resolution = 0);
	// ��������������Ⱦsize
	// ����ʧ��ʱ����false  ��ʱ������ⲻ֧�ִ�size
	bool Set_Char_Size_Fast(FT_F26Dot6 size = 16 * 64, FT_UInt resolution = 0);
	// �����־�  1.0Ϊ����Ĭ�Ͼ���
	void Set_Character_Spacing(double character_spacing = 1.1);

	// ���ֿ���ѡȡ����
	bool Select_Char(FT_ULong  char_code);
	// ��ȡ��һ��ѡȡ�������Glyph
	FT_GlyphSlot const& Glyph() const;

	// ������һ������������ɫ��
	void SetColor(const cv::Scalar& color);
	// ���� ��ɫ���� �������� ���� ���
	std::tuple<cv::Mat, cv::Mat, signed int, signed int> GetCharMat() const;

	// ����  Ŀ��ͼ��  ����  ��ʼ����㣨���ֻ��ߵ㣩
	// ������Ե�����ֲ�����ʾ
	// ���Զ�����  �������Ʊ��  ֻ���ֿ�����A������½��ո�����ΪA�Ŀ��
	// ����������в����ڵ���Ч�ַ�����false
	// ע��  msvc��wstring wchar_t��16bit ����ֻ����ȷ�����׼ƽ���ڵ�UTF16 ����Ҫʹ��UTF16����Ե���չƽ�����޷���������
	bool PutText(cv::Mat& img, const std::wstring& string, cv::Point begin_point);
	// ����  Ŀ��ͼ��  ����  ��ʼ����㣨���ֻ��ߵ㣩  ����ʱ����һ���ַ��ĳ�ʼ�����
	// �˳�����ʱ�᷵�ص���һ���ַ�����ʼ�������
	bool PutText(cv::Mat& img, const std::wstring& string, cv::Point begin_point, cv::Point& end_point);

	// wstring => string
	std::string WString2String(const std::wstring& ws) const;

	// string => wstring
	std::wstring String2WString(const std::string& s) const;

private:
	FT_Library ft_library_{ nullptr };
	FT_Face ft_face_{ nullptr };
	cv::Mat color_lut_{ cv::Size(256,1), CV_8UC3, cv::Scalar{ 0,0,0 } };
	unsigned int empty_width_{ 1 };
	double character_spacing_{ 0.1 };
	int character_spacing_empty_width_{ 0 };
};

#endif // H_FREE_TYPE_TEXT_PRINTER_H
