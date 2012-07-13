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
    connect(dialog_->buttonColor, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));

    connect(dialog_->hover, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->buttonTheme, SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->tintButtons, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->fadeButtons, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));

	connect(dialog_->textureSize, SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->useRefraction, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->useLighting, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));

    connect(dialog_->animateActivate, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->iorActive, SIGNAL(valueChanged(double)),
            this, SLOT(selectionChanged(double)));
    connect(dialog_->iorInactive, SIGNAL(valueChanged(double)),
            this, SLOT(selectionChanged(double)));
    connect(dialog_->colorActive, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
    connect(dialog_->colorInactive, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));

	
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
    dialog_->buttonColor->setColor(config_->readColorEntry("ButtonColor",&color));
    int cornersFlag = config_->readNumEntry("RoundCorners",TOP_LEFT & TOP_RIGHT );
    dialog_->tlc->setChecked( cornersFlag & TOP_LEFT );
    dialog_->trc->setChecked( cornersFlag & TOP_RIGHT );
    dialog_->blc->setChecked( cornersFlag & BOT_LEFT );
    dialog_->brc->setChecked( cornersFlag & BOT_RIGHT );
	
	dialog_->hover->setChecked(config_->readBoolEntry("HoverEffect",true));
	dialog_->tintButtons->setChecked(config_->readBoolEntry("TintButtons",dialog_->buttonColor->color()!=QColor(255,255,255)));
	
	dialog_->buttonTheme->setCurrentItem(config_->readNumEntry("ButtonTheme",0));
	
	button=(QRadioButton*)dialog_->repaintMode->find(config_->readNumEntry("RepaintMode",1));
	if (button)button->setChecked(true);
    dialog_->updateTime->setValue(config_->readNumEntry("RepaintTime",200));

	dialog_->fadeButtons->setChecked(config_->readBoolEntry("FadeButtons",true));
	dialog_->textureSize->setCurrentItem(config_->readNumEntry("TextureSize",2));
	dialog_->useRefraction->setChecked(config_->readBoolEntry("SimulateRefraction",true));
	dialog_->useLighting->setChecked(config_->readBoolEntry("SimulateLighting",true));

	dialog_->animateActivate->setChecked(config_->readBoolEntry("AnimateActivate",true));
    dialog_->iorActive->setValue(config_->readDoubleNumEntry("IORActive",2.4));
    dialog_->iorInactive->setValue(config_->readDoubleNumEntry("IORInactive",1.2));
	
	color=QColor(150,160,255);
    dialog_->colorActive->setColor(config_->readColorEntry("ActiveColor",&color));
	color=QColor(160,160,160);
    dialog_->colorInactive->setColor(config_->readColorEntry("InactiveColor",&color));
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
    
	config_->writeEntry("ButtonColor",dialog_->buttonColor->color());
	
    int cornersFlag = 0;
    if(dialog_->tlc->isChecked()) cornersFlag += TOP_LEFT;
    if(dialog_->trc->isChecked()) cornersFlag += TOP_RIGHT;
    if(dialog_->blc->isChecked()) cornersFlag += BOT_LEFT;
    if(dialog_->brc->isChecked()) cornersFlag += BOT_RIGHT;
    config_->writeEntry("RoundCorners", cornersFlag );
	
	config_->writeEntry("HoverEffect",dialog_->hover->isChecked());
	config_->writeEntry("TintButtons",dialog_->tintButtons->isChecked());
	config_->writeEntry("FadeButtons",dialog_->fadeButtons->isChecked());
	
	config_->writeEntry("ButtonTheme",dialog_->buttonTheme->currentItem());
	config_->writeEntry("RepaintMode",dialog_->repaintMode->selectedId());
    config_->writeEntry("RepaintTime",dialog_->updateTime->value());
        
	
	config_->writeEntry("TextureSize",dialog_->textureSize->currentItem());
	config_->writeEntry("SimulateRefraction",dialog_->useRefraction->isChecked());
	config_->writeEntry("SimulateLighting",dialog_->useLighting->isChecked());
	config_->writeEntry("AnimateActivate",dialog_->animateActivate->isChecked());
	config_->writeEntry("TintButtons",dialog_->tintButtons->isChecked());
    config_->writeEntry("IORActive",dialog_->iorActive->value());
    config_->writeEntry("IORInactive",dialog_->iorInactive->value());
	
	config_->writeEntry("ActiveColor",dialog_->colorActive->color());
	config_->writeEntry("InactiveColor",dialog_->colorInactive->color());
	
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
