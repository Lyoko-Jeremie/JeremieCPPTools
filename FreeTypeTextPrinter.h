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
	// 构造函数  传入指定的字体文件以初始化
	// 字体文件读取或初始化失败  FreeType库初始化失败时  均抛出run_time异常
	FreeTypeTextPrinter(std::string font_file);
	virtual ~FreeTypeTextPrinter();
	// 设置字体渲染size  应该在获取第一个字体前设置
	// 设置失败时返回false  此时此字体库不支持此size
	bool Set_Char_Size(FT_F26Dot6 char_width = 16 * 64, FT_F26Dot6 char_height = 0, FT_UInt horz_resolution = 300, FT_UInt vert_resolution = 0);
	// 快速设置字体渲染size
	// 设置失败时返回false  此时此字体库不支持此size
	bool Set_Char_Size_Fast(FT_F26Dot6 size = 16 * 64, FT_UInt resolution = 0);
	// 设置字距  1.0为字体默认距离
	void Set_Character_Spacing(double character_spacing = 1.1);

	// 从字库中选取字体
	bool Select_Char(FT_ULong  char_code);
	// 获取上一次选取的字体的Glyph
	FT_GlyphSlot const& Glyph() const;

	// 设置下一个函数的字体色彩
	void SetColor(const cv::Scalar& color);
	// 返回 彩色字体 字体掩码 基线 左距
	std::tuple<cv::Mat, cv::Mat, signed int, signed int> GetCharMat() const;

	// 参数  目标图像  文字  初始坐标点（文字基线点）
	// 超出边缘的文字不会显示
	// 不自动换行  不处理制表符  只在字库中有A的情况下将空格设置为A的宽度
	// 遇到字体库中不存在的无效字符返回false
	// 注意  msvc的wstring wchar_t是16bit 所以只能正确处理标准平面内的UTF16 而需要使用UTF16代理对的扩展平面则无法正常工作
	bool PutText(cv::Mat& img, const std::wstring& string, cv::Point begin_point);
	// 参数  目标图像  文字  初始坐标点（文字基线点）  结束时的下一个字符的初始坐标点
	// 退出函数时会返回到下一个字符的起始点的引用
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
