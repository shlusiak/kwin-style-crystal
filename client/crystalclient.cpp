/***************************************************************************
 *   Copyright (C) 2006-2008 by Sascha Hlusiak                                  *
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

#include <kconfig.h>
#include <kconfiggroup.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kdebug.h>

#include <qbitmap.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qtooltip.h>
#include <qapplication.h>
#include <qimage.h>
#include <kwindowsystem.h>
#include <kwindowinfo.h>
#include <kprocess.h>

#include "crystalclient.h"
#include "crystalbutton.h"
#include "buttonimage.h"
#include "overlays.h"

// Button themes
#include "tiles.h"


CrystalFactory* factory=NULL;

bool CrystalFactory::initialized_              = false;
Qt::AlignmentFlag CrystalFactory::titlealign_ = Qt::AlignHCenter;


extern "C" KDE_EXPORT KDecorationFactory* create_factory()
{
	return new CrystalFactory();
}



CrystalFactory::CrystalFactory()
{
	for (int i=0;i<ButtonImageCount;i++)
		buttonImages[i]=NULL;

	::factory=this;
	readConfig();
	initialized_ = true;

	CreateButtonImages();
}

CrystalFactory::~CrystalFactory() 
{
	initialized_ = false; 
	::factory=NULL;
	for (int i=0;i<ButtonImageCount;i++)
	{
		if (buttonImages[i])delete buttonImages[i];
		buttonImages[i]=NULL;
	}
}

KDecoration* CrystalFactory::createDecoration(KDecorationBridge* b)
{
	return new CrystalClient(b,factory );
}

bool CrystalFactory::reset(unsigned long /*changed*/)
{
	initialized_ = false;
	readConfig();
	initialized_ = true;
	CreateButtonImages();
	
	return true;
}

bool CrystalFactory::supports(Ability ability) const
{
	switch (ability)
	{
	case AbilityButtonResize: return false;
	default:
		return true;
	}
}

void setupOverlay(WND_CONFIG *cfg,int mode,QString filename)
{
	switch(mode)
	{
		case 0:	break;
		case 1:{
			QImage img=QImage((uchar*)lighting_overlay_data,1,60,QImage::Format_ARGB32);
			cfg->overlay = QPixmap::fromImage(img.scaled(256,::factory->titlesize));
			break;
		}
		case 2:{
			QImage img=QImage((uchar*)glass_overlay_data,20,64,QImage::Format_ARGB32);
			cfg->overlay = QPixmap::fromImage(img.scaled(256,::factory->titlesize));
			break;
		}
		case 3:{
			QImage img=QImage((uchar*)steel_overlay_data,28,64,QImage::Format_ARGB32);
			cfg->overlay = QPixmap::fromImage(img.scaled(256,::factory->titlesize));
			break;
		}
		case 4:{
			QImage img;
			if (img.load(filename))
			{
				cfg->overlay = QPixmap::fromImage(img.scaled(256,::factory->titlesize));
			}
			break;
		}
	}
}

bool CrystalFactory::readConfig()
{
    // create a config object
	KConfig config("kwincrystalrc");
	KConfigGroup cg(&config, "General");
	QColor c;

	int value = cg.readEntry("TitleAlignment", 0);
	if (value == 0) titlealign_ = Qt::AlignLeft;
	else if (value == 1) titlealign_ = Qt::AlignHCenter;
	else if (value == 2) titlealign_ = Qt::AlignRight;
	
	drawcaption=(bool)cg.readEntry("DrawCaption",true);
	textshadow=(bool)cg.readEntry("TextShadow",true);
	captiontooltip=(bool)cg.readEntry("CaptionTooltip",true);
	wheelTask=(bool)cg.readEntry("WheelTask",false);

	active.outlineMode=(int)cg.readEntry("ActiveFrame",1);
	inactive.outlineMode=(int)cg.readEntry("InactiveFrame",1);
	c=QColor(160,160,160);
	active.frameColor=cg.readEntry("FrameColor1",c);
	c=QColor(128,128,128);
	inactive.frameColor=cg.readEntry("FrameColor2",c);

	active.inlineMode=(int)cg.readEntry("ActiveInline",0);
	inactive.inlineMode=(int)cg.readEntry("InactiveInline",0);
	c=QColor(160,160,160);
	active.inlineColor=cg.readEntry("InlineColor1",c);
	c=QColor(160,160,160);
	inactive.inlineColor=cg.readEntry("InlineColor2",c);

	borderwidth=cg.readEntry("Borderwidth",5);
	titlesize=cg.readEntry("Titlebarheight",21);
 
	buttonColor_normal=QColor(255,255,255);
	buttonColor_normal=cg.readEntry("ButtonColor",buttonColor_normal);
	buttonColor_hovered=cg.readEntry("ButtonColor2",buttonColor_normal);
	buttonColor_pressed=cg.readEntry("ButtonColor3",buttonColor_normal);
	minColor_normal=QColor(255,255,255);
	minColor_normal=cg.readEntry("MinColor",buttonColor_normal);
	minColor_hovered=cg.readEntry("MinColor2",buttonColor_normal);
	minColor_pressed=cg.readEntry("MinColor3",buttonColor_normal);
	maxColor_normal=QColor(255,255,255);
	maxColor_normal=cg.readEntry("MaxColor",buttonColor_normal);
	maxColor_hovered=cg.readEntry("MaxColor2",buttonColor_normal);
	maxColor_pressed=cg.readEntry("MaxColor3",buttonColor_normal);
	closeColor_normal=QColor(255,255,255);
	closeColor_normal=cg.readEntry("CloseColor",closeColor_normal);
	closeColor_hovered=cg.readEntry("CloseColor2",closeColor_normal);
	closeColor_pressed=cg.readEntry("CloseColor3",closeColor_normal);

	roundCorners=cg.readEntry("RoundCorners",TOP_LEFT & TOP_RIGHT);

	hovereffect=cg.readEntry("HoverEffect",true);
	animateHover=cg.readEntry("AnimateHover",true);
	tintButtons=cg.readEntry("TintButtons",false);
	menuImage=cg.readEntry("MenuImage",true);
	buttontheme=cg.readEntry("ButtonTheme",8);

	setupOverlay(&active,cg.readEntry("OverlayModeActive",0),cg.readEntry("OverlayFileActive",""));
	setupOverlay(&inactive,cg.readEntry("OverlayModeInactive",0),cg.readEntry("OverlayFileInactive",""));

	logoEnabled=cg.readEntry("LogoAlignment",1);
	logoStretch=cg.readEntry("LogoStretch",0);
	logoActive=cg.readEntry("LogoActive",0);
	logoDistance=cg.readEntry("LogoDistance",0);
	QString filename=cg.readEntry("LogoFile","");
	if (!filename.isNull() && logoEnabled!=1)
	{
		if (logo.load(filename))
		{
			if (logoStretch==0)
			{
				logo=logo.scaled((titlesize*logo.width())/logo.height(),titlesize);
			}
		}else logoEnabled=1;
	}else logo = QPixmap();
	return true;
}

