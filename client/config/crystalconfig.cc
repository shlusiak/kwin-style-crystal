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

#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qwhatsthis.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qwidgetstack.h>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <kcolorbutton.h>
#include <kfiledialog.h>
#include <qpicture.h>
#include <kapplication.h>
#include <kurlrequester.h>

#include "configdialog.h"
#include "infodialog.h"
#include "crystalconfig.h"


CrystalConfig::CrystalConfig(KConfig*, QWidget* parent)
    : QObject(parent), config_(0), dialog_(0)
{
	config_ = new KConfig("kwincrystalrc");
	
	dialog_ = new ConfigDialog(parent);
	dialog_->show();
		
	connect(dialog_->titlealign, SIGNAL(clicked(int)),this, SLOT(selectionChanged(int)));
	
	connect(dialog_->drawCaption, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->textshadow, SIGNAL(stateChanged(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->tooltip,SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->wheelTask,SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
	
	connect(dialog_->trackdesktop, SIGNAL(stateChanged(int)),this, SLOT(selectionChanged(int)));
	
	connect(dialog_->shade1, SIGNAL(valueChanged(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->shade2, SIGNAL(valueChanged(int)),this, SLOT(selectionChanged(int)));
	
	connect(dialog_->frame1, SIGNAL(activated(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->frame2, SIGNAL(activated(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->frameColor1, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->frameColor2, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	
	connect(dialog_->inline1, SIGNAL(activated(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->inline2, SIGNAL(activated(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->inlineColor1, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->inlineColor2, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
		
	connect(dialog_->type1,SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->type2,SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));
	
	connect(dialog_->enableTransparency,SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
	
	connect(dialog_->borderwidth, SIGNAL(valueChanged(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->titlebarheight, SIGNAL(valueChanged(int)),this, SLOT(selectionChanged(int)));
	
	connect(dialog_->tlc, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->trc, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->blc, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->brc, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->buttonColor1, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->buttonColor2, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->buttonColor3, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->minColor1, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->minColor2, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->minColor3, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->maxColor1, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->maxColor2, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->maxColor3, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->closeColor1, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->closeColor2, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->closeColor3, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	
	connect(dialog_->hover, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->animateHover, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->buttonTheme, SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->tintButtons, SIGNAL(toggled(bool)),this,SLOT(boolChanged(bool)));
	
	connect(dialog_->repaintMode, SIGNAL(clicked(int)),this, SLOT(selectionChanged(int)));
	
	connect(dialog_->updateTime, SIGNAL(valueChanged(int)),this, SLOT(selectionChanged(int)));
		
	connect(dialog_->infoButton, SIGNAL(clicked(void)),this,SLOT(infoDialog(void)));

	connect(dialog_->active_blur, SIGNAL(valueChanged(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->inactive_blur, SIGNAL(valueChanged(int)),this,SLOT(selectionChanged(int)));

	connect(dialog_->userPicture1, SIGNAL(stateChanged(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->userPicture2, SIGNAL(stateChanged(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->activeFile,SIGNAL(textChanged(const QString&)),this,SLOT(textChanged( const QString& )));
	connect(dialog_->inactiveFile,SIGNAL(textChanged(const QString&)),this,SLOT(textChanged( const QString& )));


	connect(dialog_->overlay_active, SIGNAL(activated(int)),this, SLOT(overlay_active_changed(int)));
	connect(dialog_->overlay_inactive, SIGNAL(activated(int)),this, SLOT(overlay_inactive_changed(int)));

	connect(dialog_->overlay_active_file,SIGNAL(textChanged(const QString&)),this,SLOT(textChanged(const QString &)));
	connect(dialog_->overlay_inactive_file,SIGNAL(textChanged(const QString&)),this,SLOT(textChanged(const QString &)));


	connect(dialog_->logoEnabled, SIGNAL(clicked(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->logoFile, SIGNAL(textChanged(const QString &)),this, SLOT(logoTextChanged(const QString&)));
	connect(dialog_->logoStretch, SIGNAL(activated(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->logoActive, SIGNAL(stateChanged(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->logoDistance,SIGNAL(valueChanged(int)),this,SLOT(selectionChanged(int)));

	load(config_);
}

CrystalConfig::~CrystalConfig()
{
	if (dialog_) delete dialog_;
	if (config_) delete config_;
}

void CrystalConfig::selectionChanged(int)
{
	emit changed();
}

void CrystalConfig::load(KConfig*)
{
	QColor color(255,255,255);

	config_->setGroup("General");

	QString value = config_->readEntry("TitleAlignment", "AlignHCenter");
	QRadioButton *button = (QRadioButton*)dialog_->titlealign->child(value);
	if (button) button->setChecked(true);
	
	dialog_->drawCaption->setChecked(config_->readBoolEntry("DrawCaption",true));
	dialog_->textshadow->setChecked(config_->readBoolEntry("TextShadow",true));
	dialog_->tooltip->setChecked(config_->readBoolEntry("CaptionTooltip",true));
	dialog_->wheelTask->setChecked(config_->readBoolEntry("WheelTask",true));

	dialog_->enableTransparency->setChecked(config_->readBoolEntry("EnableTransparency",true));
	dialog_->trackdesktop->setChecked(config_->readBoolEntry("TrackDesktop",false));

	dialog_->frame1->setCurrentItem(config_->readNumEntry("ActiveFrame",1));
	color=QColor(192,192,192);
	dialog_->frameColor1->setColor(config_->readColorEntry("FrameColor1",&color));
	dialog_->frame2->setCurrentItem(config_->readNumEntry("InactiveFrame",1));
	color=QColor(192,192,192);
	dialog_->frameColor2->setColor(config_->readColorEntry("FrameColor2",&color));

	dialog_->inline1->setCurrentItem(config_->readNumEntry("ActiveInline",0));
	color=QColor(192,192,192);
	dialog_->inlineColor1->setColor(config_->readColorEntry("InlineColor1",&color));
	dialog_->inline2->setCurrentItem(config_->readNumEntry("InactiveInline",0));
	color=QColor(192,192,192);
	dialog_->inlineColor2->setColor(config_->readColorEntry("InlineColor2",&color));


	dialog_->borderwidth->setValue(config_->readNumEntry("Borderwidth",5));
	dialog_->titlebarheight->setValue(config_->readNumEntry("Titlebarheight",19));

	int active=config_->readNumEntry("ActiveShade",30);
	dialog_->shade1->setValue(active);

	active=config_->readNumEntry("InactiveShade",-30);
	dialog_->shade2->setValue(active);

	dialog_->type1->setCurrentItem(config_->readNumEntry("ActiveMode",0));
	dialog_->type2->setCurrentItem(config_->readNumEntry("InactiveMode",2));

	int cornersFlag = config_->readNumEntry("RoundCorners",TOP_LEFT & TOP_RIGHT );
	dialog_->tlc->setChecked( cornersFlag & TOP_LEFT );
	dialog_->trc->setChecked( cornersFlag & TOP_RIGHT );
	dialog_->blc->setChecked( cornersFlag & BOT_LEFT );
	dialog_->brc->setChecked( cornersFlag & BOT_RIGHT );

	dialog_->hover->setChecked(config_->readBoolEntry("HoverEffect",true));
	dialog_->animateHover->setChecked(config_->readBoolEntry("AnimateHover",true));

	color=QColor(255,255,255);
	dialog_->buttonColor1->setColor(config_->readColorEntry("ButtonColor",&color));
	dialog_->buttonColor2->setColor(config_->readColorEntry("ButtonColor2",&color));
	dialog_->buttonColor3->setColor(config_->readColorEntry("ButtonColor3",&color));
	dialog_->minColor1->setColor(config_->readColorEntry("MinColor",&color));
	dialog_->minColor2->setColor(config_->readColorEntry("MinColor2",&color));
	dialog_->minColor3->setColor(config_->readColorEntry("MinColor3",&color));
	dialog_->maxColor1->setColor(config_->readColorEntry("MaxColor",&color));
	dialog_->maxColor2->setColor(config_->readColorEntry("MaxColor2",&color));
	dialog_->maxColor3->setColor(config_->readColorEntry("MaxColor3",&color));
	dialog_->closeColor1->setColor(config_->readColorEntry("CloseColor",&color));
	dialog_->closeColor2->setColor(config_->readColorEntry("CloseColor2",&color));
	dialog_->closeColor3->setColor(config_->readColorEntry("CloseColor3",&color));

	dialog_->tintButtons->setChecked(config_->readBoolEntry("TintButtons",dialog_->buttonColor1->color()!=QColor(255,255,255)));

	dialog_->buttonTheme->setCurrentItem(config_->readNumEntry("ButtonTheme",0));
	
	dialog_->updateTime->setValue(config_->readNumEntry("RepaintTime",200));
	button=(QRadioButton*)dialog_->repaintMode->find(config_->readNumEntry("RepaintMode",1));
	if (button)button->setChecked(true);

	dialog_->active_blur->setValue(config_->readNumEntry("ActiveBlur",0));
	dialog_->inactive_blur->setValue(config_->readNumEntry("InactiveBlur",0));


	dialog_->activeFile->setURL(config_->readEntry("ActiveUserdefinedPicture",""));
	dialog_->userPicture1->setChecked(config_->readBoolEntry("ActiveUserdefined",false));
	dialog_->inactiveFile->setURL(config_->readEntry("InactiveUserdefinedPicture",""));
	dialog_->userPicture2->setChecked(config_->readBoolEntry("InactiveUserdefined",false));


	dialog_->overlay_active->setCurrentItem(config_->readNumEntry("OverlayModeActive",0));
	dialog_->overlay_active_file->setURL(config_->readEntry("OverlayFileActive",""));
	overlay_active_changed(dialog_->overlay_active->currentItem());

	dialog_->overlay_inactive->setCurrentItem(config_->readNumEntry("OverlayModeInactive",0));
	dialog_->overlay_inactive_file->setURL(config_->readEntry("OverlayFileInactive",""));
	overlay_inactive_changed(dialog_->overlay_inactive->currentItem());

	dialog_->logoEnabled->setButton(config_->readNumEntry("LogoAlignment",1));
	dialog_->logoFile->setURL(config_->readEntry("LogoFile",""));
	dialog_->logoActive->setChecked(config_->readBoolEntry("LogoActive",1));
	dialog_->logoStretch->setCurrentItem(config_->readNumEntry("LogoStretch",0));
	dialog_->logoDistance->setValue(config_->readNumEntry("LogoDistance",0));
	updateLogo();
}

void CrystalConfig::save(KConfig*)
{
	config_->setGroup("General");

	QRadioButton *button = (QRadioButton*)dialog_->titlealign->selected();
	if (button) config_->writeEntry("TitleAlignment", QString(button->name()));
	config_->writeEntry("DrawCaption",dialog_->drawCaption->isChecked());
	config_->writeEntry("TextShadow",dialog_->textshadow->isChecked());
	config_->writeEntry("CaptionTooltip",dialog_->tooltip->isChecked());
	config_->writeEntry("WheelTask",dialog_->wheelTask->isChecked());

	config_->writeEntry("EnableTransparency",dialog_->enableTransparency->isChecked());
	config_->writeEntry("TrackDesktop",dialog_->trackdesktop->isChecked());

	config_->writeEntry("Borderwidth",dialog_->borderwidth->value());
	config_->writeEntry("Titlebarheight",dialog_->titlebarheight->value());

	config_->writeEntry("ActiveShade",dialog_->shade1->value());
	config_->writeEntry("InactiveShade",dialog_->shade2->value());
	config_->writeEntry("ActiveFrame",dialog_->frame1->currentItem());
	config_->writeEntry("FrameColor1",dialog_->frameColor1->color());
	config_->writeEntry("InactiveFrame",dialog_->frame2->currentItem());
	config_->writeEntry("ActiveMode",dialog_->type1->currentItem());
	config_->writeEntry("InactiveMode",dialog_->type2->currentItem());
	config_->writeEntry("FrameColor2",dialog_->frameColor2->color());

	config_->writeEntry("ActiveInline",dialog_->inline1->currentItem());
	config_->writeEntry("InlineColor1",dialog_->inlineColor1->color());
	config_->writeEntry("InactiveInline",dialog_->inline2->currentItem());
	config_->writeEntry("InlineColor2",dialog_->inlineColor2->color());

	config_->writeEntry("ButtonColor",dialog_->buttonColor1->color());
	config_->writeEntry("ButtonColor2",dialog_->buttonColor2->color());
	config_->writeEntry("ButtonColor3",dialog_->buttonColor3->color());
	config_->writeEntry("MinColor",dialog_->minColor1->color());
	config_->writeEntry("MinColor2",dialog_->minColor2->color());
	config_->writeEntry("MinColor3",dialog_->minColor3->color());
	config_->writeEntry("MaxColor",dialog_->maxColor1->color());
	config_->writeEntry("MaxColor2",dialog_->maxColor2->color());
	config_->writeEntry("MaxColor3",dialog_->maxColor3->color());
	config_->writeEntry("CloseColor",dialog_->closeColor1->color());
	config_->writeEntry("CloseColor2",dialog_->closeColor2->color());
	config_->writeEntry("CloseColor3",dialog_->closeColor3->color());

	int cornersFlag = 0;
	if(dialog_->tlc->isChecked()) cornersFlag += TOP_LEFT;
	if(dialog_->trc->isChecked()) cornersFlag += TOP_RIGHT;
	if(dialog_->blc->isChecked()) cornersFlag += BOT_LEFT;
	if(dialog_->brc->isChecked()) cornersFlag += BOT_RIGHT;
	config_->writeEntry("RoundCorners", cornersFlag );
	
	config_->writeEntry("HoverEffect",dialog_->hover->isChecked());
	config_->writeEntry("AnimateHover",dialog_->animateHover->isChecked());
	config_->writeEntry("TintButtons",dialog_->tintButtons->isChecked());
	
	config_->writeEntry("ButtonTheme",dialog_->buttonTheme->currentItem());
	config_->writeEntry("RepaintMode",dialog_->repaintMode->selectedId());
	config_->writeEntry("RepaintTime",dialog_->updateTime->value());

	config_->writeEntry("ActiveBlur",dialog_->active_blur->value());
	config_->writeEntry("InactiveBlur",dialog_->inactive_blur->value());

	config_->writeEntry("ActiveUserdefined",dialog_->userPicture1->isChecked());
	config_->writeEntry("ActiveUserdefinedPicture",dialog_->activeFile->url());
	config_->writeEntry("InactiveUserdefined",dialog_->userPicture2->isChecked());
	config_->writeEntry("InactiveUserdefinedPicture",dialog_->inactiveFile->url());

	config_->writeEntry("OverlayModeActive",dialog_->overlay_active->currentItem());
	config_->writeEntry("OverlayFileActive",dialog_->overlay_active_file->url());
	config_->writeEntry("OverlayModeInactive",dialog_->overlay_inactive->currentItem());
	config_->writeEntry("OverlayFileInactive",dialog_->overlay_inactive_file->url());

	config_->writeEntry("LogoAlignment",dialog_->logoEnabled->selectedId());
	config_->writeEntry("LogoFile",dialog_->logoFile->url());
	config_->writeEntry("LogoActive",dialog_->logoActive->isChecked());
	config_->writeEntry("LogoStretch",dialog_->logoStretch->currentItem());
	config_->writeEntry("LogoDistance",dialog_->logoDistance->value());

	config_->sync();
}

void CrystalConfig::infoDialog()
{
	InfoDialog d(dialog_);
	connect((QLabel*)(d.kURLLabel1),SIGNAL(leftClickedURL(const QString&)),KApplication::kApplication(),SLOT(invokeBrowser(const QString &)));
	d.exec();
}

void CrystalConfig::logoTextChanged(const QString&)
{
	updateLogo();
	emit changed();
}

void CrystalConfig::overlay_active_changed(int a)
{
	dialog_->overlay_active_file->setEnabled(a==4);
	emit changed();
}

void CrystalConfig::overlay_inactive_changed(int a)
{
	dialog_->overlay_inactive_file->setEnabled(a==4);
	emit changed();
}

void CrystalConfig::updateLogo()
{
	QPixmap pic;
	pic.load(dialog_->logoFile->url());
	dialog_->logoPreview->setPixmap(pic);
}

void CrystalConfig::defaults()
{
	QRadioButton *button =
		(QRadioButton*)dialog_->titlealign->child("AlignHCenter");
	if (button) button->setChecked(true);
	dialog_->shade1->setValue(50);
	dialog_->shade2->setValue(50);
}

extern "C"
{
	QObject* allocate_config(KConfig* config, QWidget* parent) {
		return (new CrystalConfig(config, parent));
	}
}

#include "crystalconfig.moc"
