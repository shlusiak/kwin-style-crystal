/***************************************************************************
 *   Copyright (C) 2006 by Sascha Hlusiak                                  *
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

#include "config.h"
#include <kconfig.h>
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
#include <qpopupmenu.h>
#include <kwin.h>
#include <kprocess.h>

#include "crystalclient.h"
#include "crystalbutton.h"
#include "buttonimage.h"
#include "imageholder.h"
#include "overlays.h"

// Button themes
#include "tiles.h"


CrystalFactory* factory=NULL;

bool CrystalFactory::initialized_              = false;
Qt::AlignmentFlags CrystalFactory::titlealign_ = Qt::AlignHCenter;


extern "C" KDecorationFactory* create_factory()
{
	return new CrystalFactory();
}


/*****************
 * Tooltip class for the titlebar
 **/
class CCrystalTooltip:public QToolTip
{
private:
	CrystalClient *client;
public:
	CCrystalTooltip(QWidget *widget,CrystalClient *vc):QToolTip(widget),client(vc) {}
	virtual void maybeTip(const QPoint& p)
	{
		if (client->titlebar_->geometry().contains(p))
		{
			tip(client->titlebar_->geometry(),client->caption());
		}
	}
};




CrystalFactory::CrystalFactory()
{
	for (int i=0;i<ButtonImageCount;i++)
		buttonImages[i]=NULL;

	::factory=this;
	readConfig();
	initialized_ = true;

	if (transparency)image_holder=new QImageHolder(active.userdefinedPicture,inactive.userdefinedPicture);
		else image_holder=NULL;
	CreateButtonImages();
}

CrystalFactory::~CrystalFactory() 
{
	initialized_ = false; 
	::factory=NULL;
	if (image_holder)delete image_holder;
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

	if (transparency)
	{
		if (!image_holder)image_holder=new QImageHolder(active.userdefinedPicture,inactive.userdefinedPicture);
		image_holder->setUserdefinedPictures(active.userdefinedPicture,inactive.userdefinedPicture);
		image_holder->repaint(true);
	}else{
		if (image_holder)delete image_holder;
		image_holder=NULL;
	}
	CreateButtonImages();
	
	return true;
}

bool CrystalFactory::supports(Ability ability)
{
	switch (ability)
	{
#if KDE_IS_VERSION(3,4,0)
	case AbilityButtonResize: return false;
#endif
	default:
		return true;
	}
}

void setupOverlay(WND_CONFIG *cfg,int mode,QString filename)
{
	cfg->overlay.resize(0,0);
	switch(mode)
	{
		case 0:	break;
		case 1:{
			cfg->overlay.resize(0,0);
			QImage img=QImage((uchar*)lighting_overlay_data,1,60,32,NULL,0,QImage::LittleEndian);
			img.setAlphaBuffer(true);
			cfg->overlay.convertFromImage(img.smoothScale(256,::factory->titlesize));
			break;
		}
		case 2:{
			cfg->overlay.resize(0,0);
			QImage img=QImage((uchar*)glass_overlay_data,20,64,32,NULL,0,QImage::LittleEndian);
			img.setAlphaBuffer(true);
			cfg->overlay.convertFromImage(img.smoothScale(256,::factory->titlesize));
			break;
		}
		case 3:{
			cfg->overlay.resize(0,0);
			QImage img=QImage((uchar*)steel_overlay_data,28,64,32,NULL,0,QImage::LittleEndian);
			img.setAlphaBuffer(true);
			cfg->overlay.convertFromImage(img.smoothScale(256,::factory->titlesize));
			break;
		}
		case 4:{
			QImage img;
			if (img.load(filename))
			{
				img.setAlphaBuffer(true);
				cfg->overlay.convertFromImage(img.smoothScale(256,::factory->titlesize));
			}
			break;
		}
	}
}

