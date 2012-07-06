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

#include "crystalconfig.h"
#include "configdialog.h"

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
    
    connect(dialog_->shade1, SIGNAL(valueChanged(int)),
            this, SLOT(selectionChanged(int)));
    connect(dialog_->shade2, SIGNAL(valueChanged(int)),
            this, SLOT(selectionChanged(int)));

    connect(dialog_->frame1, SIGNAL(stateChanged(int)),
            this, SLOT(selectionChanged(int)));
    connect(dialog_->frame2, SIGNAL(stateChanged(int)),
            this, SLOT(selectionChanged(int)));
	    
    connect(dialog_->type1,SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->type2,SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));

    connect(dialog_->borderwidth, SIGNAL(valueChanged(int)),
            this, SLOT(selectionChanged(int)));
    connect(dialog_->titlebarheight, SIGNAL(valueChanged(int)),
            this, SLOT(selectionChanged(int)));
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
    config_->setGroup("General");
    
    QString value = config_->readEntry("TitleAlignment", "AlignHCenter");
    QRadioButton *button = (QRadioButton*)dialog_->titlealign->child(value);
    if (button) button->setChecked(true);
    
    dialog_->textshadow->setChecked(config_->readBoolEntry("TextShadow",true));
    
    dialog_->frame1->setChecked(config_->readBoolEntry("ActiveFrame",true));
    dialog_->frame2->setChecked(config_->readBoolEntry("InactiveFrame",true));
    
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
    config_->writeEntry("Borderwidth",dialog_->borderwidth->value());
    config_->writeEntry("Titlebarheight",dialog_->titlebarheight->value());
    
    config_->writeEntry("ActiveShade",dialog_->shade1->value());
    config_->writeEntry("InactiveShade",dialog_->shade2->value());
    config_->writeEntry("ActiveFrame",dialog_->frame1->isChecked());
    config_->writeEntry("InactiveFrame",dialog_->frame2->isChecked());
    config_->writeEntry("ActiveMode",dialog_->type1->currentItem());
    config_->writeEntry("InactiveMode",dialog_->type2->currentItem());
    
    config_->sync();
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