void CrystalFactory::CreateButtonImages()
{
	for (int i=0;i<ButtonImageCount;i++)
	{
		if (buttonImages[i])buttonImages[i]->reset(); else
		buttonImages[i]=new ButtonImage;
		if (!tintButtons)buttonImages[i]->setColors(Qt::white,Qt::white,Qt::white);
		else switch(i)
		{
			case ButtonImageMin:
				buttonImages[i]->setColors(minColor_normal,minColor_hovered,minColor_pressed);
				break;
			case ButtonImageMax:
				buttonImages[i]->setColors(maxColor_normal,maxColor_hovered,maxColor_pressed);
				break;
			case ButtonImageClose:
				buttonImages[i]->setColors(closeColor_normal,closeColor_hovered,closeColor_pressed);
				break;

			default:
				buttonImages[i]->setColors(buttonColor_normal,buttonColor_hovered,buttonColor_pressed);
				break;
		}
	}
	
	switch(buttontheme)
	{
	default:
	case 0:	// Crystal default
		buttonImages[ButtonImageMenu]->SetNormal(crystal_menu_data);
		buttonImages[ButtonImageHelp]->SetNormal(crystal_help_data);
		buttonImages[ButtonImageMax]->SetNormal(crystal_max_data);
		buttonImages[ButtonImageRestore]->SetNormal(crystal_restore_data);
		buttonImages[ButtonImageMin]->SetNormal(crystal_min_data);
		buttonImages[ButtonImageClose]->SetNormal(crystal_close_data);
		buttonImages[ButtonImageSticky]->SetNormal(crystal_sticky_data);
		buttonImages[ButtonImageUnSticky]->SetNormal(crystal_un_sticky_data);
		buttonImages[ButtonImageShade]->SetNormal(crystal_shade_data);
		buttonImages[ButtonImageUnShade]->SetNormal(crystal_shade_data);
	
		buttonImages[ButtonImageAbove]->SetNormal(crystal_above_data);
		buttonImages[ButtonImageUnAbove]->SetNormal(crystal_unabove_data);
		buttonImages[ButtonImageBelow]->SetNormal(crystal_below_data);
		buttonImages[ButtonImageUnBelow]->SetNormal(crystal_unbelow_data);
		break;
	case 1: // Aqua buttons
		buttonImages[ButtonImageMenu]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageHelp]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageMax]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageRestore]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageMin]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageClose]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageSticky]->SetNormal(aqua_sticky_data,16,16);
		buttonImages[ButtonImageUnSticky]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageShade]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageUnShade]->SetNormal(aqua_default_data,16,16);
	
		buttonImages[ButtonImageAbove]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageUnAbove]->SetNormal(aqua_above_data,16,16);
		buttonImages[ButtonImageBelow]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageUnBelow]->SetNormal(aqua_below_data,16,16);
		
		buttonImages[ButtonImageClose]->SetHovered(aqua_close_data);
		buttonImages[ButtonImageMax]->SetHovered(aqua_max_data);
		buttonImages[ButtonImageMin]->SetHovered(aqua_min_data);
		buttonImages[ButtonImageRestore]->SetHovered(aqua_max_data);
		buttonImages[ButtonImageUnSticky]->SetHovered(aqua_un_sticky_data);
		buttonImages[ButtonImageHelp]->SetHovered(aqua_help_data);
		buttonImages[ButtonImageAbove]->SetHovered(aqua_above_data);
		buttonImages[ButtonImageBelow]->SetHovered(aqua_below_data);
		buttonImages[ButtonImageShade]->SetHovered(aqua_shade_data);
		buttonImages[ButtonImageUnShade]->SetHovered(aqua_shade_data);
		break;
	case 2: // Knifty buttons
		buttonImages[ButtonImageHelp]->SetNormal(knifty_help_data);
		buttonImages[ButtonImageMax]->SetNormal(knifty_max_data);
		buttonImages[ButtonImageRestore]->SetNormal(knifty_restore_data);
		buttonImages[ButtonImageMin]->SetNormal(knifty_min_data);
		buttonImages[ButtonImageClose]->SetNormal(knifty_close_data);
		buttonImages[ButtonImageSticky]->SetNormal(knifty_sticky_data);
		buttonImages[ButtonImageUnSticky]->SetNormal(knifty_un_sticky_data);
		buttonImages[ButtonImageShade]->SetNormal(knifty_shade_data);
		buttonImages[ButtonImageUnShade]->SetNormal(knifty_shade_data);

		buttonImages[ButtonImageAbove]->SetNormal(knifty_above_data);
		buttonImages[ButtonImageUnAbove]->SetNormal(knifty_unabove_data);
		buttonImages[ButtonImageBelow]->SetNormal(knifty_below_data);
		buttonImages[ButtonImageUnBelow]->SetNormal(knifty_unbelow_data);
		break;
	
	case 3:	// Handpainted
		buttonImages[ButtonImageHelp]->SetNormal(handpainted_help_data);
		buttonImages[ButtonImageMax]->SetNormal(handpainted_max_data);
		buttonImages[ButtonImageRestore]->SetNormal(handpainted_restore_data);
		buttonImages[ButtonImageMin]->SetNormal(handpainted_min_data);
		buttonImages[ButtonImageClose]->SetNormal(handpainted_close_data);
		buttonImages[ButtonImageSticky]->SetNormal(handpainted_sticky_data);
		buttonImages[ButtonImageUnSticky]->SetNormal(handpainted_un_sticky_data);
		buttonImages[ButtonImageShade]->SetNormal(handpainted_shade_data);
		buttonImages[ButtonImageUnShade]->SetNormal(handpainted_un_shade_data);
	
		buttonImages[ButtonImageAbove]->SetNormal(handpainted_above_data);
		buttonImages[ButtonImageUnAbove]->SetNormal(handpainted_unabove_data);
		buttonImages[ButtonImageBelow]->SetNormal(handpainted_below_data);
		buttonImages[ButtonImageUnBelow]->SetNormal(handpainted_unbelow_data);
		break;
	case 4: // SVG
		buttonImages[ButtonImageMenu]->SetNormal(svg_menu_data);
		buttonImages[ButtonImageHelp]->SetNormal(svg_help_data);
		buttonImages[ButtonImageMax]->SetNormal(svg_max_data);
		buttonImages[ButtonImageRestore]->SetNormal(svg_restore_data);
		buttonImages[ButtonImageMin]->SetNormal(svg_min_data);
		buttonImages[ButtonImageClose]->SetNormal(svg_close_data);
		buttonImages[ButtonImageSticky]->SetNormal(svg_sticky_data);
		buttonImages[ButtonImageUnSticky]->SetNormal(svg_unsticky_data);
		buttonImages[ButtonImageShade]->SetNormal(svg_shade_data);
		buttonImages[ButtonImageUnShade]->SetNormal(svg_shade_data);
	
		buttonImages[ButtonImageAbove]->SetNormal(svg_above_data);
		buttonImages[ButtonImageUnAbove]->SetNormal(svg_above_data);
		buttonImages[ButtonImageBelow]->SetNormal(svg_below_data);
		buttonImages[ButtonImageUnBelow]->SetNormal(svg_below_data);
		break;
	case 5: // Vista
		buttonImages[ButtonImageMenu]->SetNormal(vista_menu_data,26,15);
		buttonImages[ButtonImageMenu]->SetHovered(vista_menu_hovered_data);
		buttonImages[ButtonImageMenu]->SetPressed(vista_menu_pressed_data);

		buttonImages[ButtonImageHelp]->SetNormal(vista_help_data,26,15);
		buttonImages[ButtonImageHelp]->SetHovered(vista_help_hovered_data);
		buttonImages[ButtonImageHelp]->SetPressed(vista_help_pressed_data);

		buttonImages[ButtonImageMax]->SetNormal(vista_max_data,27,15);
		buttonImages[ButtonImageMax]->SetHovered(vista_max_hovered_data);
		buttonImages[ButtonImageMax]->SetPressed(vista_max_pressed_data);
		buttonImages[ButtonImageRestore]->SetNormal(vista_restore_data,27,15);
		buttonImages[ButtonImageRestore]->SetHovered(vista_restore_hovered_data);
		buttonImages[ButtonImageRestore]->SetPressed(vista_restore_pressed_data);
		buttonImages[ButtonImageMin]->SetNormal(vista_min_data,26,15);
		buttonImages[ButtonImageMin]->SetHovered(vista_min_hovered_data);
		buttonImages[ButtonImageMin]->SetPressed(vista_min_pressed_data);
		buttonImages[ButtonImageClose]->SetNormal(vista_close_data,40,15);
		buttonImages[ButtonImageClose]->SetHovered(vista_close_hovered_data);
		buttonImages[ButtonImageClose]->SetPressed(vista_close_pressed_data);

		buttonImages[ButtonImageSticky]->SetNormal(vista_sticky_data,26,15);
		buttonImages[ButtonImageSticky]->SetHovered(vista_sticky_hovered_data);
		buttonImages[ButtonImageSticky]->SetPressed(vista_sticky_pressed_data);
		buttonImages[ButtonImageUnSticky]->SetNormal(vista_un_sticky_data,26,15);
		buttonImages[ButtonImageUnSticky]->SetHovered(vista_un_sticky_hovered_data);
		buttonImages[ButtonImageUnSticky]->SetPressed(vista_un_sticky_pressed_data);

		buttonImages[ButtonImageAbove]->SetNormal(vista_above_data,26,15);
		buttonImages[ButtonImageAbove]->SetHovered(vista_above_hovered_data);
		buttonImages[ButtonImageAbove]->SetPressed(vista_above_pressed_data);
		buttonImages[ButtonImageUnAbove]->SetNormal(vista_un_above_data,26,15);
		buttonImages[ButtonImageUnAbove]->SetHovered(vista_un_above_hovered_data);
		buttonImages[ButtonImageUnAbove]->SetPressed(vista_un_above_pressed_data);

		buttonImages[ButtonImageBelow]->SetNormal(vista_below_data,26,15);
		buttonImages[ButtonImageBelow]->SetHovered(vista_below_hovered_data);
		buttonImages[ButtonImageBelow]->SetPressed(vista_below_pressed_data);
		buttonImages[ButtonImageUnBelow]->SetNormal(vista_un_below_data,26,15);
		buttonImages[ButtonImageUnBelow]->SetHovered(vista_un_below_hovered_data);
		buttonImages[ButtonImageUnBelow]->SetPressed(vista_un_below_pressed_data);

		buttonImages[ButtonImageShade]->SetNormal(vista_shade_data,26,15);
		buttonImages[ButtonImageShade]->SetHovered(vista_shade_hovered_data);
		buttonImages[ButtonImageShade]->SetPressed(vista_shade_pressed_data);
		buttonImages[ButtonImageUnShade]->SetNormal(vista_un_shade_data,26,15);
		buttonImages[ButtonImageUnShade]->SetHovered(vista_un_shade_hovered_data);
		buttonImages[ButtonImageUnShade]->SetPressed(vista_un_shade_pressed_data);
	
		for (int i=0;i<ButtonImageCount;i++)
		{
			buttonImages[i]->setSpace(1,0);
			buttonImages[i]->setDrawMode(1);
		}
		buttonImages[ButtonImageMax]->setSpace(0,0);
		buttonImages[ButtonImageRestore]->setSpace(0,0);
		buttonImages[ButtonImageMin]->setSpace(0,0);
		buttonImages[ButtonImageClose]->setSpace(0,0);

		break;
	case 6: // Kubuntu Dapper
		buttonImages[ButtonImageMenu]->SetNormal(dapper_menu_data,28,17);
		buttonImages[ButtonImageMenu]->SetHovered(dapper_menu_hovered_data);
		buttonImages[ButtonImageMenu]->SetPressed(dapper_menu_pressed_data);

		buttonImages[ButtonImageHelp]->SetNormal(dapper_help_data,28,17);
		buttonImages[ButtonImageHelp]->SetHovered(dapper_help_hovered_data);
		buttonImages[ButtonImageHelp]->SetPressed(dapper_help_pressed_data);

		buttonImages[ButtonImageMax]->SetNormal(dapper_max_data,28,17);
		buttonImages[ButtonImageMax]->SetHovered(dapper_max_hovered_data);
		buttonImages[ButtonImageMax]->SetPressed(dapper_max_pressed_data);
		buttonImages[ButtonImageRestore]->SetNormal(dapper_restore_data,28,17);
		buttonImages[ButtonImageRestore]->SetHovered(dapper_restore_hovered_data);
		buttonImages[ButtonImageRestore]->SetPressed(dapper_restore_pressed_data);
		buttonImages[ButtonImageMin]->SetNormal(dapper_min_data,28,17);
		buttonImages[ButtonImageMin]->SetHovered(dapper_min_hovered_data);
		buttonImages[ButtonImageMin]->SetPressed(dapper_min_pressed_data);
		buttonImages[ButtonImageClose]->SetNormal(dapper_close_data,28,17);
		buttonImages[ButtonImageClose]->SetHovered(dapper_close_hovered_data);
		buttonImages[ButtonImageClose]->SetPressed(dapper_close_pressed_data);

		buttonImages[ButtonImageSticky]->SetNormal(dapper_sticky_data,28,17);
		buttonImages[ButtonImageSticky]->SetHovered(dapper_sticky_hovered_data);
		buttonImages[ButtonImageSticky]->SetPressed(dapper_sticky_pressed_data);
		buttonImages[ButtonImageUnSticky]->SetNormal(dapper_un_sticky_data,28,17);
		buttonImages[ButtonImageUnSticky]->SetHovered(dapper_un_sticky_hovered_data);
		buttonImages[ButtonImageUnSticky]->SetPressed(dapper_un_sticky_pressed_data);

		buttonImages[ButtonImageAbove]->SetNormal(dapper_above_data,28,17);
		buttonImages[ButtonImageAbove]->SetHovered(dapper_above_hovered_data);
		buttonImages[ButtonImageAbove]->SetPressed(dapper_above_pressed_data);
		buttonImages[ButtonImageUnAbove]->SetNormal(dapper_un_above_data,28,17);
		buttonImages[ButtonImageUnAbove]->SetHovered(dapper_un_above_hovered_data);
		buttonImages[ButtonImageUnAbove]->SetPressed(dapper_un_above_pressed_data);


		buttonImages[ButtonImageBelow]->SetNormal(dapper_below_data,28,17);
		buttonImages[ButtonImageBelow]->SetHovered(dapper_below_hovered_data);
		buttonImages[ButtonImageBelow]->SetPressed(dapper_below_pressed_data);

		buttonImages[ButtonImageUnBelow]->SetNormal(dapper_un_below_data,28,17);
		buttonImages[ButtonImageUnBelow]->SetHovered(dapper_un_below_hovered_data);
		buttonImages[ButtonImageUnBelow]->SetPressed(dapper_un_below_pressed_data);

		buttonImages[ButtonImageShade]->SetNormal(dapper_shade_data,28,17);
		buttonImages[ButtonImageShade]->SetHovered(dapper_shade_hovered_data);
		buttonImages[ButtonImageShade]->SetPressed(dapper_shade_pressed_data);
		buttonImages[ButtonImageUnShade]->SetNormal(dapper_un_shade_data,28,17);
		buttonImages[ButtonImageUnShade]->SetHovered(dapper_un_shade_hovered_data);
		buttonImages[ButtonImageUnShade]->SetPressed(dapper_un_shade_pressed_data);
	
		for (int i=0;i<ButtonImageCount;i++)
		{
			buttonImages[i]->setSpace(1,0);
			buttonImages[i]->setDrawMode(0);
		}
		buttonImages[ButtonImageMax]->setSpace(0,0);
		buttonImages[ButtonImageRestore]->setSpace(0,0);
		buttonImages[ButtonImageMin]->setSpace(0,0);
		buttonImages[ButtonImageClose]->setSpace(0,0);
		break;

	case 7: // Kubuntu-Edgy
		buttonImages[ButtonImageMenu]->SetNormal(edgy_menu_data,28,17);
		buttonImages[ButtonImageMenu]->SetHovered(edgy_menu_hovered_data);
		buttonImages[ButtonImageMenu]->SetPressed(edgy_menu_pressed_data);

		buttonImages[ButtonImageHelp]->SetNormal(edgy_help_data,28,17);
		buttonImages[ButtonImageHelp]->SetHovered(edgy_help_hovered_data);
		buttonImages[ButtonImageHelp]->SetPressed(edgy_help_pressed_data);

		buttonImages[ButtonImageMax]->SetNormal(edgy_max_data,28,17);
		buttonImages[ButtonImageMax]->SetHovered(edgy_max_hovered_data);
		buttonImages[ButtonImageMax]->SetPressed(edgy_max_pressed_data);
		buttonImages[ButtonImageRestore]->SetNormal(edgy_restore_data,28,17);
		buttonImages[ButtonImageRestore]->SetHovered(edgy_restore_hovered_data);
		buttonImages[ButtonImageRestore]->SetPressed(edgy_restore_pressed_data);
		buttonImages[ButtonImageMin]->SetNormal(edgy_min_data,28,17);
		buttonImages[ButtonImageMin]->SetHovered(edgy_min_hovered_data);
		buttonImages[ButtonImageMin]->SetPressed(edgy_min_pressed_data);
		buttonImages[ButtonImageClose]->SetNormal(edgy_close_data,28,17);
		buttonImages[ButtonImageClose]->SetHovered(edgy_close_hovered_data);
		buttonImages[ButtonImageClose]->SetPressed(edgy_close_pressed_data);

		buttonImages[ButtonImageSticky]->SetNormal(edgy_sticky_data,28,17);
		buttonImages[ButtonImageSticky]->SetHovered(edgy_sticky_hovered_data);
		buttonImages[ButtonImageSticky]->SetPressed(edgy_sticky_pressed_data);
		buttonImages[ButtonImageUnSticky]->SetNormal(edgy_un_sticky_data,28,17);
		buttonImages[ButtonImageUnSticky]->SetHovered(edgy_un_sticky_hovered_data);
		buttonImages[ButtonImageUnSticky]->SetPressed(edgy_un_sticky_pressed_data);

		buttonImages[ButtonImageAbove]->SetNormal(edgy_above_data,28,17);
		buttonImages[ButtonImageAbove]->SetHovered(edgy_above_hovered_data);
		buttonImages[ButtonImageAbove]->SetPressed(edgy_above_pressed_data);
		buttonImages[ButtonImageUnAbove]->SetNormal(edgy_un_above_data,28,17);
		buttonImages[ButtonImageUnAbove]->SetHovered(edgy_un_above_hovered_data);
		buttonImages[ButtonImageUnAbove]->SetPressed(edgy_un_above_pressed_data);


		buttonImages[ButtonImageBelow]->SetNormal(edgy_below_data,28,17);
		buttonImages[ButtonImageBelow]->SetHovered(edgy_below_hovered_data);
		buttonImages[ButtonImageBelow]->SetPressed(edgy_below_pressed_data);

		buttonImages[ButtonImageUnBelow]->SetNormal(edgy_un_below_data,28,17);
		buttonImages[ButtonImageUnBelow]->SetHovered(edgy_un_below_hovered_data);
		buttonImages[ButtonImageUnBelow]->SetPressed(edgy_un_below_pressed_data);

		buttonImages[ButtonImageShade]->SetNormal(edgy_shade_data,28,17);
		buttonImages[ButtonImageShade]->SetHovered(edgy_shade_hovered_data);
		buttonImages[ButtonImageShade]->SetPressed(edgy_shade_pressed_data);
		buttonImages[ButtonImageUnShade]->SetNormal(edgy_un_shade_data,28,17);
		buttonImages[ButtonImageUnShade]->SetHovered(edgy_un_shade_hovered_data);
		buttonImages[ButtonImageUnShade]->SetPressed(edgy_un_shade_pressed_data);
	
		for (int i=0;i<ButtonImageCount;i++)
		{
			buttonImages[i]->setSpace(1,0);
			buttonImages[i]->setDrawMode(0);
		}
		buttonImages[ButtonImageMax]->setSpace(0,0);
		buttonImages[ButtonImageRestore]->setSpace(0,0);
		buttonImages[ButtonImageMin]->setSpace(0,0);
		buttonImages[ButtonImageClose]->setSpace(0,0);

		break;
	case 8: // Kubuntu-Feisty
		buttonImages[ButtonImageMenu]->SetNormal(feisty_menu_data,21,17);
		buttonImages[ButtonImageMenu]->SetHovered(feisty_menu_hovered_data);
		buttonImages[ButtonImageMenu]->SetPressed(feisty_menu_pressed_data);

		buttonImages[ButtonImageHelp]->SetNormal(feisty_help_data,28,17);
		buttonImages[ButtonImageHelp]->SetHovered(feisty_help_hovered_data);
		buttonImages[ButtonImageHelp]->SetPressed(feisty_help_pressed_data);

		buttonImages[ButtonImageMax]->SetNormal(feisty_max_data,28,17);
		buttonImages[ButtonImageMax]->SetHovered(feisty_max_hovered_data);
		buttonImages[ButtonImageMax]->SetPressed(feisty_max_pressed_data);
		buttonImages[ButtonImageRestore]->SetNormal(feisty_restore_data,28,17);
		buttonImages[ButtonImageRestore]->SetHovered(feisty_restore_hovered_data);
		buttonImages[ButtonImageRestore]->SetPressed(feisty_restore_pressed_data);
		buttonImages[ButtonImageMin]->SetNormal(feisty_min_data,28,17);
		buttonImages[ButtonImageMin]->SetHovered(feisty_min_hovered_data);
		buttonImages[ButtonImageMin]->SetPressed(feisty_min_pressed_data);
		buttonImages[ButtonImageClose]->SetNormal(feisty_close_data,28,17);
		buttonImages[ButtonImageClose]->SetHovered(feisty_close_hovered_data);
		buttonImages[ButtonImageClose]->SetPressed(feisty_close_pressed_data);

		buttonImages[ButtonImageSticky]->SetNormal(feisty_sticky_data,28,17);
		buttonImages[ButtonImageSticky]->SetHovered(feisty_sticky_hovered_data);
		buttonImages[ButtonImageSticky]->SetPressed(feisty_sticky_pressed_data);
		buttonImages[ButtonImageUnSticky]->SetNormal(feisty_un_sticky_data,28,17);
		buttonImages[ButtonImageUnSticky]->SetHovered(feisty_un_sticky_hovered_data);
		buttonImages[ButtonImageUnSticky]->SetPressed(feisty_un_sticky_pressed_data);

		buttonImages[ButtonImageAbove]->SetNormal(feisty_above_data,28,17);
		buttonImages[ButtonImageAbove]->SetHovered(feisty_above_hovered_data);
		buttonImages[ButtonImageAbove]->SetPressed(feisty_above_pressed_data);
		buttonImages[ButtonImageUnAbove]->SetNormal(feisty_un_above_data,28,17);
		buttonImages[ButtonImageUnAbove]->SetHovered(feisty_un_above_hovered_data);
		buttonImages[ButtonImageUnAbove]->SetPressed(feisty_un_above_pressed_data);


		buttonImages[ButtonImageBelow]->SetNormal(feisty_below_data,28,17);
		buttonImages[ButtonImageBelow]->SetHovered(feisty_below_hovered_data);
		buttonImages[ButtonImageBelow]->SetPressed(feisty_below_pressed_data);

		buttonImages[ButtonImageUnBelow]->SetNormal(feisty_un_below_data,28,17);
		buttonImages[ButtonImageUnBelow]->SetHovered(feisty_un_below_hovered_data);
		buttonImages[ButtonImageUnBelow]->SetPressed(feisty_un_below_pressed_data);

		buttonImages[ButtonImageShade]->SetNormal(feisty_shade_data,28,17);
		buttonImages[ButtonImageShade]->SetHovered(feisty_shade_hovered_data);
		buttonImages[ButtonImageShade]->SetPressed(feisty_shade_pressed_data);
		buttonImages[ButtonImageUnShade]->SetNormal(feisty_un_shade_data,28,17);
		buttonImages[ButtonImageUnShade]->SetHovered(feisty_un_shade_hovered_data);
		buttonImages[ButtonImageUnShade]->SetPressed(feisty_un_shade_pressed_data);
	
		for (int i=0;i<ButtonImageCount;i++)
		{
			buttonImages[i]->setSpace(1,0);
			buttonImages[i]->setDrawMode(0);
		}
		buttonImages[ButtonImageMax]->setSpace(0,0);
		buttonImages[ButtonImageRestore]->setSpace(0,0);
		buttonImages[ButtonImageMin]->setSpace(0,0);
		buttonImages[ButtonImageClose]->setSpace(0,0);

		break;

	}


	for (int i=0;i<ButtonImageCount;i++)buttonImages[i]->finish();
}