bool CrystalFactory::readConfig()
{
    // create a config object
	KConfig config("kwincrystalrc");
	config.setGroup("General");
	QColor c;

	QString value = config.readEntry("TitleAlignment", "AlignHCenter");
	if (value == "AlignLeft") titlealign_ = Qt::AlignLeft;
	else if (value == "AlignHCenter") titlealign_ = Qt::AlignHCenter;
	else if (value == "AlignRight") titlealign_ = Qt::AlignRight;
	
	drawcaption=(bool)config.readBoolEntry("DrawCaption",true);
	textshadow=(bool)config.readBoolEntry("TextShadow",true);
	captiontooltip=(bool)config.readBoolEntry("CaptionTooltip",true);
	wheelTask=(bool)config.readBoolEntry("WheelTask",false);
	transparency=(bool)config.readBoolEntry("EnableTransparency",true);
	trackdesktop=(bool)config.readBoolEntry("TrackDesktop",false);

	active.mode=config.readNumEntry("ActiveMode",0);
	inactive.mode=config.readNumEntry("InactiveMode",1);
	active.amount=(double)config.readNumEntry("ActiveShade",30)/100.0;
	inactive.amount=(double)config.readNumEntry("InactiveShade",-30)/100.0;
	active.outlineMode=(int)config.readNumEntry("ActiveFrame",1);
	inactive.outlineMode=(int)config.readNumEntry("InactiveFrame",1);
	c=QColor(160,160,160);
	active.frameColor=config.readColorEntry("FrameColor1",&c);
	c=QColor(128,128,128);
	inactive.frameColor=config.readColorEntry("FrameColor2",&c);

	active.inlineMode=(int)config.readNumEntry("ActiveInline",0);
	inactive.inlineMode=(int)config.readNumEntry("InactiveInline",0);
	c=QColor(160,160,160);
	active.inlineColor=config.readColorEntry("InlineColor1",&c);
	c=QColor(160,160,160);
	inactive.inlineColor=config.readColorEntry("InlineColor2",&c);

	active.blur=config.readNumEntry("ActiveBlur",0);
	inactive.blur=config.readNumEntry("InactiveBlur",0);

	active.userdefinedPicture=QImage();
	inactive.userdefinedPicture=QImage();
	if ((bool)config.readBoolEntry("ActiveUserdefined",false))
	{
		active.userdefinedPicture.load(config.readEntry("ActiveUserdefinedPicture"));
	}
	if ((bool)config.readBoolEntry("InactiveUserdefined",false))
	{
		inactive.userdefinedPicture.load(config.readEntry("InactiveUserdefinedPicture"));
	}

	borderwidth=config.readNumEntry("Borderwidth",5);
	titlesize=config.readNumEntry("Titlebarheight",21);
 
	buttonColor_normal=QColor(255,255,255);
	buttonColor_normal=config.readColorEntry("ButtonColor",&buttonColor_normal);
	buttonColor_hovered=config.readColorEntry("ButtonColor2",&buttonColor_normal);
	buttonColor_pressed=config.readColorEntry("ButtonColor3",&buttonColor_normal);
	minColor_normal=QColor(255,255,255);
	minColor_normal=config.readColorEntry("MinColor",&buttonColor_normal);
	minColor_hovered=config.readColorEntry("MinColor2",&buttonColor_normal);
	minColor_pressed=config.readColorEntry("MinColor3",&buttonColor_normal);
	maxColor_normal=QColor(255,255,255);
	maxColor_normal=config.readColorEntry("MaxColor",&buttonColor_normal);
	maxColor_hovered=config.readColorEntry("MaxColor2",&buttonColor_normal);
	maxColor_pressed=config.readColorEntry("MaxColor3",&buttonColor_normal);
	closeColor_normal=QColor(255,255,255);
	closeColor_normal=config.readColorEntry("CloseColor",&closeColor_normal);
	closeColor_hovered=config.readColorEntry("CloseColor2",&closeColor_normal);
	closeColor_pressed=config.readColorEntry("CloseColor3",&closeColor_normal);

	roundCorners=config.readNumEntry("RoundCorners",TOP_LEFT & TOP_RIGHT);

	hovereffect=config.readBoolEntry("HoverEffect",true);
	animateHover=config.readBoolEntry("AnimateHover",true);
	tintButtons=config.readBoolEntry("TintButtons",false);
	menuImage=config.readBoolEntry("MenuImage",true);
	repaintMode=config.readNumEntry("RepaintMode",1);
	repaintTime=config.readNumEntry("RepaintTime",200);
	buttontheme=config.readNumEntry("ButtonTheme",8);


	setupOverlay(&active,config.readNumEntry("OverlayModeActive",0),config.readEntry("OverlayFileActive",""));
	setupOverlay(&inactive,config.readNumEntry("OverlayModeInactive",0),config.readEntry("OverlayFileInactive",""));

	logoEnabled=config.readNumEntry("LogoAlignment",1);
	logoStretch=config.readNumEntry("LogoStretch",0);
	logoActive=config.readBoolEntry("LogoActive",0);
	logoDistance=config.readNumEntry("LogoDistance",0);
	QString filename=config.readEntry("LogoFile","");
	if (!filename.isNull() && logoEnabled!=1)
	{
		if (logo.load(filename))
		{
			if (logoStretch==0)
			{
				logo=logo.convertToImage().smoothScale((titlesize*logo.width())/logo.height(),titlesize);
			}
		}else logoEnabled=1;
	}else logo.resize(0,0);
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
	::factory->clients.remove(this);
	for (int n=0; n<ButtonTypeCount; n++) {
		if (button[n]) delete button[n];
	}
}

