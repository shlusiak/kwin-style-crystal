#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdeversion.h>

#include "crystalfactory.h"
#include "crystalclient.h"
#include "buttonimage.h"
#include "overlays.h"

// Button themes
#include "tiles.h"

// Logo
#include "logos.h"


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
	  case AbilityAnnounceButtons:
	  case AbilityAnnounceColors:
	  case AbilityButtonMenu:
	  case AbilityButtonOnAllDesktops:
	  case AbilityButtonSpacer:
	  case AbilityButtonHelp:
	  case AbilityButtonMinimize:
	  case AbilityButtonMaximize:
	  case AbilityButtonClose:
	  case AbilityButtonBelowOthers:
	  case AbilityButtonAboveOthers:
	  case AbilityButtonShade:
// 	  case AbilityButtonResize:

	  case AbilityColorTitleBack:
	  case AbilityColorTitleFore:	    
// 	  case AbilityColorTitleBlend:
// 	  case AbilityColorFrame:
// 	  case AbilityColorHandle:
// 	  case AbilityColorButtonBack:
// 	  case AbilityColorButtonFore:
// 	  case AbilityProvidesShadow:
 	  case AbilityUsesAlphaChannel:
#if KDE_IS_VERSION(4,6,0)
	  case AbilityUsesBlurBehind:
#endif
		return true;
		
	  default: return false;
	}
}

