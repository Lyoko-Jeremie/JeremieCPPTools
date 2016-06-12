#include "FreeTypeTextPrinter.h"


FreeTypeTextPrinter::FreeTypeTextPrinter(std::string font_file)
{
	if (FT_Err_Ok != FT_Init_FreeType(&ft_library_))
	{
		throw std::runtime_error("FT_Err_Ok != FT_Init_FreeType");
	}
	if (FT_Err_Ok != FT_New_Face(ft_library_, font_file.c_str(), 0, &ft_face_))
	{
		throw std::runtime_error("FT_Err_Ok != FT_New_Face");
	}
}

FreeTypeTextPrinter::~FreeTypeTextPrinter()
{
	FT_Done_Face(ft_face_);
	FT_Done_FreeType(ft_library_);
}

bool FreeTypeTextPrinter::Set_Char_Size(FT_F26Dot6 char_width, FT_F26Dot6 char_height, FT_UInt horz_resolution, FT_UInt vert_resolution)
{
	if (FT_Err_Ok == FT_Set_Char_Size(ft_face_, char_width, char_height, horz_resolution, vert_resolution))
	{
		// 获取A的宽度作为空格宽度
		if (this->Select_Char(L'A'))
		{
			empty_width_ = ft_face_->glyph->bitmap.width;
			character_spacing_empty_width_ = static_cast<int>(character_spacing_ * empty_width_);
		}
		return true;
	}
	return false;
}

bool FreeTypeTextPrinter::Set_Char_Size_Fast(FT_F26Dot6 size, FT_UInt resolution)
{
	return this->Set_Char_Size(size, 0, resolution, 0);
}

void FreeTypeTextPrinter::Set_Character_Spacing(double character_spacing)
{
	character_spacing_ = character_spacing - 1.0;
	character_spacing_empty_width_ = static_cast<int>(character_spacing_ * empty_width_);
}

bool FreeTypeTextPrinter::Select_Char(FT_ULong char_code)
{
	return FT_Err_Ok == FT_Load_Char(ft_face_, char_code, FT_LOAD_RENDER);
}

FT_GlyphSlot const& FreeTypeTextPrinter::Glyph() const
{
	return ft_face_->glyph;
}

void FreeTypeTextPrinter::SetColor(const cv::Scalar& color)
{
	// 重新初始化色彩查找表
	std::vector<cv::Mat> luts;
	luts.emplace_back(cv::Size(256, 1), CV_8U, cv::Scalar(color[0]));
	luts.emplace_back(cv::Size(256, 1), CV_8U, cv::Scalar(color[1]));
	luts.emplace_back(cv::Size(256, 1), CV_8U, cv::Scalar(color[2]));
	luts[0].at<uchar>(0, 0) = 0;
	luts[1].at<uchar>(0, 0) = 0;
	luts[2].at<uchar>(0, 0) = 0;
	merge(luts, color_lut_);
}

std::tuple<cv::Mat, cv::Mat, signed int, signed int> FreeTypeTextPrinter::GetCharMat() const
{
	// bitmap_left为左距  bitmap_top为图形顶部到基线的距离基线
	auto &bitmap = ft_face_->glyph->bitmap;

	// 就地构造 免除手动内存拷贝  注意，生成下一个字符时内存会被清除
	cv::Mat char_bitmap(cv::Size(bitmap.width, bitmap.rows), CV_8U, bitmap.buffer);
	cv::Mat char_img;
	// 处理非打印字符问题
	if (0 != char_bitmap.rows || 0 != char_bitmap.cols)
	{
		char_img = char_bitmap.clone();
	}
	else
	{
		char_img = cv::Mat(cv::Size(std::max(bitmap.width, empty_width_), std::max(bitmap.rows, empty_width_)), CV_8U, cv::Scalar(0));
	}

	cvtColor(char_img, char_img, CV_GRAY2BGR);

	cv::Mat mask;	// 填涂掩码  用来挖除目标图像的掩码区域以便去除字体色彩不饱和导致字体模糊的副作用
	bitwise_not(char_img, mask);

	LUT(char_img, color_lut_, char_img);

	return std::make_tuple(char_img, mask, ft_face_->glyph->bitmap_top, ft_face_->glyph->bitmap_left);
}

