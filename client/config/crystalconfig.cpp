/***************************************************************************
 *   Copyright (C) 2006-2008 by Sascha Hlusiak                             *
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

#include <QObject>
#include <ktoolinvocation.h>

#include "crystalconfig.h"

#include "../logos.h"


CrystalConfig::CrystalConfig(KConfig*, QWidget* parent)
    : QObject(parent), config_(0), dialog_(0)
{
	config_ = new KConfig("kwincrystalrc");
	KConfigGroup cg(config_, "General");
	
	dialog_ = new ConfigDialog(parent);
	dialog_->show();
		
	connect(dialog_->titlealign, SIGNAL(clicked(int)),this, SLOT(selectionChanged(int)));
	
	connect(dialog_->drawCaption, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->textshadow, SIGNAL(stateChanged(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->tooltip,SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->wheelTask,SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));

	connect(dialog_->frame1, SIGNAL(activated(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->frame2, SIGNAL(activated(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->frameColor1, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->frameColor2, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	
	connect(dialog_->inline1, SIGNAL(activated(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->inline2, SIGNAL(activated(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->inlineColor1, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->inlineColor2, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
		
	connect(dialog_->active_transparency,SIGNAL(valueChanged(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->inactive_transparency,SIGNAL(valueChanged(int)),this,SLOT(selectionChanged(int)));
	
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
	connect(dialog_->menuimage, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
	
	connect(dialog_->infoButton, SIGNAL(clicked(void)),this,SLOT(infoDialog(void)));

	connect(dialog_->overlay_active, SIGNAL(activated(int)),this, SLOT(overlay_active_changed(int)));
	connect(dialog_->overlay_inactive, SIGNAL(activated(int)),this, SLOT(overlay_inactive_changed(int)));

	connect(dialog_->overlay_active_file,SIGNAL(textChanged(const QString&)),this,SLOT(textChanged(const QString &)));
	connect(dialog_->overlay_inactive_file,SIGNAL(textChanged(const QString&)),this,SLOT(textChanged(const QString &)));

	// -----------------------------------------------------------------------------------------------
	// CQ 20090101
	connect(dialog_->stretch_active, SIGNAL(toggled(bool)),this,SLOT(boolChanged(bool)));
	connect(dialog_->stretch_inactive, SIGNAL(toggled(bool)),this,SLOT(boolChanged(bool)));
	connect(dialog_->fwidth_active, SIGNAL(toggled(bool)),this,SLOT(boolChanged(bool)));
	connect(dialog_->fwidth_inactive, SIGNAL(toggled(bool)),this,SLOT(boolChanged(bool)));
	connect(dialog_->nostretch_active, SIGNAL(toggled(bool)),this,SLOT(boolChanged(bool)));
	connect(dialog_->nostretch_inactive, SIGNAL(toggled(bool)),this,SLOT(boolChanged(bool)));
	connect(dialog_->fwvalue_active, SIGNAL(valueChanged(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->fwvalue_inactive, SIGNAL(valueChanged(int)),this, SLOT(selectionChanged(int)));
	// -----------------------------------------------------------------------------------------------

	connect(dialog_->logoEnabled, SIGNAL(clicked(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->logoFile, SIGNAL(textChanged(const QString &)),this, SLOT(logoTextChanged(const QString&)));
	connect(dialog_->logoStretch, SIGNAL(activated(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->logoActive, SIGNAL(stateChanged(int)),this, SLOT(selectionChanged(int)));
	connect(dialog_->logoDistance,SIGNAL(valueChanged(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->logoList,SIGNAL(activated(int)), this, SLOT(logoIndexChanged(int)));

	load(cg);
}

CrystalConfig::~CrystalConfig()
{
	if (dialog_) delete dialog_;
	dialog_ = NULL;
	if (config_) delete config_;
	config_ = NULL;
}

void CrystalConfig::selectionChanged(int)
{
	emit changed();
}

void CrystalConfig::load(KConfigGroup&)
{
	KConfigGroup cg(config_, "General");
	QColor color(255,255,255);

	int value = cg.readEntry("TitleAlignment", 1);
	QRadioButton *button = (QRadioButton*)dialog_->titlealign->find(value);
	if (button) button->setChecked(true);
	
	dialog_->drawCaption->setChecked(cg.readEntry("DrawCaption",true));
	dialog_->textshadow->setChecked(cg.readEntry("TextShadow",true));
	dialog_->tooltip->setChecked(cg.readEntry("CaptionTooltip",true));
	dialog_->wheelTask->setChecked(cg.readEntry("WheelTask",false));

	dialog_->active_transparency->setValue(cg.readEntry("ActiveTransparency",80));
	dialog_->inactive_transparency->setValue(cg.readEntry("InactiveTransparency",60));

	dialog_->frame1->setCurrentIndex(cg.readEntry("ActiveFrame",1));
	color=QColor(192,192,192);
	dialog_->frameColor1->setColor(cg.readEntry("FrameColor1",color));
	dialog_->frame2->setCurrentIndex(cg.readEntry("InactiveFrame",1));
	color=QColor(192,192,192);
	dialog_->frameColor2->setColor(cg.readEntry("FrameColor2",color));

	dialog_->inline1->setCurrentIndex(cg.readEntry("ActiveInline",0));
	color=QColor(192,192,192);
	dialog_->inlineColor1->setColor(cg.readEntry("InlineColor1",color));
	dialog_->inline2->setCurrentIndex(cg.readEntry("InactiveInline",0));
	color=QColor(192,192,192);
	dialog_->inlineColor2->setColor(cg.readEntry("InlineColor2",color));

	dialog_->borderwidth->setValue(cg.readEntry("Borderwidth",6));
	dialog_->titlebarheight->setValue(cg.readEntry("Titlebarheight",21));

	int cornersFlag = cg.readEntry("RoundCorners",TOP_LEFT & TOP_RIGHT );
	dialog_->tlc->setChecked( cornersFlag & TOP_LEFT );
	dialog_->trc->setChecked( cornersFlag & TOP_RIGHT );
	dialog_->blc->setChecked( cornersFlag & BOT_LEFT );
	dialog_->brc->setChecked( cornersFlag & BOT_RIGHT );

	dialog_->hover->setChecked(cg.readEntry("HoverEffect",true));
	dialog_->animateHover->setChecked(cg.readEntry("AnimateHover",true));
	dialog_->menuimage->setChecked(cg.readEntry("MenuImage",true));

	color=QColor(255,255,255);
	dialog_->buttonColor1->setColor(cg.readEntry("ButtonColor",color));
	dialog_->buttonColor2->setColor(cg.readEntry("ButtonColor2",color));
	dialog_->buttonColor3->setColor(cg.readEntry("ButtonColor3",color));
	dialog_->minColor1->setColor(cg.readEntry("MinColor",color));
	dialog_->minColor2->setColor(cg.readEntry("MinColor2",color));
	dialog_->minColor3->setColor(cg.readEntry("MinColor3",color));
	dialog_->maxColor1->setColor(cg.readEntry("MaxColor",color));
	dialog_->maxColor2->setColor(cg.readEntry("MaxColor2",color));
	dialog_->maxColor3->setColor(cg.readEntry("MaxColor3",color));
	dialog_->closeColor1->setColor(cg.readEntry("CloseColor",color));
	dialog_->closeColor2->setColor(cg.readEntry("CloseColor2",color));
	dialog_->closeColor3->setColor(cg.readEntry("CloseColor3",color));

	dialog_->tintButtons->setChecked(cg.readEntry("TintButtons",dialog_->buttonColor1->color()!=QColor(255,255,255)));
	dialog_->buttonTheme->setCurrentIndex(cg.readEntry("ButtonTheme",10));
	
	dialog_->overlay_active->setCurrentIndex(cg.readEntry("OverlayModeActive",2));
	dialog_->overlay_active_file->setUrl(cg.readEntry("OverlayFileActive",""));
	dialog_->stretch_active->setChecked(cg.readEntry("OverlayStretchActive",false));
	dialog_->fwidth_active->setChecked(cg.readEntry("OverlayFWidthActive",true));
	dialog_->nostretch_active->setChecked(cg.readEntry("OverlayNoStretchActive",false));
	dialog_->fwvalue_active->setValue(cg.readEntry("OverlayFWValueActive",256));
	overlay_active_changed(dialog_->overlay_active->currentIndex());

	dialog_->overlay_inactive->setCurrentIndex(cg.readEntry("OverlayModeInactive",2));
	dialog_->overlay_inactive_file->setUrl(cg.readEntry("OverlayFileInactive",""));
	dialog_->stretch_inactive->setChecked(cg.readEntry("OverlayStretchInactive",false));
	dialog_->fwidth_inactive->setChecked(cg.readEntry("OverlayFWidthInactive",true));
	dialog_->nostretch_inactive->setChecked(cg.readEntry("OverlayNoStretchInactive",false));
	dialog_->fwvalue_inactive->setValue(cg.readEntry("OverlayFWValueInactive",256));
	overlay_inactive_changed(dialog_->overlay_inactive->currentIndex());

	dialog_->logoEnabled->setButton(cg.readEntry("LogoAlignment",1));
	dialog_->logoFile->setUrl(cg.readEntry("LogoFile",""));
	dialog_->logoActive->setChecked(cg.readEntry("LogoActive",true));
	dialog_->logoStretch->setCurrentIndex(cg.readEntry("LogoStretch",0));
	dialog_->logoDistance->setValue(cg.readEntry("LogoDistance",0));
	dialog_->logoList->setCurrentIndex(cg.readEntry("LogoIndex", 0));
	updateLogo();
}

void CrystalConfig::save(KConfigGroup&)
{
	KConfigGroup cg(config_, "General");

	int button = dialog_->titlealign->selectedId();
	cg.writeEntry("TitleAlignment", button);
	cg.writeEntry("DrawCaption",dialog_->drawCaption->isChecked());
	cg.writeEntry("TextShadow",dialog_->textshadow->isChecked());
	cg.writeEntry("CaptionTooltip",dialog_->tooltip->isChecked());
	cg.writeEntry("WheelTask",dialog_->wheelTask->isChecked());

	cg.writeEntry("ActiveTransparency",dialog_->active_transparency->value());
	cg.writeEntry("InactiveTransparency",dialog_->inactive_transparency->value());

	cg.writeEntry("Borderwidth",dialog_->borderwidth->value());
	cg.writeEntry("Titlebarheight",dialog_->titlebarheight->value());

	cg.writeEntry("ActiveFrame",dialog_->frame1->currentIndex());
	cg.writeEntry("FrameColor1",dialog_->frameColor1->color());
	cg.writeEntry("InactiveFrame",dialog_->frame2->currentIndex());
	cg.writeEntry("FrameColor2",dialog_->frameColor2->color());

	cg.writeEntry("ActiveInline",dialog_->inline1->currentIndex());
	cg.writeEntry("InlineColor1",dialog_->inlineColor1->color());
	cg.writeEntry("InactiveInline",dialog_->inline2->currentIndex());
	cg.writeEntry("InlineColor2",dialog_->inlineColor2->color());

	cg.writeEntry("ButtonColor",dialog_->buttonColor1->color());
	cg.writeEntry("ButtonColor2",dialog_->buttonColor2->color());
	cg.writeEntry("ButtonColor3",dialog_->buttonColor3->color());
	cg.writeEntry("MinColor",dialog_->minColor1->color());
	cg.writeEntry("MinColor2",dialog_->minColor2->color());
	cg.writeEntry("MinColor3",dialog_->minColor3->color());
	cg.writeEntry("MaxColor",dialog_->maxColor1->color());
	cg.writeEntry("MaxColor2",dialog_->maxColor2->color());
	cg.writeEntry("MaxColor3",dialog_->maxColor3->color());
	cg.writeEntry("CloseColor",dialog_->closeColor1->color());
	cg.writeEntry("CloseColor2",dialog_->closeColor2->color());
	cg.writeEntry("CloseColor3",dialog_->closeColor3->color());

	int cornersFlag = 0;
	if(dialog_->tlc->isChecked()) cornersFlag += TOP_LEFT;
	if(dialog_->trc->isChecked()) cornersFlag += TOP_RIGHT;
	if(dialog_->blc->isChecked()) cornersFlag += BOT_LEFT;
	if(dialog_->brc->isChecked()) cornersFlag += BOT_RIGHT;
	cg.writeEntry("RoundCorners", cornersFlag );
	
	cg.writeEntry("HoverEffect",dialog_->hover->isChecked());
	cg.writeEntry("AnimateHover",dialog_->animateHover->isChecked());
	cg.writeEntry("TintButtons",dialog_->tintButtons->isChecked());
	cg.writeEntry("MenuImage",dialog_->menuimage->isChecked());
	
	cg.writeEntry("ButtonTheme",dialog_->buttonTheme->currentIndex());

	cg.writeEntry("OverlayModeActive",dialog_->overlay_active->currentIndex());
	cg.writeEntry("OverlayFileActive",dialog_->overlay_active_file->url().toLocalFile());
	cg.writeEntry("OverlayStretchActive",dialog_->stretch_active->isChecked());
	cg.writeEntry("OverlayFWidthActive",dialog_->fwidth_active->isChecked());
	cg.writeEntry("OverlayNoStretchActive",dialog_->nostretch_active->isChecked());
	cg.writeEntry("OverlayFWValueActive",dialog_->fwvalue_active->value());
	cg.writeEntry("OverlayModeInactive",dialog_->overlay_inactive->currentIndex());
	cg.writeEntry("OverlayFileInactive",dialog_->overlay_inactive_file->url().toLocalFile());
	cg.writeEntry("OverlayStretchInactive",dialog_->stretch_inactive->isChecked());
	cg.writeEntry("OverlayNoStretchInactive",dialog_->nostretch_inactive->isChecked());
	cg.writeEntry("OverlayFWidthInactive",dialog_->fwidth_inactive->isChecked());
	cg.writeEntry("OverlayFWValueInactive",dialog_->fwvalue_inactive->value());

	cg.writeEntry("LogoAlignment",dialog_->logoEnabled->selectedId());
	cg.writeEntry("LogoFile",dialog_->logoFile->url().toLocalFile());
	cg.writeEntry("LogoActive",dialog_->logoActive->isChecked());
	cg.writeEntry("LogoStretch",dialog_->logoStretch->currentIndex());
	cg.writeEntry("LogoDistance",dialog_->logoDistance->value());
	cg.writeEntry("LogoIndex",dialog_->logoList->currentIndex());

	config_->sync();
}

void CrystalConfig::infoDialog()
{
	InfoDialog d(dialog_);
	connect((QLabel*)(d.URL_Crystal_1),SIGNAL(leftClickedUrl(const QString&)),KToolInvocation::self(),SLOT(invokeBrowser(const QString &)));
	connect((QLabel*)(d.URL_Crystal_2),SIGNAL(leftClickedUrl(const QString&)),KToolInvocation::self(),SLOT(invokeBrowser(const QString &)));
	d.exec();
}

void CrystalConfig::logoTextChanged(const QString&)
{
	updateLogo();
	emit changed();
}

void CrystalConfig::logoIndexChanged(int)
{
	updateLogo();
	emit changed();
}

void CrystalConfig::overlay_active_changed(int a)
{
	dialog_->overlay_active_file->setEnabled(a==4);
	dialog_->stretch_active->setEnabled(a==4);
	dialog_->fwidth_active->setEnabled(a==4);
	dialog_->nostretch_active->setEnabled(a==4);
	dialog_->fwvalue_active->setEnabled((a==4) && (dialog_->fwidth_active->isChecked()));
	emit changed();
}

void CrystalConfig::overlay_inactive_changed(int a)
{
	dialog_->overlay_inactive_file->setEnabled(a==4);
	dialog_->stretch_inactive->setEnabled(a==4);
	dialog_->fwidth_inactive->setEnabled(a==4);
	dialog_->nostretch_inactive->setEnabled(a==4);
	dialog_->fwvalue_inactive->setEnabled((a==4) && (dialog_->fwidth_inactive->isChecked()));
	emit changed();
}

void CrystalConfig::updateLogo()
{
	QIcon icon;
	QPixmap pic;
	
	dialog_->logoFile->setEnabled(dialog_->logoList->currentIndex()==0);
	
	if (dialog_->logoList->currentIndex() == 0) {
		KUrl url;
		url = dialog_->logoFile->url();
		pic.load(url.toLocalFile());
	}else{
		icon = dialog_->logoList->itemIcon(dialog_->logoList->currentIndex());

		if (!icon.isNull()) {
			pic = icon.pixmap(32);
		}
	}
	dialog_->logoPreview->setPixmap(pic);
}

extern "C"
{
	KDE_EXPORT QObject* allocate_config(KConfig* config, QWidget* parent) {
		return (new CrystalConfig(config, parent));
	}
}

#include "crystalconfig.moc"
