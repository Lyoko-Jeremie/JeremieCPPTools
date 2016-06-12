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


/*

// 基本使用方法示例

bool test()
{
	// 需要放置文字的图片  BGR格式
	Mat img = Mat(768, 1024, CV_8UC3, Scalar(255, 255, 255));

	// FreeType初始化 字体表面初始化
	// 初始化失败时抛出异常 程序退出
	// 注意	因为FreeType库实现方式的原因（内部自带状态），同一个FreeTypeTextPrinter对象不可在多线程环境下使用
	FreeTypeTextPrinter fttp(R"(SourceHanSansSC-Medium.otf)");

	// 文字大小设定  应该在获取第一个字体前设置
	// 必须要设置并检查  某些字体可能不支持某些分辨率
	// 这里如使用Set_Char_Size()可以有更多选项
	if (!fttp.Set_Char_Size_Fast(64 * 64))
	{
		return false;
	}

	// 字距设定  1.0为字体默认字距0字距  默认值1.1
	fttp.Set_Character_Spacing(1.25);

	// 文字颜色设定  默认为{0,0,0}
	fttp.SetColor(Scalar{ 255,0,255 });

	// 在图片上放置文字
	// 参数为  Mat图片  wchar_t宽字节字符串   字体基线（左下角）绘制起点
	// 绘制成功返回true  绘制到无法解析的字符时返回false
	// 注意 即使FreeType库要求输入UTF-32字符，但在windows下因为wchar_t只有16bit，所以基本平面外的字符无法正确显示
	// TODO 处理UTF-16代理对
	fttp.PutText(img, L"(´ﾟДﾟ`)(＾o＾)ﾉ(｡◕∀◕｡)", { 50,100 });
	fttp.PutText(img, L"中华人民共和国", { 50,200 });

	// 显示图像或做其他处理......
	namedWindow("", WINDOW_NORMAL | WINDOW_KEEPRATIO | CV_GUI_NORMAL);
	imshow("", img);
	waitKey();

return true;
}


*/






#endif // H_FREE_TYPE_TEXT_PRINTER_H