void CrystalClient::init()
{
	createMainWidget(WResizeNoErase | WRepaintNoErase);
	widget()->installEventFilter(this);

	FullMax=false;
	if (!options()->moveResizeMaximizedWindows())
		FullMax=(maximizeMode()==MaximizeFull);
	
	// for flicker-free redraws
	widget()->setBackgroundMode(NoBackground);

	// setup layout
	mainlayout = new QGridLayout(widget(), 4, 3); // 4x3 grid
	titlelayout = new QHBoxLayout();
	titlebar_ = new QSpacerItem(1, ::factory->titlesize-1, QSizePolicy::Expanding,
					QSizePolicy::Fixed);

	mainlayout->setResizeMode(QLayout::FreeResize);
	mainlayout->setRowSpacing(0, (::factory->buttontheme==5)?0:1);
	mainlayout->setRowSpacing(3, ::factory->borderwidth*1);

	mainlayout->setColSpacing(2,borderSpacing());
	mainlayout->setColSpacing(0,borderSpacing());
	mainlayout->addLayout(titlelayout, 1, 1);

	if (isPreview()) {
		char c[512];
		sprintf(c,"<center><b>Crystal %s Preview</b><br>Built: %s</center>",VERSION,__DATE__);
		mainlayout->addItem(new QSpacerItem(1, 1,QSizePolicy::Expanding,QSizePolicy::Fixed), 0, 1);
		mainlayout->addItem(new QSpacerItem(1, ::factory->borderwidth,QSizePolicy::Expanding,QSizePolicy::Expanding), 3, 1);
		mainlayout->addWidget(new QLabel(i18n(c),widget()), 2, 1);
	} else {
		mainlayout->addItem(new QSpacerItem(0, 0), 2, 1);
	}
	
	mainlayout->setRowStretch(2, 10);
	mainlayout->setColStretch(1, 10);

	updateMask();

	for (int n=0; n<ButtonTypeCount; n++) button[n] = 0;
	addButtons(titlelayout, options()->titleButtonsLeft());
	titlelayout->addItem(titlebar_);
	{
		CrystalButton* lastbutton=addButtons(titlelayout, options()->titleButtonsRight());
		if (lastbutton)lastbutton->setFirstLast(false,true);
	}

	if (::factory->captiontooltip) new CCrystalTooltip(widget(),this);	

	connect( this, SIGNAL( keepAboveChanged( bool )), SLOT( keepAboveChange( bool )));
	connect( this, SIGNAL( keepBelowChanged( bool )), SLOT( keepBelowChange( bool )));
	
	if (::factory->transparency)connect ( ::factory->image_holder,SIGNAL(repaintNeeded()),this,SLOT(Repaint()));
	if (::factory->transparency)connect ( &timer,SIGNAL(timeout()),this,SLOT(Repaint()));

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
		for (unsigned n=0; n < s.length(); n++)
		{
			CrystalButton *current=NULL;
			switch (s[n]) {
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
				if (layout->findWidget(current)==0)current->setFirstLast(true,false);
			}
			lastone=current;
		}
	}
	return lastone;
}

