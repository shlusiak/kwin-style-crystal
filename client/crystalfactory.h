/***************************************************************************
 *   crystalfactory.h                                                      *
 *   -----------------------                                               *
 *   Copyright (C) 2006-2011 by Sascha Hlusiak                             *
 *   Spam84@gmx.de                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


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