CrystalClient::CrystalClient(KDecorationBridge *b,CrystalFactory *f)
: KDecoration(b,f)
{
	::factory->clients.append(this);
}

CrystalClient::~CrystalClient()
{
	::factory->clients.removeAll(this);
	for (int n=0; n<ButtonTypeCount; n++) {
		if (button[n]) delete button[n];
	}
}

void CrystalClient::init()
{
	createMainWidget();
	widget()->installEventFilter(this);

	widget()->setAttribute(Qt::WA_NoSystemBackground);
	FullMax=false;
	if (!options()->moveResizeMaximizedWindows())
		FullMax=(maximizeMode()==MaximizeFull);
	
	// setup layout
	mainlayout = new QGridLayout(widget()); // 4x3 grid
	titlelayout = new QHBoxLayout();
	titlebar_ = new QSpacerItem(1, ::factory->titlesize-1, QSizePolicy::Expanding,
					QSizePolicy::Fixed);
	titlelayout->setMargin(0);
	titlelayout->setSpacing(0);

	mainlayout->setSizeConstraint(QLayout::SetNoConstraint);
	mainlayout->setRowMinimumHeight(0, (::factory->buttontheme==5)?0:1);
	mainlayout->setRowMinimumHeight(1, 0);
	mainlayout->setRowMinimumHeight(2, 0);
	mainlayout->setRowMinimumHeight(3, 0/*::factory->borderwidth*1*/);

	mainlayout->setColumnMinimumWidth(0,borderSpacing());
	mainlayout->setColumnMinimumWidth(1,0);
	mainlayout->setColumnMinimumWidth(2,borderSpacing());

	mainlayout->setRowStretch(0,0);
	mainlayout->setRowStretch(1,0);
	mainlayout->setRowStretch(2,10);
	mainlayout->setRowStretch(3,0);
	mainlayout->setColumnStretch(1, 10);
	
	mainlayout->setMargin(0);
	mainlayout->setSpacing(0);
	mainlayout->addLayout(titlelayout, 1, 1);

	if (isPreview()) {
		char c[512];
		QLabel *label;
#define		VERSION "KWIN4-pre1"
		sprintf(c,"<center><b>Crystal %s Preview</b><br>Built: %s</center>",VERSION,__DATE__);
		mainlayout->addItem(new QSpacerItem(1, 1,QSizePolicy::Expanding,QSizePolicy::Fixed), 0, 1);
		mainlayout->addItem(new QSpacerItem(1, ::factory->borderwidth,QSizePolicy::Expanding,QSizePolicy::Expanding), 3, 1);
		label = new QLabel(i18n(c), widget());
		label->setAutoFillBackground(true);
		mainlayout->addWidget(label, 2, 1);
	} else {
		mainlayout->addItem(new QSpacerItem(0, 0), 2, 1);
	}
	
	updateMask();

	for (int n=0; n<ButtonTypeCount; n++) button[n] = 0;
	addButtons(titlelayout, options()->titleButtonsLeft());

	titlelayout->addItem(titlebar_);

	{
		CrystalButton* lastbutton=addButtons(titlelayout, options()->titleButtonsRight());
		if (lastbutton)lastbutton->setFirstLast(false,true);
	}

	connect( this, SIGNAL( keepAboveChanged( bool )), SLOT( keepAboveChange( bool )));
	connect( this, SIGNAL( keepBelowChanged( bool )), SLOT( keepBelowChange( bool )));
	
	updateLayout();
}

