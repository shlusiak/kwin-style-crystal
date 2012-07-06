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

#include "crystalconfig.h"
#include "configdialog.h"

//////////////////////////////////////////////////////////////////////////////
// ExampleConfig()
// -------------
// Constructor

ExampleConfig::ExampleConfig(KConfig* config, QWidget* parent)
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
    connect(dialog_->shadeactive, SIGNAL(valueChanged(int)),
            this, SLOT(selectionChanged(int)));
    connect(dialog_->shadeinactive, SIGNAL(valueChanged(int)),
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

void ExampleConfig::selectionChanged(int)
{
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
    
    int active=config_->readNumEntry("ActiveShade",0);
    dialog_->shadeactive->setValue(active);
    
    active=config_->readNumEntry("InactiveShade",0);
    dialog_->shadeinactive->setValue(active);
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
    config_->writeEntry("ActiveShade",dialog_->shadeactive->value());
    config_->writeEntry("InactiveShade",dialog_->shadeinactive->value());
    
    
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
    dialog_->shadeactive->setValue(50);
    dialog_->shadeinactive->setValue(50);
    
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
