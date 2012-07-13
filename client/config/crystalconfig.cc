//////////////////////////////////////////////////////////////////////////////
// exampleconfig.cc
// -------------------
// Config module for Example window decoration
// -------------------
// Copyright (c) 2003 David Johnson <david@usermode.org>
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////

#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qwhatsthis.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qwidgetstack.h>
#include <kcolorbutton.h>
#include <knuminput.h>

#include "crystalconfig.h"
#include "configdialog.h"
#include "infodialog.h"

//////////////////////////////////////////////////////////////////////////////
// ExampleConfig()
// -------------
// Constructor

ExampleConfig::ExampleConfig(KConfig*, QWidget* parent)
    : QObject(parent), config_(0), dialog_(0)
{
    // create the configuration object
    config_ = new KConfig("kwincrystalrc");
    KGlobal::locale()->insertCatalogue("kwin_crystal_config");

    // create and show the configuration dialog
    dialog_ = new ConfigDialog(parent);
    dialog_->show();

    // load the configuration
    load(config_);

    // setup the connections
    connect(dialog_->titlealign, SIGNAL(clicked(int)),
            this, SLOT(selectionChanged(int)));
    
    connect(dialog_->textshadow, SIGNAL(stateChanged(int)),
            this, SLOT(selectionChanged(int)));
    connect(dialog_->antialiaseCaption, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
			
    connect(dialog_->trackdesktop, SIGNAL(stateChanged(int)),
            this, SLOT(selectionChanged(int)));
    
    connect(dialog_->borderwidth, SIGNAL(valueChanged(int)),
            this, SLOT(selectionChanged(int)));
    connect(dialog_->titlebarheight, SIGNAL(valueChanged(int)),
            this, SLOT(selectionChanged(int)));
    
	connect(dialog_->tlc, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->trc, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->blc, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->brc, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    
	connect(dialog_->normalColorNormal, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->normalColorHovered, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->normalColorPressed, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->closeColorNormal, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->closeColorHovered, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->closeColorPressed, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));

    connect(dialog_->hover, SIGNAL(stateChanged(int)),this,SLOT(hoverChanged(int)));
	connect(dialog_->buttonTheme, SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->fadeButtons, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->animateHover, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));

	connect(dialog_->textureSize, SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->useTransparency, SIGNAL(stateChanged(int)),this,SLOT(transparencyChanged(int)));
    connect(dialog_->useRefraction, SIGNAL(stateChanged(int)),this,SLOT(refractionChanged(int)));
    connect(dialog_->useLighting, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));

    connect(dialog_->animateActivate, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->iorActive, SIGNAL(valueChanged(double)),
            this, SLOT(selectionChanged(double)));
    connect(dialog_->iorInactive, SIGNAL(valueChanged(double)),
            this, SLOT(selectionChanged(double)));
    connect(dialog_->colorActive, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
    connect(dialog_->colorInactive, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	connect(dialog_->brightness,SIGNAL(valueChanged(int)),this,SLOT(selectionChanged(int)));
	
	
    connect(dialog_->repaintMode, SIGNAL(clicked(int)),
            this, SLOT(selectionChanged(int)));

    connect(dialog_->updateTime, SIGNAL(valueChanged(int)),
            this, SLOT(selectionChanged(int)));
	
	connect(dialog_->infoButton, SIGNAL(clicked(void)),this,SLOT(infoDialog(void)));
}

//////////////////////////////////////////////////////////////////////////////
// ~ExampleConfig()
// --------------
// Destructor

ExampleConfig::~ExampleConfig()
{
    if (dialog_) delete dialog_;
    if (config_) delete config_;
}

//////////////////////////////////////////////////////////////////////////////
// selectionChanged()
// ------------------
// Selection has changed

void ExampleConfig::hoverChanged(int bla)
{
	dialog_->animateHover->setEnabled(bla==QButton::On);
	emit changed();
}

void ExampleConfig::transparencyChanged(int bla)
{
	bool e=(bla==QButton::On);
	bool f=dialog_->useRefraction->isChecked();
	dialog_->useRefraction->setEnabled(e);
	dialog_->iorActive->setEnabled(e && f);
	dialog_->iorInactive->setEnabled(e && f);
	emit changed();
}

void ExampleConfig::refractionChanged(int bla)
{
	bool e=(bla==QButton::On);
	
	dialog_->iorActive->setEnabled(e);
	dialog_->iorInactive->setEnabled(e);
	
	emit changed();
}


void ExampleConfig::selectionChanged(int)
{
    emit changed();
}

void ExampleConfig::selectionChanged(double)
{
	emit changed();
}

//////////////////////////////////////////////////////////////////////////////
// load()
// ------
// Load configuration data

void ExampleConfig::load(KConfig*)
{
    QColor color(255,255,255);
	bool b1,b2;
    
	config_->setGroup("CrystalGL");
    
    QString value = config_->readEntry("TitleAlignment", "AlignHCenter");
    QRadioButton *button = (QRadioButton*)dialog_->titlealign->child(value);
    if (button) button->setChecked(true);
    
    dialog_->textshadow->setChecked(config_->readBoolEntry("TextShadow",true));
    dialog_->antialiaseCaption->setChecked(config_->readBoolEntry("AntialiaseCaption",false));
    dialog_->trackdesktop->setChecked(config_->readBoolEntry("TrackDesktop",true));
    
    
    dialog_->borderwidth->setValue(config_->readNumEntry("Borderwidth",4));
    dialog_->titlebarheight->setValue(config_->readNumEntry("Titlebarheight",20));
    
	color=QColor(255,255,255);
    dialog_->normalColorNormal->setColor(config_->readColorEntry("ButtonColor",&color));
    dialog_->normalColorHovered->setColor(config_->readColorEntry("ButtonHoveredColor",&color));
    dialog_->normalColorPressed->setColor(config_->readColorEntry("ButtonPressedColor",&color));
    dialog_->closeColorNormal->setColor(config_->readColorEntry("CloseColor",&color));
    dialog_->closeColorHovered->setColor(config_->readColorEntry("CloseHoveredColor",&color));
    dialog_->closeColorPressed->setColor(config_->readColorEntry("ClosePressedColor",&color));
	
    int cornersFlag = config_->readNumEntry("RoundCorners",TOP_LEFT & TOP_RIGHT );
    dialog_->tlc->setChecked( cornersFlag & TOP_LEFT );
    dialog_->trc->setChecked( cornersFlag & TOP_RIGHT );
    dialog_->blc->setChecked( cornersFlag & BOT_LEFT );
    dialog_->brc->setChecked( cornersFlag & BOT_RIGHT );
	
	dialog_->hover->setChecked(b1=config_->readBoolEntry("HoverEffect",true));
	dialog_->animateHover->setEnabled(b1);

	dialog_->animateHover->setChecked(config_->readBoolEntry("AnimateHover",true));
	
	dialog_->buttonTheme->setCurrentItem(config_->readNumEntry("ButtonTheme",0));
	
	button=(QRadioButton*)dialog_->repaintMode->find(config_->readNumEntry("RepaintMode",1));
	if (button)button->setChecked(true);
    dialog_->updateTime->setValue(config_->readNumEntry("RepaintTime",200));

	dialog_->fadeButtons->setChecked(config_->readBoolEntry("FadeButtons",true));
	dialog_->textureSize->setCurrentItem(config_->readNumEntry("TextureSize",2));
	dialog_->useTransparency->setChecked(b1=config_->readBoolEntry("Transparency",true));
	dialog_->useRefraction->setEnabled(b1);
	dialog_->useRefraction->setChecked(b2=config_->readBoolEntry("SimulateRefraction",true));
	dialog_->useLighting->setChecked(config_->readBoolEntry("SimulateLighting",true));

	dialog_->animateActivate->setChecked(config_->readBoolEntry("AnimateActivate",true));
    dialog_->iorActive->setValue(config_->readDoubleNumEntry("IORActive",2.4));
    dialog_->iorInactive->setValue(config_->readDoubleNumEntry("IORInactive",1.2));
	dialog_->iorActive->setEnabled(b1 && b2);
	dialog_->iorInactive->setEnabled(b1 && b2);	
	
	color=QColor(150,160,255);
    dialog_->colorActive->setColor(config_->readColorEntry("ActiveColor",&color));
	color=QColor(160,160,160);
    dialog_->colorInactive->setColor(config_->readColorEntry("InactiveColor",&color));
	dialog_->brightness->setValue(config_->readNumEntry("Brightness",100));
}

//////////////////////////////////////////////////////////////////////////////
// save()
// ------
// Save configuration data

void ExampleConfig::save(KConfig*)
{
    config_->setGroup("CrystalGL");

    QRadioButton *button = (QRadioButton*)dialog_->titlealign->selected();
    if (button) config_->writeEntry("TitleAlignment", QString(button->name()));
    config_->writeEntry("TextShadow",dialog_->textshadow->isChecked());
    config_->writeEntry("AntialiaseCaption",dialog_->antialiaseCaption->isChecked());
    config_->writeEntry("TrackDesktop",dialog_->trackdesktop->isChecked());
    
    config_->writeEntry("Borderwidth",dialog_->borderwidth->value());
    config_->writeEntry("Titlebarheight",dialog_->titlebarheight->value());
    
	config_->writeEntry("ButtonColor",dialog_->normalColorNormal->color());
	config_->writeEntry("ButtonHoveredColor",dialog_->normalColorHovered->color());
	config_->writeEntry("ButtonPressedColor",dialog_->normalColorPressed->color());
	config_->writeEntry("CloseColor",dialog_->closeColorNormal->color());
	config_->writeEntry("CloseHoveredColor",dialog_->closeColorHovered->color());
	config_->writeEntry("ClosePressedColor",dialog_->closeColorPressed->color());
	
    int cornersFlag = 0;
    if(dialog_->tlc->isChecked()) cornersFlag += TOP_LEFT;
    if(dialog_->trc->isChecked()) cornersFlag += TOP_RIGHT;
    if(dialog_->blc->isChecked()) cornersFlag += BOT_LEFT;
    if(dialog_->brc->isChecked()) cornersFlag += BOT_RIGHT;
    config_->writeEntry("RoundCorners", cornersFlag );
	
	config_->writeEntry("HoverEffect",dialog_->hover->isChecked());
	config_->writeEntry("FadeButtons",dialog_->fadeButtons->isChecked());
	config_->writeEntry("AnimateHover",dialog_->animateHover->isChecked());
	
	config_->writeEntry("ButtonTheme",dialog_->buttonTheme->currentItem());
	config_->writeEntry("RepaintMode",dialog_->repaintMode->selectedId());
    config_->writeEntry("RepaintTime",dialog_->updateTime->value());
        
	
	config_->writeEntry("TextureSize",dialog_->textureSize->currentItem());
	config_->writeEntry("Transparency",dialog_->useTransparency->isChecked());
	config_->writeEntry("SimulateRefraction",dialog_->useRefraction->isChecked());
	config_->writeEntry("SimulateLighting",dialog_->useLighting->isChecked());
	config_->writeEntry("AnimateActivate",dialog_->animateActivate->isChecked());
    config_->writeEntry("IORActive",dialog_->iorActive->value());
    config_->writeEntry("IORInactive",dialog_->iorInactive->value());
	
	config_->writeEntry("ActiveColor",dialog_->colorActive->color());
	config_->writeEntry("InactiveColor",dialog_->colorInactive->color());
	config_->writeEntry("Brightness",dialog_->brightness->value());
    config_->sync();
}

void ExampleConfig::infoDialog()
{
	InfoDialog d(dialog_);
	d.exec();
}


//////////////////////////////////////////////////////////////////////////////
// defaults()
// ----------
// Set configuration defaults

void ExampleConfig::defaults()
{
    QRadioButton *button =
        (QRadioButton*)dialog_->titlealign->child("AlignHCenter");
    if (button) button->setChecked(true);
}

//////////////////////////////////////////////////////////////////////////////
// Plugin Stuff                                                             //
//////////////////////////////////////////////////////////////////////////////

extern "C"
{
    QObject* allocate_config(KConfig* config, QWidget* parent) {
        return (new ExampleConfig(config, parent));
    }
}

#include "crystalconfig.moc"