void CrystalClient::updateMask()
{
	if ((::factory->roundCorners==0)|| (!options()->moveResizeMaximizedWindows() && maximizeMode() & MaximizeFull ) ) 
	{
		setMask(QRegion(widget()->rect()));
		return;
	}
	
	int cornersFlag = ::factory->roundCorners;
	int r(width());
	int b(height());
	QRegion mask;

	mask=QRegion(widget()->rect());
	
	// Remove top-left corner.
	if(cornersFlag & TOP_LEFT) {
		mask -= QRegion(0, 0, 5, 1);
		mask -= QRegion(0, 1, 3, 1);
		mask -= QRegion(0, 2, 2, 1);
		mask -= QRegion(0, 3, 1, 2);
	}
	// Remove top-right corner.
	if(cornersFlag & TOP_RIGHT) {
		mask -= QRegion(r - 5, 0, 5, 1);
		mask -= QRegion(r - 3, 1, 3, 1);
		mask -= QRegion(r - 2, 2, 2, 1);
		mask -= QRegion(r - 1, 3, 1, 2);
	}
	// Remove bottom-left corner.
	if(cornersFlag & BOT_LEFT) {
		mask -= QRegion(0, b - 5, 1, 3);
		mask -= QRegion(0, b - 3, 2, 1);
		mask -= QRegion(0, b - 2, 3, 1);
		mask -= QRegion(0, b - 1, 5, 1);
	}
	// Remove bottom-right corner.
	if(cornersFlag & BOT_RIGHT) {
		mask -= QRegion(r - 5, b - 1, 5, 1);
		mask -= QRegion(r - 3, b - 2, 3, 1);
		mask -= QRegion(r - 2, b - 3, 2, 1);
		mask -= QRegion(r - 1, b - 5, 1, 2);
	}
	
	setMask(mask);
}

