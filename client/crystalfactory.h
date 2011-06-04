//////////////////////////////////////////////////////////////////////////////
// crystalfactory.h
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


#ifndef CRYSTALFACTORY_H
#define CRYSTALFACTORY_H

#include <kdecoration.h>
#include <kdecorationfactory.h>

#include "common.h"

class ButtonImage;
class CrystalClient;
 
 

class CrystalFactory: public KDecorationFactory
{
public:
	CrystalFactory();
	virtual ~CrystalFactory();
	virtual KDecoration *createDecoration(KDecorationBridge *b);
	virtual bool reset(unsigned long changed);
	virtual bool supports(Ability ability) const;

	static bool initialized() { return initialized_; }
	static Qt::Alignment titleAlign() { return titlealign_; }
public:
	QPixmap logo;
	int logoEnabled,logoStretch,logoActive,logoDistance;

	int titlesize;
	bool hovereffect,tintButtons,animateHover,menuImage,wheelTask;

	QColor buttonColor_normal,buttonColor_hovered,buttonColor_pressed;
	QColor minColor_normal,minColor_hovered,minColor_pressed;
	QColor maxColor_normal,maxColor_hovered,maxColor_pressed;
	QColor closeColor_normal,closeColor_hovered,closeColor_pressed;

	int borderwidth;
	bool drawcaption,textshadow,captiontooltip;
	int roundCorners;
	WND_CONFIG active,inactive;
	int buttontheme;
	
	ButtonImage *buttonImages[ButtonImageCount];
	QList <CrystalClient*> clients;
private:
	bool readConfig();
	void CreateButtonImages();
private:
	static bool initialized_;
	static Qt::AlignmentFlag titlealign_;
};


extern CrystalFactory *factory;


#endif