bool FreeTypeTextPrinter::PutText(cv::Mat& img, const std::wstring& string, cv::Point begin_point)
{
	return PutText(img, string, begin_point, begin_point);
}

bool FreeTypeTextPrinter::PutText(cv::Mat& img, const std::wstring& string, cv::Point begin_point, cv::Point& end_point)
{
	for (size_t i = 0; i != string.size(); ++i)
	{
		if (!this->Select_Char(string.c_str()[i]))
		{
			end_point = begin_point;
			return false;
		}

		std::pair<cv::Mat, cv::Mat> char_p;
		signed int left, top;

		std::tie(char_p.first, char_p.second, top, left) = this->GetCharMat();

		auto rect = cv::Rect(
			begin_point.x,
			begin_point.y - top,
			char_p.first.size().width,
			char_p.first.size().height
		);

		if (
			0 < rect.x &&
			0 < rect.y &&
			rect.br().x < img.cols &&
			rect.br().y < img.rows
			)
		{
			auto roi = img(rect);
			bitwise_and(roi, char_p.second, roi);
			bitwise_or(roi, char_p.first, roi);
		}
		else
		{
			// 裁剪算法

			auto p1 = rect.tl();
			auto p2 = rect.br();
			cv::Point p0{ 0,0 };

			int width = rect.width;
			int height = rect.height;

			// 角点区内移动
			if (p2.x > img.cols)
			{
				width -= p2.x - img.cols;
				p2.x = img.cols;
			}
			if (p2.y > img.rows)
			{
				height -= p2.y - img.rows;
				p2.y = img.rows;
			}
			if (p1.x < 0)
			{
				width += p1.x;
				p0.x -= p1.x;
				p1.x = 0;
			}
			if (p1.y < 0)
			{
				height += p1.y;
				p0.y -= p1.y;
				p1.y = 0;
			}

			// 实际出区检查
			if (width > 0 && height > 0)
			{

				// 新的放置区
				rect = cv::Rect{ p1, p2 };
				auto rect_i = cv::Rect{ p0,rect.size() };

				// 拷贝

				auto roi = img(rect);
				bitwise_and(roi, char_p.second(rect_i), roi);
				bitwise_or(roi, char_p.first(rect_i), roi);
			}

		}
		// 不进行自动换行

		// 下一个字  此处字符间距从A中计算得来
		begin_point.x += char_p.first.size().width + character_spacing_empty_width_;
	}
	end_point = begin_point;
	return true;
}

std::string FreeTypeTextPrinter::WString2String(const std::wstring& ws) const
{
	std::string strLocale = setlocale(LC_ALL, "");
	const wchar_t* wchSrc = ws.c_str();
	size_t nDestSize{ 0 };
	wcstombs_s(&nDestSize, nullptr, 0, wchSrc, 0);
	char* chDest = new char[nDestSize];
	memset(chDest, 0, nDestSize);
	wcstombs_s(&nDestSize, chDest, nDestSize, wchSrc, nDestSize);
	std::string strResult = chDest;
	delete[]chDest;
	setlocale(LC_ALL, strLocale.c_str());
	return strResult;
}

std::wstring FreeTypeTextPrinter::String2WString(const std::string& s) const
{
	std::string strLocale = setlocale(LC_ALL, "");
	const char* chSrc = s.c_str();
	size_t nDestSize{ 0 };
	mbstowcs_s(&nDestSize, nullptr, 0, chSrc, 0);
	wchar_t* wchDest = new wchar_t[nDestSize];
	wmemset(wchDest, 0, nDestSize);
	mbstowcs_s(&nDestSize, wchDest, nDestSize, chSrc, nDestSize);
	std::wstring wstrResult = wchDest;
	delete[]wchDest;
	setlocale(LC_ALL, strLocale.c_str());
	return wstrResult;
}