CrystalButton* CrystalClient::addButtons(QBoxLayout *layout, const QString& s)
{
	ButtonImage *bitmap;
	QString tip;
	CrystalButton *lastone=NULL;

	if (s.length() > 0) 
	{
		for (int n=0; n < s.length(); n++)
		{
			CrystalButton *current=NULL;
			switch (s[n].toAscii()) {
			case 'M': // Menu button
				if (!button[ButtonMenu]) {
					button[ButtonMenu] = current = new CrystalButton(this, "menu", i18n("Menu"), ButtonMenu, ::factory->buttonImages[ButtonImageMenu]);
					connect(button[ButtonMenu], SIGNAL(pressed()), this, SLOT(menuButtonPressed()));
				}
				break;

			case 'S': // Sticky button
				if (!button[ButtonSticky]) {
					if (isOnAllDesktops()) {
						bitmap = ::factory->buttonImages[ButtonImageSticky];
						tip = i18n("Not on all desktops");
					} else {
						bitmap = ::factory->buttonImages[ButtonImageUnSticky];
						tip = i18n("On All Desktops");
					}
					button[ButtonSticky] =current=new CrystalButton(this, "sticky", tip,ButtonSticky, bitmap);
					connect(button[ButtonSticky], SIGNAL(clicked()),this, SLOT(toggleOnAllDesktops()));
				}
				break;

			case 'H': // Help button
				if (providesContextHelp()) {
					button[ButtonHelp] =current=
						new CrystalButton(this, "help", i18n("Help"),ButtonHelp, ::factory->buttonImages[ButtonImageHelp]);
					connect(button[ButtonHelp], SIGNAL(clicked()),this, SLOT(showContextHelp()));
				}
				break;

			case 'I': // Minimize button
				if ((!button[ButtonMin]) && isMinimizable())  {
					button[ButtonMin] =current=
						new CrystalButton(this, "iconify", i18n("Minimize"),ButtonMin, ::factory->buttonImages[ButtonImageMin]);
					connect(button[ButtonMin], SIGNAL(clicked()),this, SLOT(minButtonPressed()));
				}
				break;

			case 'F': // Above button
				if (!button[ButtonAbove]) {
					button[ButtonAbove] =current=
						new CrystalButton(this, "above", i18n("Keep Above Others"),ButtonAbove, ::factory->buttonImages[keepAbove()?ButtonImageUnAbove:ButtonImageAbove]);
					connect(button[ButtonAbove], SIGNAL(clicked()),this, SLOT(aboveButtonPressed()));
				}
				break;
		
			case 'B': // Below button
				if ((!button[ButtonBelow])) {
					button[ButtonBelow] =current=
						new CrystalButton(this, "below", i18n("Keep Below Others"),ButtonBelow, ::factory->buttonImages[keepBelow()?ButtonImageUnBelow:ButtonImageBelow]);
					connect(button[ButtonBelow], SIGNAL(clicked()),this, SLOT(belowButtonPressed()));
				}
				break;

			case 'L': // Shade button
				if ((!button[ButtonShade]) && isShadeable())  {
					button[ButtonShade] =current=
						new CrystalButton(this, "shade", i18n("Shade"),ButtonShade, ::factory->buttonImages[ButtonImageShade]);
					connect(button[ButtonShade], SIGNAL(clicked()),this, SLOT(shadeButtonPressed()));
				}
				break;
		  
			case 'A': // Maximize button
				if ((!button[ButtonMax]) && isMaximizable()) 
				{
					if (maximizeMode() == MaximizeFull) 
					{
						bitmap = ::factory->buttonImages[ButtonImageRestore];
						tip = i18n("Restore");
					} else {
						bitmap = ::factory->buttonImages[ButtonImageMax];
						tip = i18n("Maximize");
					}
					button[ButtonMax]  =current=
						new CrystalButton(this, "maximize", tip,ButtonMax, bitmap);
					connect(button[ButtonMax], SIGNAL(clicked()),this, SLOT(maxButtonPressed()));
				}
				break;

			case 'X': // Close button
				if (isCloseable()) {
					button[ButtonClose] =current=
					new CrystalButton(this, "close", i18n("Close"),ButtonClose, ::factory->buttonImages[ButtonImageClose]);
					connect(button[ButtonClose], SIGNAL(clicked()),this, SLOT(closeButtonPressed()));
				}
				break;

			case '_': // Spacer item
				layout->addSpacing(4);
				current=NULL;
				break;
			}
			
			if (current)
			{
				layout->addWidget(current);
				if (layout->indexOf(current)==0)current->setFirstLast(true,false);
			}
			lastone=current;
		}
	}
	return lastone;
}