void CrystalClient::activeChange()
{
	Repaint();
	if (isActive()) ::factory->clients.at(::factory->clients.find(this));
}

void CrystalClient::captionChange()
{
	if (::factory->drawcaption) widget()->repaint(titlebar_->geometry(), false);
}

void CrystalClient::desktopChange()
{
	bool d = isOnAllDesktops();
	if (button[ButtonSticky]) {
		button[ButtonSticky]->setBitmap(::factory->buttonImages[d ? ButtonImageSticky : ButtonImageUnSticky ]);
		QToolTip::remove(button[ButtonSticky]);
		QToolTip::add(button[ButtonSticky], d ? i18n("Not on all desktops") : i18n("On All Desktops"));
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
		QToolTip::remove(button[ButtonMax]);
		QToolTip::add(button[ButtonMax], m ? i18n("Restore") : i18n("Maximize"));
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
		mainlayout->setColSpacing(0,0);
		mainlayout->setColSpacing(2,0);
	}else{
		mainlayout->setColSpacing(2,borderSpacing());
		mainlayout->setColSpacing(0,borderSpacing());
	}
	
	mainlayout->setRowSpacing(0, (FullMax||::factory->buttontheme==5)?0:1);
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
	if (!::factory->transparency)Repaint();
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
	if (obj != widget()) return false;
	
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

void CrystalClient::ClientWindows(Window* v_frame,Window* v_wrapper,Window *v_client)
{
	Window root=0,frame=0,wrapper=0,client=0,parent=0,*children=NULL;
	uint numc;
	if (v_frame) *v_frame=0;
	if (v_wrapper) *v_wrapper=0;
	if (v_client) *v_client=0;
	// Our Deco is the child of a frame, get our parent
	if (XQueryTree(qt_xdisplay(),widget()->winId(),&root,&frame,&children,&numc) == 0)
		return;
	if (children!=NULL)XFree(children);
	children=NULL;
	
	// frame has two children, us and a wrapper, get the wrapper
	if (XQueryTree(qt_xdisplay(),frame,&root,&parent,&children,&numc)==0)
		return;

	for (uint i=0;i<numc;i++)
	{
		if (children[i]!=widget()->winId())wrapper=children[i];
	}
	if (children!=NULL)XFree(children);
	children=NULL;
	
	// wrapper has only one child, which is the client. We want this!!
	if (XQueryTree(qt_xdisplay(),wrapper,&root,&parent,&children,&numc)==0)
		return;
	if (numc==1)client=children[0];	
	if (children!=NULL)XFree(children);
	children=NULL;
	if (v_client) *v_client=client;
	if (v_wrapper) *v_wrapper=wrapper;
	if (v_frame) *v_frame=frame;
}

void CrystalClient::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (/*(titlebar_->geometry().contains(e->pos()))&&*/(e->button()==LeftButton)) titlebarDblClickOperation();
	else {
		QMouseEvent me(QEvent::MouseButtonPress,e->pos(),e->button(),e->state());
		processMousePressEvent(&me);
	}
}