void setupOverlay(WND_CONFIG *cfg,int mode,QString filename, int fwidth)
{
	switch(mode)
	{
		case 0:	cfg->overlay = QPixmap();
			break;
		case 1:{
			QImage img=QImage((uchar*)lighting_overlay_data,1,60,QImage::Format_ARGB32);
			cfg->stretch_overlay = false;
			cfg->overlay = QPixmap::fromImage(img.scaled(256,::factory->titlesize));
			break;
		}
		case 2:{
			QImage img=QImage((uchar*)glass_overlay_data,20,64,QImage::Format_ARGB32);
			cfg->stretch_overlay = false;
			cfg->overlay = QPixmap::fromImage(img.scaled(256,::factory->titlesize));
			break;
		}
		case 3:{
			QImage img=QImage((uchar*)steel_overlay_data,28,64,QImage::Format_ARGB32);
			cfg->stretch_overlay = false;
			cfg->overlay = QPixmap::fromImage(img.scaled(256,::factory->titlesize));
			break;
		}
		case 4:{
			QImage img;
			if (img.load(filename))
			{
				if (fwidth == 0)
					cfg->overlay = QPixmap::fromImage(img.scaled(img.width (),::factory->titlesize));
				else
					cfg->overlay = QPixmap::fromImage(img.scaled(fwidth ,::factory->titlesize));
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

	int value = cg.readEntry("TitleAlignment", 1);
	if (value == 0) titlealign_ = Qt::AlignLeft;
	else if (value == 1) titlealign_ = Qt::AlignHCenter;
	else if (value == 2) titlealign_ = Qt::AlignRight;
	
	drawcaption=(bool)cg.readEntry("DrawCaption",true);
	textshadow=(bool)cg.readEntry("TextShadow",true);
	captiontooltip=(bool)cg.readEntry("CaptionTooltip",true);
	wheelTask=(bool)cg.readEntry("WheelTask",false);

	active.transparency=(int)cg.readEntry("ActiveTransparency", 80);
	inactive.transparency=(int)cg.readEntry("InactiveTransparency", 60);
	
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

	borderwidth=cg.readEntry("Borderwidth",6);
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
	buttontheme=cg.readEntry("ButtonTheme",9);

	{
		QString afname = cg.readEntry("OverlayFileActive","");
		QString ifname = cg.readEntry("OverlayFileInactive","");

		int aovmode = cg.readEntry("OverlayModeActive",2);
		int iovmode = cg.readEntry("OverlayModeInactive",2);

		active.stretch_overlay = cg.readEntry("OverlayStretchActive",false);
		inactive.stretch_overlay = cg.readEntry("OverlayStretchInactive",false);

		bool fwidth_active = cg.readEntry("OverlayFWidthActive",true);
		bool fwidth_inactive = cg.readEntry("OverlayFWidthInactive",true);

		int fwvalue_active = cg.readEntry("OverlayFWValueActive",256);
		int fwvalue_inactive = cg.readEntry("OverlayFWValueInactive",256);
		
		if (fwidth_active == false) fwvalue_active = 0;
		if (fwidth_inactive == false) fwvalue_inactive = 0;

		setupOverlay(&active,aovmode,afname,fwvalue_active);
		setupOverlay(&inactive,iovmode,ifname,fwvalue_inactive);
	}


	logoEnabled=cg.readEntry("LogoAlignment",1);
	logoStretch=cg.readEntry("LogoStretch",0);
	logoActive=cg.readEntry("LogoActive",false);
	logoDistance=cg.readEntry("LogoDistance",0);
	
	int logoIndex=cg.readEntry("LogoIndex", 0);
	QString filename=cg.readEntry("LogoFile","");

	if (logoEnabled!=1)
	{
		if (logoIndex == 0)
		{
			if (!filename.isNull() && logo.load(filename))
			{
				
			}else logoEnabled=1;
		} else {
			QImage img;
			
			switch (logoIndex)
			{
				default:
				case 1:
					img=QImage((uchar*)kde_data,26,26,QImage::Format_ARGB32);
					break;
				case 2:
					img=QImage((uchar*)tux_data,36,26,QImage::Format_ARGB32);
					break;
				case 3:
					img=QImage((uchar*)gentoo_data,64,67,QImage::Format_ARGB32);
					break;
				case 4:
					img=QImage((uchar*)kubuntu_data,24,26,QImage::Format_ARGB32);
					break;
				case 5:
					img=QImage((uchar*)ubuntu_data,64,64,QImage::Format_ARGB32);
					break;
				case 6:
					img=QImage((uchar*)opensuse_data,32,26,QImage::Format_ARGB32);
					break;
				case 7:
					img=QImage((uchar*)pclinuxos_data,26,26,QImage::Format_ARGB32);
					break;
			}
			logo = QPixmap::fromImage(img);
		}
		
		if ((logoEnabled != 1) && (logoStretch==0))
		{
			logo=logo.scaled(((titlesize-2)*logo.width())/logo.height(),titlesize-2);
		}
	}
	else logo = QPixmap ();
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
// 		buttonImages[ButtonImageMenu]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageHelp]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageMax]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageRestore]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageMin]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageClose]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageSticky]->SetNormal(aqua_sticky_data,16,16);
		buttonImages[ButtonImageUnSticky]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageShade]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageUnShade]->SetNormal(aqua_default_hovered_data,16,16);
	
		buttonImages[ButtonImageAbove]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageUnAbove]->SetNormal(aqua_default_hovered_data,16,16);
		buttonImages[ButtonImageBelow]->SetNormal(aqua_default_data,16,16);
		buttonImages[ButtonImageUnBelow]->SetNormal(aqua_default_hovered_data,16,16);
		
		buttonImages[ButtonImageClose]->SetHovered(aqua_close_hovered_data);
		buttonImages[ButtonImageMax]->SetHovered(aqua_max_hovered_data);
		buttonImages[ButtonImageMin]->SetHovered(aqua_min_hovered_data);
		buttonImages[ButtonImageRestore]->SetHovered(aqua_max_hovered_data);
		buttonImages[ButtonImageUnSticky]->SetHovered(aqua_un_sticky_hovered_data);
		buttonImages[ButtonImageHelp]->SetHovered(aqua_help_hovered_data);
// 		buttonImages[ButtonImageAbove]->SetHovered(aqua_above_hovered_data);
// 		buttonImages[ButtonImageBelow]->SetHovered(aqua_below_hovered_data);
// 		buttonImages[ButtonImageShade]->SetHovered(aqua_shade_hovered_data);
// 		buttonImages[ButtonImageUnShade]->SetHovered(aqua_shade_hovered_data);
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
	case 9: // Kubuntu-hardy
		buttonImages[ButtonImageMenu]->SetNormal(hardy_menu_data,28,17);
		buttonImages[ButtonImageMenu]->SetHovered(hardy_menu_hovered_data);
		buttonImages[ButtonImageMenu]->SetPressed(hardy_menu_pressed_data);

		buttonImages[ButtonImageHelp]->SetNormal(hardy_help_data,28,17);
		buttonImages[ButtonImageHelp]->SetHovered(hardy_help_hovered_data);
		buttonImages[ButtonImageHelp]->SetPressed(hardy_help_pressed_data);

		buttonImages[ButtonImageMax]->SetNormal(hardy_max_data,28,17);
		buttonImages[ButtonImageMax]->SetHovered(hardy_max_hovered_data);
		buttonImages[ButtonImageMax]->SetPressed(hardy_max_pressed_data);
		buttonImages[ButtonImageRestore]->SetNormal(hardy_restore_data,28,17);
		buttonImages[ButtonImageRestore]->SetHovered(hardy_restore_hovered_data);
		buttonImages[ButtonImageRestore]->SetPressed(hardy_restore_pressed_data);
		buttonImages[ButtonImageMin]->SetNormal(hardy_min_data,28,17);
		buttonImages[ButtonImageMin]->SetHovered(hardy_min_hovered_data);
		buttonImages[ButtonImageMin]->SetPressed(hardy_min_pressed_data);
		buttonImages[ButtonImageClose]->SetNormal(hardy_close_data,28,17);
		buttonImages[ButtonImageClose]->SetHovered(hardy_close_hovered_data);
		buttonImages[ButtonImageClose]->SetPressed(hardy_close_pressed_data);

		buttonImages[ButtonImageSticky]->SetNormal(hardy_sticky_data,28,17);
		buttonImages[ButtonImageSticky]->SetHovered(hardy_sticky_hovered_data);
		buttonImages[ButtonImageSticky]->SetPressed(hardy_sticky_pressed_data);
		buttonImages[ButtonImageUnSticky]->SetNormal(hardy_un_sticky_data,28,17);
		buttonImages[ButtonImageUnSticky]->SetHovered(hardy_un_sticky_hovered_data);
		buttonImages[ButtonImageUnSticky]->SetPressed(hardy_un_sticky_pressed_data);

		buttonImages[ButtonImageAbove]->SetNormal(hardy_above_data,28,17);
		buttonImages[ButtonImageAbove]->SetHovered(hardy_above_hovered_data);
		buttonImages[ButtonImageAbove]->SetPressed(hardy_above_pressed_data);
		buttonImages[ButtonImageUnAbove]->SetNormal(hardy_un_above_data,28,17);
		buttonImages[ButtonImageUnAbove]->SetHovered(hardy_un_above_hovered_data);
		buttonImages[ButtonImageUnAbove]->SetPressed(hardy_un_above_pressed_data);


		buttonImages[ButtonImageBelow]->SetNormal(hardy_below_data,28,17);
		buttonImages[ButtonImageBelow]->SetHovered(hardy_below_hovered_data);
		buttonImages[ButtonImageBelow]->SetPressed(hardy_below_pressed_data);

		buttonImages[ButtonImageUnBelow]->SetNormal(hardy_un_below_data,28,17);
		buttonImages[ButtonImageUnBelow]->SetHovered(hardy_un_below_hovered_data);
		buttonImages[ButtonImageUnBelow]->SetPressed(hardy_un_below_pressed_data);

		buttonImages[ButtonImageShade]->SetNormal(hardy_shade_data,28,17);
		buttonImages[ButtonImageShade]->SetHovered(hardy_shade_hovered_data);
		buttonImages[ButtonImageShade]->SetPressed(hardy_shade_pressed_data);
		buttonImages[ButtonImageUnShade]->SetNormal(hardy_un_shade_data,28,17);
		buttonImages[ButtonImageUnShade]->SetHovered(hardy_un_shade_hovered_data);
		buttonImages[ButtonImageUnShade]->SetPressed(hardy_un_shade_pressed_data);

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
	case 10: // Glass Pill
		buttonImages[ButtonImageMenu]->SetNormal(gp_menu_data,44,22);
		buttonImages[ButtonImageMenu]->SetHovered(gp_menu_hover_data);

		buttonImages[ButtonImageHelp]->SetNormal(gp_help_data,44,22);
		buttonImages[ButtonImageHelp]->SetHovered(gp_help_hover_data);

		buttonImages[ButtonImageMax]->SetNormal(gp_maximize_data,44,22);
		buttonImages[ButtonImageMax]->SetHovered(gp_maximize_hover_data);
		buttonImages[ButtonImageRestore]->SetNormal(gp_restore_data,44,22);
		buttonImages[ButtonImageRestore]->SetHovered(gp_restore_hover_data);
		buttonImages[ButtonImageMin]->SetNormal(gp_minimize_data,44,22);
		buttonImages[ButtonImageMin]->SetHovered(gp_minimize_hover_data);
		buttonImages[ButtonImageClose]->SetNormal(gp_close_data,44,22);
		buttonImages[ButtonImageClose]->SetHovered(gp_close_hover_data);

		buttonImages[ButtonImageSticky]->SetNormal(gp_sticky_data,44,22);
		buttonImages[ButtonImageSticky]->SetHovered(gp_sticky_hover_data);
		buttonImages[ButtonImageUnSticky]->SetNormal(gp_unsticky_data,44,22);
		buttonImages[ButtonImageUnSticky]->SetHovered(gp_unsticky_hover_data);

		buttonImages[ButtonImageAbove]->SetNormal(gp_above_data,44,22);
		buttonImages[ButtonImageAbove]->SetHovered(gp_above_hover_data);
		buttonImages[ButtonImageUnAbove]->SetNormal(gp_unabove_data,44,22);
		buttonImages[ButtonImageUnAbove]->SetHovered(gp_unabove_hover_data);


		buttonImages[ButtonImageBelow]->SetNormal(gp_below_data,44,22);
		buttonImages[ButtonImageBelow]->SetHovered(gp_below_hover_data);
		buttonImages[ButtonImageUnBelow]->SetNormal(gp_unbelow_data,44,22);
		buttonImages[ButtonImageUnBelow]->SetHovered(gp_unbelow_hover_data);

		buttonImages[ButtonImageShade]->SetNormal(gp_shade_data,44,22);
		buttonImages[ButtonImageShade]->SetHovered(gp_shade_hover_data);
		buttonImages[ButtonImageUnShade]->SetNormal(gp_unshade_data,44,22);
		buttonImages[ButtonImageUnShade]->SetHovered(gp_unshade_hover_data);

		for (int i=0;i<ButtonImageCount;i++)
		{
			buttonImages[i]->setSpace(0,0);
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