void CrystalClient::activeChange()
{
	Repaint();
	if (isActive()) ::factory->clients.at(::factory->clients.indexOf(this));
}

void CrystalClient::captionChange()
{
	if (::factory->drawcaption) widget()->repaint(titlebar_->geometry());
	widget()->setToolTip(caption());
}

void CrystalClient::desktopChange()
{
	bool d = isOnAllDesktops();
	if (button[ButtonSticky]) {
		button[ButtonSticky]->setBitmap(::factory->buttonImages[d ? ButtonImageSticky : ButtonImageUnSticky ]);
		button[ButtonSticky]->setToolTip(d ? i18n("Not on all desktops") : i18n("On All Desktops"));
	}
}

void CrystalClient::iconChange()
{
	if (button[ButtonMenu]) {
		button[ButtonMenu]->setBitmap(::factory->buttonImages[ButtonImageMenu]);
	}
}

void CrystalClient::maximizeChange()
{
	bool m = (maximizeMode() == MaximizeFull);
	if (button[ButtonMax]) {
		button[ButtonMax]->setBitmap(::factory->buttonImages[ m ? ButtonImageRestore : ButtonImageMax ]);
		button[ButtonMax]->setToolTip(m ? i18n("Restore") : i18n("Maximize"));
	}
		
	if (!options()->moveResizeMaximizedWindows())
	{
		FullMax=m;
		updateLayout();
		Repaint();
	}
}

void CrystalClient::updateLayout()
{
	if (FullMax)
	{
		mainlayout->setColumnMinimumWidth(0,0);
		mainlayout->setColumnMinimumWidth(2,0);
	}else{
		mainlayout->setColumnMinimumWidth(2,borderSpacing());
		mainlayout->setColumnMinimumWidth(0,borderSpacing());
	}
	
	mainlayout->setRowMinimumHeight(0, (FullMax||::factory->buttontheme==5)?0:1);
	for (int i=0;i<ButtonTypeCount;i++)if (button[i])
		button[i]->resetSize(FullMax);
	widget()->layout()->activate();
}

int CrystalClient::borderSpacing()
{
	if (::factory->roundCorners)
		return (::factory->borderwidth<=5)?5: ::factory->borderwidth;
	return (::factory->borderwidth<=1)?1: ::factory->borderwidth;
}

void CrystalClient::shadeChange()
{ 
	if (button[ButtonShade])
	{
		button[ButtonShade]->setBitmap(::factory->buttonImages[isShade()?ButtonImageUnShade:ButtonImageShade]);
	}
	Repaint();
	return;
}

void CrystalClient::borders(int &l, int &r, int &t, int &b) const
{
	l = r = ::factory->borderwidth;
	t = ::factory->titlesize;
	if (!isShade())b = ::factory->borderwidth; else b=0;
	
	if (!options()->moveResizeMaximizedWindows() )
	{
		if ( maximizeMode() & MaximizeHorizontal )l=r=1;
		if ( maximizeMode() & MaximizeVertical )
		{
			b=isShade()?0:1;
			if (!isShade() && ( maximizeMode() & MaximizeHorizontal ))b=0;
		}
		if ( (maximizeMode() & MaximizeFull)==MaximizeFull)
			l=r=0;
	}
}

void CrystalClient::resize(const QSize &size)
{
	widget()->resize(size);
}

QSize CrystalClient::minimumSize() const
{
	return widget()->minimumSize();
}

KDecoration::Position CrystalClient::mousePosition(const QPoint &point) const
{
	const int corner = 20;
	Position pos;
	const int RESIZESIZE=::factory->borderwidth;
	
	if (isShade() || !isResizable()) pos=PositionCenter; 
	else if (point.y() <= 3) {
		// inside top frame
		if (point.x() <= corner)                 pos = PositionTopLeft;
		else if (point.x() >= (width()-corner))  pos = PositionTopRight;
		else                                     pos = PositionTop;
	} else if (point.y() >= (height()-RESIZESIZE)) {
		// inside handle
		if (point.x() <= corner)                 pos = PositionBottomLeft;
		else if (point.x() >= (width()-corner))  pos = PositionBottomRight;
		else                                     pos = PositionBottom;
	} else if (point.x() <= RESIZESIZE) {
		// on left frame
		if (point.y() <= corner)                 pos = PositionTopLeft;
		else if (point.y() >= (height()-corner)) pos = PositionBottomLeft;
		else                                     pos = PositionLeft;
	} else if (point.x() >= width()-RESIZESIZE) {
		// on right frame
		if (point.y() <= corner)                 pos = PositionTopRight;
		else if (point.y() >= (height()-corner)) pos = PositionBottomRight;
		else                                     pos = PositionRight;
	} else {
		// inside the frame
		pos = PositionCenter;
	}
	return pos;
}

