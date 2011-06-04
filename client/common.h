//////////////////////////////////////////////////////////////////////////////
// common.h
// -------------------
// Crystal window decoration for KDE
// -------------------
// Copyright (c) 2006-2011 Sascha Hlusiak <spam84@gmx.de>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////


#ifndef COMMON_H_INCLUDED_
#define COMMON_H_INCLUDED_


#define TOP_LEFT 1
#define TOP_RIGHT 2
#define BOT_LEFT 4
#define BOT_RIGHT 8

struct WND_CONFIG
{
	int outlineMode,inlineMode;
	QColor frameColor,inlineColor;
	QPixmap overlay;
	int transparency;
	bool stretch_overlay;
}; 


enum ButtonImageTypes {
	ButtonImageMenu=0,
	ButtonImageHelp,
	ButtonImageMax,
	ButtonImageRestore,
	ButtonImageMin,
	ButtonImageClose, 
	ButtonImageSticky,
	ButtonImageUnSticky,
	ButtonImageShade,
	ButtonImageUnShade,
	ButtonImageBelow,
	ButtonImageUnBelow,
	ButtonImageAbove,
	ButtonImageUnAbove,
	ButtonImageCount
};


#endif