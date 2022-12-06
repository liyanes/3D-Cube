#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include "glext.h"

namespace glExt::font {
	class face;
	class library {
		FT_Library _lib;
	public:
		inline library();
		inline ~library();

		friend class face;
	};

	class face {
		FT_Face _face;
	public:
		inline face(const library&,const char* path);
		inline void setPixelSize(unsigned x, unsigned y);
		inline const FT_Face getFace() const noexcept;
		inline void loadChar(unsigned long _char,int flag = FT_LOAD_RENDER);

	};
}

inline glExt::font::library::library() {
	if (FT_Init_FreeType(&_lib)) throw glExt::err::glFontLoadFailed();
}

inline glExt::font::library::~library()
{
	FT_Done_FreeType(_lib);
}

inline glExt::font::face::face(const library& _lib, const char* path)
{
	if (FT_New_Face(_lib._lib, path, 0, &this->_face)) throw glExt::err::glFontLoadFailed();
}

inline void glExt::font::face::setPixelSize(unsigned x, unsigned y)
{
	FT_Set_Pixel_Sizes(_face, x, y);
}

inline const FT_Face glExt::font::face::getFace() const noexcept
{
	return this->_face;
}

inline void glExt::font::face::loadChar(unsigned long _char,int flag)
{
	if (FT_Load_Char(_face, _char, flag)) throw glExt::err::glFontLoadFailed();
}


