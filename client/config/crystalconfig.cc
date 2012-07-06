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
    connect(dialog_->trackdesktop, SIGNAL(stateChanged(int)),
            this, SLOT(selectionChanged(int)));
    
    connect(dialog_->shade1, SIGNAL(valueChanged(int)),
            this, SLOT(selectionChanged(int)));
    connect(dialog_->shade2, SIGNAL(valueChanged(int)),
            this, SLOT(selectionChanged(int)));

    connect(dialog_->frame1, SIGNAL(stateChanged(int)),
            this, SLOT(selectionChanged(int)));
    connect(dialog_->frame2, SIGNAL(stateChanged(int)),
            this, SLOT(selectionChanged(int)));
    connect(dialog_->frameColor1, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
    connect(dialog_->frameColor2, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	    
    connect(dialog_->type1,SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->type2,SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));

    connect(dialog_->borderwidth, SIGNAL(valueChanged(int)),
            this, SLOT(selectionChanged(int)));
    connect(dialog_->titlebarheight, SIGNAL(valueChanged(int)),
            this, SLOT(selectionChanged(int)));

    connect(dialog_->roundCorners, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->buttonColor, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));

    connect(dialog_->hover, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
	connect(dialog_->buttonTheme, SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));

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

void ExampleConfig::updateStack(QWidgetStack* stack,int selected)
{
    switch(selected)
    {
    case 0: // Fade
    case 1: // channelIntensity
    case 2: // Brighten
    case 3: // desaturate
    case 4: // solarize
    	stack->raiseWidget(0);
    	break;
    
    default:stack->raiseWidget(1);
    	break;
    }
}

void ExampleConfig::selectionChanged(int)
{
    updateStack(dialog_->stack1,dialog_->type1->currentItem());
    updateStack(dialog_->stack2,dialog_->type2->currentItem());

    emit changed();
}

//////////////////////////////////////////////////////////////////////////////
// load()
// ------
// Load configuration data

void ExampleConfig::load(KConfig*)
{
    QColor color(255,255,255);
    
	config_->setGroup("General");
    
    QString value = config_->readEntry("TitleAlignment", "AlignHCenter");
    QRadioButton *button = (QRadioButton*)dialog_->titlealign->child(value);
    if (button) button->setChecked(true);
    
    dialog_->textshadow->setChecked(config_->readBoolEntry("TextShadow",true));
    dialog_->trackdesktop->setChecked(config_->readBoolEntry("TrackDesktop",true));
    
    dialog_->frame1->setChecked(config_->readBoolEntry("ActiveFrame",true));
	color=QColor(192,192,192);
    dialog_->frameColor1->setColor(config_->readColorEntry("FrameColor1",&color));
    dialog_->frame2->setChecked(config_->readBoolEntry("InactiveFrame",true));
	color=QColor(192,192,192);
    dialog_->frameColor2->setColor(config_->readColorEntry("FrameColor2",&color));
    
    dialog_->borderwidth->setValue(config_->readNumEntry("Borderwidth",4));
    dialog_->titlebarheight->setValue(config_->readNumEntry("Titlebarheight",20));
    
    int active=config_->readNumEntry("ActiveShade",40);
    dialog_->shade1->setValue(active);
    
    active=config_->readNumEntry("InactiveShade",40);
    dialog_->shade2->setValue(active);
    
    // Set the modus --> !!!! select new widget box !!!!
    dialog_->type1->setCurrentItem(config_->readNumEntry("ActiveMode",0));
    dialog_->type2->setCurrentItem(config_->readNumEntry("InactiveMode",0));

    updateStack(dialog_->stack1,dialog_->type1->currentItem());
    updateStack(dialog_->stack2,dialog_->type2->currentItem());
    
	color=QColor(255,255,255);
    dialog_->buttonColor->setColor(config_->readColorEntry("ButtonColor",&color));
    dialog_->roundCorners->setChecked(config_->readBoolEntry("RoundCorners",false));
	
	dialog_->hover->setChecked(config_->readBoolEntry("HoverEffect",false));
	
	dialog_->buttonTheme->setCurrentItem(config_->readNumEntry("ButtonTheme",0));
	
	button=(QRadioButton*)dialog_->repaintMode->find(config_->readNumEntry("RepaintMode",2));
	if (button)button->setChecked(true);
    dialog_->updateTime->setValue(config_->readNumEntry("RepaintTime",200));
}

//////////////////////////////////////////////////////////////////////////////
// save()
// ------
// Save configuration data

void ExampleConfig::save(KConfig*)
{
    config_->setGroup("General");

    QRadioButton *button = (QRadioButton*)dialog_->titlealign->selected();
    if (button) config_->writeEntry("TitleAlignment", QString(button->name()));
    config_->writeEntry("TextShadow",dialog_->textshadow->isChecked());
    config_->writeEntry("TrackDesktop",dialog_->trackdesktop->isChecked());
    
    config_->writeEntry("Borderwidth",dialog_->borderwidth->value());
    config_->writeEntry("Titlebarheight",dialog_->titlebarheight->value());
    
    config_->writeEntry("ActiveShade",dialog_->shade1->value());
    config_->writeEntry("InactiveShade",dialog_->shade2->value());
    config_->writeEntry("ActiveFrame",dialog_->frame1->isChecked());
    config_->writeEntry("FrameColor1",dialog_->frameColor1->color());
    config_->writeEntry("InactiveFrame",dialog_->frame2->isChecked());
    config_->writeEntry("ActiveMode",dialog_->type1->currentItem());
    config_->writeEntry("InactiveMode",dialog_->type2->currentItem());
    config_->writeEntry("FrameColor2",dialog_->frameColor2->color());

    config_->writeEntry("ButtonColor",dialog_->buttonColor->color());
    config_->writeEntry("RoundCorners",dialog_->roundCorners->isChecked());
	
	config_->writeEntry("HoverEffect",dialog_->hover->isChecked());
	
	config_->writeEntry("ButtonTheme",dialog_->buttonTheme->currentItem());
	config_->writeEntry("RepaintMode",dialog_->repaintMode->selectedId());
    config_->writeEntry("RepaintTime",dialog_->updateTime->value());
        
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
    dialog_->shade1->setValue(50);
    dialog_->shade2->setValue(50);
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