void CrystalClient::mouseWheelEvent(QWheelEvent *e)
{
	if (::factory->wheelTask)
	{
		QPtrList <CrystalClient> *l=&(::factory->clients);
		
		if (l->current()==NULL) for (unsigned int i=0;i<l->count();i++) if ((l->at(i))->isActive()) break;
		
		CrystalClient *n=this;
		Window client,frame,wrapper;
		do
		{
			if(e->delta()>0)
			{
				n=l->next();
				if (n==NULL)n=l->first();
			}else{
				n=l->prev();
				if (n==NULL)n=l->last();
			}
			
			n->ClientWindows(&frame,&wrapper,&client);
			if (client == 0) { /* FALLBACK */
#if KDE_IS_VERSION(3,5,0)
				titlebarMouseWheelOperation(e->delta());
#endif
				return;
			}
			KWin::WindowInfo info=KWin::windowInfo(client);
			if ((n->desktop()==desktop()) && !info.isMinimized())break;
		}while(n!=this);
			
		KWin::activateWindow(client);
	}else{
#if KDE_IS_VERSION(3,5,0)
		titlebarMouseWheelOperation(e->delta());
#endif
	}
}

void CrystalClient::paintEvent(QPaintEvent*)
{
	if (!CrystalFactory::initialized()) return;

	QColorGroup group;
	QPainter painter(widget());

	// draw the titlebar
	group = options()->colorGroup(KDecoration::ColorTitleBar, isActive());
	WND_CONFIG* wndcfg=(isActive()?&::factory->active:&::factory->inactive);

	if (::factory->transparency && ::factory->trackdesktop)
		::factory->image_holder->repaint(false); // If other desktop than the last, regrab the root image
	QPixmap *background=::factory->transparency?::factory->image_holder->image(isActive()):NULL;
	int drawFrame;

	{
		QRect r;
		QPoint p=widget()->mapToGlobal(QPoint(0,0));
		int bl,br,bt,bb;
		borders(bl,br,bt,bb);
	
		QPixmap pufferPixmap;
		pufferPixmap.resize(widget()->width(), bt);
		QPainter pufferPainter(&pufferPixmap);

		r=QRect(p.x(),p.y(),widget()->width(),bt);
 		if (background && !background->isNull())pufferPainter.drawPixmap(QPoint(0,0),*background,r);
 		else 
		{
			pufferPainter.fillRect(widget()->rect(),group.background());
		}
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
			r.moveBy(0,-1);
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
				textalign=AlignLeft, textwidth=r.width();			
			if (::factory->textshadow)
			{
				pufferPainter.translate(1,1);
				pufferPainter.setPen(color.dark(300));
				pufferPainter.drawText(r,textalign | AlignVCenter,caption());
				pufferPainter.translate(-1,-1);
			}
		
			pufferPainter.setPen(color);
			pufferPainter.drawText(r,
				textalign | AlignVCenter,
				caption());

			if (::factory->logoEnabled!=1 && (isActive()||!::factory->logoActive))
			{
				int x=0;
				if (::factory->logoEnabled==0 && textalign==AlignLeft)x=r.left()-logowidth;
				if (::factory->logoEnabled==2 && textalign==AlignLeft)x=r.left()+textwidth+::factory->logoDistance;

				if (::factory->logoEnabled==0 && textalign==AlignRight)x=r.right()-textwidth-logowidth;
				if (::factory->logoEnabled==2 && textalign==AlignRight)x=r.right()+::factory->logoDistance;

				if (::factory->logoEnabled==0 && textalign==AlignHCenter)x=(r.right()+r.left()-textwidth)/2-logowidth;
				if (::factory->logoEnabled==2 && textalign==AlignHCenter)x=(r.right()+r.left()+textwidth)/2+::factory->logoDistance;
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
			if (background && !background->isNull())
			{	// Draw the side and bottom of the window with transparency
				// Left
				r=QRect(p.x()+drawFrame,p.y()+bt,bl-drawFrame,widget()->height()-bt-drawFrame);
				painter.drawPixmap(QPoint(drawFrame,bt),*background,r);

				// Right
				r=QRect(widget()->width()-br+p.x(),p.y()+bt,br-drawFrame,widget()->height()-bt-drawFrame);
				painter.drawPixmap(QPoint(widget()->width()-br,bt),*background,r);

				// Bottom
				r=QRect(p.x()+bl,p.y()+widget()->height()-bb,widget()->width()-bl-br,bb-drawFrame);
				painter.drawPixmap(QPoint(bl,widget()->height()-bb),*background,r);
			}else{
				r=QRect(drawFrame,bt,bl-drawFrame,widget()->height()-bt-drawFrame);
				painter.fillRect(r,group.background());
		
				r=QRect(widget()->width()-br,bt,br-drawFrame,widget()->height()-bt-drawFrame);
				painter.fillRect(r,group.background());

				r=QRect(bl,widget()->height()-bb,widget()->width()-bl-br,bb-drawFrame);
				painter.fillRect(r,group.background());
			}
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
	if (background==NULL && ::factory->transparency)
	{	// We don't have a background image, draw a solid rectangle
		// And notify image_holder that we need an update asap
		if (::factory)if (::factory->image_holder)
		// UnInit image_holder, on next Repaint it will be Init'ed again.
		QTimer::singleShot(500,::factory->image_holder,SLOT(CheckSanity()));
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
	if (widget()->isShown()) 
	{
		if (!::factory->transparency) ;
		else if (::factory->repaintMode==1)
		{
			if (!timer.isActive())timer.start(0,true);
// 			Repaint();
		}
		// repaint only every xxx ms
		else if (::factory->repaintMode==3 || !timer.isActive())
		{
			// Repaint only, when mode!=fade || amount<100
			WND_CONFIG* wnd=isActive()?&::factory->active:&::factory->inactive;
			if (wnd->mode!=0 || wnd->amount<100)
				timer.start(::factory->repaintTime,true);	
		}
	}
	if (e->size()!=e->oldSize())
	{
		updateMask();
	}
}

void CrystalClient::moveEvent(QMoveEvent *)
{
	if (widget()->isShown())
	{
		if (!::factory->transparency) return;
		else if (::factory->repaintMode==1)
		{
			Repaint();
		}
		// repaint every xxx ms, so constant moving does not take too much CPU
		else if (::factory->repaintMode==3 || !timer.isActive())
		{
			// Repaint only, when mode!=fade || value<100, because otherwise it is a plain color
			WND_CONFIG* wnd=isActive()?&::factory->active:&::factory->inactive;
			if (wnd->mode!=0 || wnd->amount<100)
				timer.start(::factory->repaintTime,true);
		}
	}
}

void CrystalClient::showEvent(QShowEvent *)
{
	if (widget()->isShown()) 
		Repaint();
}

void CrystalClient::Repaint()
{
	widget()->repaint(false);
	for (int n=0; n<ButtonTypeCount; n++)
		if (button[n]) button[n]->reset();
}

void CrystalClient::maxButtonPressed()
{
	if (button[ButtonMax])
	{
		switch (button[ButtonMax]->lastMousePress()) 
		{
		case MidButton:
			maximize(maximizeMode() ^ MaximizeVertical);
			break;
		case RightButton:
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
		case MidButton:{
			performWindowOperation(LowerOp);
			break;
		}
		case RightButton:
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
		case RightButton:
		{
			Window frame,wrapper,client;
			char param[20];
			ClientWindows(&frame,&wrapper,&client);
			if (client != 0) {
				KProcess *proc = new KProcess;

				*proc << "kdocker";
				sprintf(param,"0x%lx",client);
				*proc << "-d" << "-w" << param;
				proc->start(KProcess::DontCare);
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
		case MidButton:
		case RightButton:
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
	QPoint p(button[ButtonMenu]->rect().bottomLeft().x(),
                 button[ButtonMenu]->rect().bottomLeft().y()+2);
	KDecorationFactory* f = factory();
	showWindowMenu(button[ButtonMenu]->mapToGlobal(p));
	if (!f->exists(this)) return; // decoration was destroyed
	button[ButtonMenu]->setDown(false);
}

#include "crystalclient.moc"