bool CrystalClient::eventFilter(QObject *obj, QEvent *e)
{
	if (obj != widget()) {
		return false;
	}
	
	switch (e->type()) {
	case QEvent::MouseButtonDblClick:
		mouseDoubleClickEvent(static_cast<QMouseEvent *>(e));
		return true;
	case QEvent::MouseButtonPress:
		processMousePressEvent(static_cast<QMouseEvent *>(e));
		return true;
	case QEvent::Paint:
		paintEvent(static_cast<QPaintEvent *>(e));
		return true;
	case QEvent::Wheel:
		mouseWheelEvent(static_cast<QWheelEvent *>(e));
		return true;

	case QEvent::Resize: 
		resizeEvent(static_cast<QResizeEvent *>(e));
		return true;

	case QEvent::Show:
		showEvent(static_cast<QShowEvent *>(e));
		return true;
	case QEvent::Move:
		moveEvent(static_cast<QMoveEvent *>(e));
		return true;
	default:return false;
	}

	return false;
}

void CrystalClient::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (/*(titlebar_->geometry().contains(e->pos()))&&*/(e->button()==Qt::LeftButton)) titlebarDblClickOperation();
	else {
		QMouseEvent me(QEvent::MouseButtonPress,e->pos(),e->button(),e->buttons(),e->modifiers());
		processMousePressEvent(&me);
	}
}

void CrystalClient::mouseWheelEvent(QWheelEvent *e)
{
	if (::factory->wheelTask)
	{
		QList <CrystalClient*> *l=&(::factory->clients);
		QList<CrystalClient*>::iterator i = l->begin(), activeOne;
		Window client;
		if (l->begin() == l->end()) return;
		activeOne = l->begin();
		while (i < l->end())
		{
		    if (*i == NULL) return;
		    if ((*i)->isActive()) activeOne = i;
		    i++;
		}
		i = activeOne;

		do
		{
			if(e->delta()>0)
			{
				i++;
				if (i >= l->end()) i = l->begin();
			}else{
				i--;
				if (i < l->begin()) i = l->end()-1;
			}
			if (*i == NULL) {
				printf("*i == NULL\n");
				return;
			}
			
			client = (*i)->windowId();
			if (client == 0) { // FALLBACK
				titlebarMouseWheelOperation(e->delta());
				return;
			}
			KWindowInfo info=KWindowSystem::windowInfo(client, NET::WMState|NET::XAWMState);
			if (((*i)->desktop()==desktop()) && !info.isMinimized())break;
		}while(i != activeOne);
			
		if (client)
			KWindowSystem::forceActiveWindow(client);
	}else{
		titlebarMouseWheelOperation(e->delta());
	}
}

void CrystalClient::paintEvent(QPaintEvent*)
{
	if (!CrystalFactory::initialized()) return;

	QPainter painter(widget());

	// draw the titlebar
	WND_CONFIG* wndcfg=(isActive()?&::factory->active:&::factory->inactive);

	int drawFrame;

	{
		QRect r;
		QPoint p=widget()->mapToGlobal(QPoint(0,0));
		int bl,br,bt,bb;
		borders(bl,br,bt,bb);
	
		QPixmap pufferPixmap(widget()->width(), bt);
		QPainter pufferPainter(&pufferPixmap);

		r=QRect(p.x(),p.y(),widget()->width(),bt);
 		pufferPainter.fillRect(widget()->rect(),options()->color(KDecoration::ColorTitleBar, isActive()));
		if (!wndcfg->overlay.isNull())
		{
			pufferPainter.drawTiledPixmap(0,0,widget()->width(),bt,wndcfg->overlay);
		}

		if (::factory->drawcaption)
		{
			// draw title text
			pufferPainter.setFont(options()->font(isActive(), false));
		
			QColor color=options()->color(KDecoration::ColorFont, isActive());
			r=titlebar_->geometry();
			int logowidth=::factory->logo.width()+::factory->logoDistance;
			if (::factory->logoEnabled!=1 && (isActive()||!::factory->logoActive))
			{
				r.setWidth(r.width()-logowidth);
				if (::factory->logoEnabled==0)r.moveLeft(r.left()+logowidth);
			}
			QFontMetrics metrics(options()->font(isActive(), false));
			int textwidth=metrics.width(caption());
			int textalign=CrystalFactory::titleAlign();
			if (textwidth>r.width())
				textalign=Qt::AlignLeft, textwidth=r.width();			
			if (::factory->textshadow)
			{
				pufferPainter.translate(1,1);
				pufferPainter.setPen(color.dark(300));
				pufferPainter.drawText(r,textalign | Qt::AlignVCenter,caption());
				pufferPainter.translate(-1,-1);
			}
		
			pufferPainter.setPen(color);
			pufferPainter.drawText(r,
				textalign | Qt::AlignVCenter,
				caption());

			if (::factory->logoEnabled!=1 && (isActive()||!::factory->logoActive))
			{
				int x=0;
				if (::factory->logoEnabled==0 && textalign==Qt::AlignLeft)x=r.left()-logowidth;
				if (::factory->logoEnabled==2 && textalign==Qt::AlignLeft)x=r.left()+textwidth+::factory->logoDistance;

				if (::factory->logoEnabled==0 && textalign==Qt::AlignRight)x=r.right()-textwidth-logowidth;
				if (::factory->logoEnabled==2 && textalign==Qt::AlignRight)x=r.right()+::factory->logoDistance;

				if (::factory->logoEnabled==0 && textalign==Qt::AlignHCenter)x=(r.right()+r.left()-textwidth)/2-logowidth;
				if (::factory->logoEnabled==2 && textalign==Qt::AlignHCenter)x=(r.right()+r.left()+textwidth)/2+::factory->logoDistance;
				pufferPainter.drawPixmap(x,(::factory->titlesize-::factory->logo.height())/2,::factory->logo);
			}
		}else if (::factory->logoEnabled!=1 && (isActive()||!::factory->logoActive)) {
			int x=0;	
			r=titlebar_->geometry();
			if (::factory->logoEnabled==0) x=r.left();
			if (::factory->logoEnabled==2) x=r.right()-::factory->logo.width();
			pufferPainter.drawPixmap(x,(::factory->titlesize-::factory->logo.height())/2,::factory->logo);

		}

		pufferPainter.end();
		painter.drawPixmap(0,0,pufferPixmap);

		drawFrame=0;
		if (wndcfg->outlineMode && (options()->moveResizeMaximizedWindows() || isShade() || (maximizeMode() & MaximizeFull)!=MaximizeFull))
			drawFrame=1;

		if (::factory->borderwidth>0)
		{
			r=QRect(drawFrame,bt,bl-drawFrame,widget()->height()-bt-drawFrame);
			painter.fillRect(r,options()->color(KDecoration::ColorTitleBar, isActive()));
	
			r=QRect(widget()->width()-br,bt,br-drawFrame,widget()->height()-bt-drawFrame);
			painter.fillRect(r,options()->color(KDecoration::ColorTitleBar, isActive()));

			r=QRect(bl,widget()->height()-bb,widget()->width()-bl-br,bb-drawFrame);
			painter.fillRect(r,options()->color(KDecoration::ColorTitleBar, isActive()));
		}

		if (!isShade())
		{
			if (wndcfg->inlineMode==1) {
				painter.setPen(wndcfg->inlineColor);
				painter.drawRect(bl-1,bt-1,widget()->width()-bl-br+2,widget()->height()-bt-bb+2);
			}
			if (wndcfg->inlineMode==2) {
				painter.setPen(wndcfg->inlineColor.dark(150));
				painter.drawLine(bl-1,bt-1,widget()->width()-br,bt-1);
				painter.drawLine(bl-1,bt-1,bl-1,widget()->height()-bb);
				painter.setPen(wndcfg->inlineColor.light(150));
				painter.drawLine(widget()->width()-br,bt-1,widget()->width()-br,widget()->height()-bb);
				painter.drawLine(bl-1,widget()->height()-bb,widget()->width()-br-1,widget()->height()-bb);
			}
			if (wndcfg->inlineMode==3) {
				painter.setPen(wndcfg->inlineColor.light(150));
				painter.drawLine(bl-1,bt-1,widget()->width()-br,bt-1);
				painter.drawLine(bl-1,bt-1,bl-1,widget()->height()-bb);
				painter.setPen(wndcfg->inlineColor.dark(150));
				painter.drawLine(widget()->width()-br,bt-1,widget()->width()-br,widget()->height()-bb);
				painter.drawLine(bl-1,widget()->height()-bb,widget()->width()-br-1,widget()->height()-bb);
			}
		}
	}

	if (drawFrame)
	{
		// outline the frame
		QRect r=widget()->rect();
		QColor c1,c2;
		c1=c2=wndcfg->frameColor;
		if (wndcfg->outlineMode==2)c1=c1.dark(140),c2=c2.light(140);
		if (wndcfg->outlineMode==3)c1=c1.light(140),c2=c2.dark(140);

		painter.setPen(c1);
		painter.drawLine(r.left(),r.top(),r.right(),r.top());
		painter.drawLine(r.left(),r.top(),r.left(),r.bottom());

		painter.setPen(c2);
		painter.drawLine(r.right(),r.top(),r.right(),r.bottom());
		painter.drawLine(r.left(),r.bottom(),r.right(),r.bottom());

		if ((::factory->roundCorners) && !(!options()->moveResizeMaximizedWindows() && maximizeMode() & MaximizeFull))
		{
			int cornersFlag = ::factory->roundCorners;
			int r=(width());
			int b=(height());

			// Draw edge of top-left corner inside the area removed by the mask.
			if(cornersFlag & TOP_LEFT) {
				painter.setPen(c1);
				painter.drawPoint(3, 1);
				painter.drawPoint(4, 1);
				painter.drawPoint(2, 2);
				painter.drawPoint(1, 3);
				painter.drawPoint(1, 4);
			}
		
			// Draw edge of top-right corner inside the area removed by the mask.
			if(cornersFlag & TOP_RIGHT) {
				painter.setPen(c1);
				painter.drawPoint(r - 5, 1);
				painter.drawPoint(r - 4, 1);
				painter.drawPoint(r - 3, 2);
				painter.drawPoint(r - 2, 3);
				painter.drawPoint(r - 2, 4);
			}
		
			// Draw edge of bottom-left corner inside the area removed by the mask.
			if(cornersFlag & BOT_LEFT) {
				painter.setPen(c2);
				painter.drawPoint(1, b - 5);
				painter.drawPoint(1, b - 4);
				painter.drawPoint(2, b - 3);
				painter.drawPoint(3, b - 2);
				painter.drawPoint(4, b - 2);
			}
		
			// Draw edge of bottom-right corner inside the area removed by the mask.
			if(cornersFlag & BOT_RIGHT) {
				painter.setPen(c2);
				painter.drawPoint(r - 2, b - 5);
				painter.drawPoint(r - 2, b - 4);
				painter.drawPoint(r - 3, b - 3);
				painter.drawPoint(r - 4, b - 2);
				painter.drawPoint(r - 5, b - 2);
			}
		}
	}
}

void CrystalClient::resizeEvent(QResizeEvent *e)
{
	if (!widget()->isHidden()) 
	{
// 			Repaint(); /* FIXME */
	}
	if (e->size()!=e->oldSize())
	{
		updateMask();
	}
}

void CrystalClient::moveEvent(QMoveEvent *)
{
// 	Repaint(); /* FIXME */
}

void CrystalClient::showEvent(QShowEvent *)
{
	if (!widget()->isHidden()) 
		Repaint();
}

void CrystalClient::Repaint()
{
	widget()->repaint();
	for (int n=0; n<ButtonTypeCount; n++)
		if (button[n]) button[n]->reset();
}

void CrystalClient::maxButtonPressed()
{
	if (button[ButtonMax])
	{
		switch (button[ButtonMax]->lastMousePress()) 
		{
		case Qt::MidButton:
			maximize(maximizeMode() ^ MaximizeVertical);
			break;
		case Qt::RightButton:
			maximize(maximizeMode() ^ MaximizeHorizontal);
			break;
		default:
			maximize((maximizeMode() == MaximizeFull) ? MaximizeRestore: MaximizeFull);
		}
	}
}

void CrystalClient::minButtonPressed()
{
	if (button[ButtonMin]) {
		switch (button[ButtonMin]->lastMousePress()) {
		case Qt::MidButton:{
			performWindowOperation(LowerOp);
			break;
		}
		case Qt::RightButton:
			if (isShadeable()) setShade(!isShade());
			break;
		default:
			minimize();
		}
	}
}

void CrystalClient::aboveButtonPressed()
{
	setKeepAbove(!keepAbove());
}

void CrystalClient::belowButtonPressed()
{
	setKeepBelow(!keepBelow());
}

void CrystalClient::keepAboveChange(bool /*set*/)
{
	if (button[ButtonAbove])
	{
		button[ButtonAbove]->setBitmap(::factory->buttonImages[keepAbove()?ButtonImageUnAbove:ButtonImageAbove]);
	}
}

void CrystalClient::keepBelowChange(bool /*set*/)
{
	if (button[ButtonBelow])
	{
		button[ButtonBelow]->setBitmap(::factory->buttonImages[keepBelow()?ButtonImageUnBelow:ButtonImageBelow]);
	}
}

void CrystalClient::closeButtonPressed()
{
	if (button[ButtonClose])
	switch (button[ButtonClose]->lastMousePress()) {
		case Qt::RightButton:
		{
			Window client;
			char param[20];
			client = windowId();
			if (client != 0) {
				KProcess *proc = new KProcess;

				*proc << "kdocker";
				sprintf(param,"0x%lx",client);
				*proc << "-d" << "-w" << param;
				proc->start();
			} else { /* Sorry man */ }
			break;
		}
		default:
			closeWindow();
			break;
	}
}

void CrystalClient::shadeButtonPressed()
{
	if (button[ButtonShade]) {
		switch (button[ButtonShade]->lastMousePress()) {
		case Qt::MidButton:
		case Qt::RightButton:
			break;
		default:
			if (isShadeable()) setShade(!isShade());
		}
	}
}

void CrystalClient::menuButtonPressed()
{
	if (!button[ButtonMenu])return;
	
	static QTime* t = 0;
	static CrystalClient* lastClient = 0;
	if (t == 0) 
		t = new QTime;
	bool dbl = (lastClient == this && t->elapsed() <= QApplication::doubleClickInterval());
	lastClient = this;
	t->start();
	
	if (dbl)
	{	// Double Click on Symbol, close window
		closeWindow();
		return;
	}

	menuPopUp();
}

void CrystalClient::menuPopUp()
{
	KDecorationFactory* f = factory();
	showWindowMenu(QRect(button[ButtonMenu]->mapToGlobal(
	  button[ButtonMenu]->rect().topLeft()), button[ButtonMenu]->mapToGlobal(button[ButtonMenu]->rect().bottomRight())));
	if (!f->exists(this)) return; // decoration was destroyed
	button[ButtonMenu]->setDown(false);
}

#include "crystalclient.moc"
