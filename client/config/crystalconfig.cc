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
//     KGlobal::locale()->insertCatalogue("kwin_crystal_config");

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
	connect(dialog_->tooltip,SIGNAL(stateChanged(int)),
			this,SLOT(selectionChanged(int)));
	connect(dialog_->wheelTask,SIGNAL(stateChanged(int)),
			this,SLOT(selectionChanged(int)));

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

    connect(dialog_->inline1, SIGNAL(stateChanged(int)),
            this, SLOT(selectionChanged(int)));
    connect(dialog_->inline2, SIGNAL(stateChanged(int)),
            this, SLOT(selectionChanged(int)));
    connect(dialog_->inlineColor1, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
    connect(dialog_->inlineColor2, SIGNAL(changed(const QColor&)),this,SLOT(colorChanged(const QColor&)));
	    
    connect(dialog_->type1,SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->type2,SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));

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

    connect(dialog_->repaintMode, SIGNAL(clicked(int)),
            this, SLOT(selectionChanged(int)));

    connect(dialog_->updateTime, SIGNAL(valueChanged(int)),
            this, SLOT(selectionChanged(int)));
	
	connect(dialog_->infoButton, SIGNAL(clicked(void)),this,SLOT(infoDialog(void)));

    connect(dialog_->active_blur, SIGNAL(valueChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->inactive_blur, SIGNAL(valueChanged(int)),this,SLOT(selectionChanged(int)));


    connect(dialog_->overlay_active, SIGNAL(clicked(int)),this, SLOT(selectionChanged(int)));
    connect(dialog_->overlay_inactive, SIGNAL(clicked(int)),this, SLOT(selectionChanged(int)));

	connect(dialog_->overlay_active_button,SIGNAL(clicked(void)),this,SLOT(loadOverlayActive(void)));
	connect(dialog_->overlay_inactive_button,SIGNAL(clicked(void)),this,SLOT(loadOverlayInactive(void)));

	connect(dialog_->overlay_active_file,SIGNAL(textChanged(const QString&)),this,SLOT(textChanged(const QString &)));
	connect(dialog_->overlay_inactive_file,SIGNAL(textChanged(const QString&)),this,SLOT(textChanged(const QString &)));


    connect(dialog_->logoEnabled, SIGNAL(clicked(int)),
            this, SLOT(selectionChanged(int)));
    connect(dialog_->logoFile, SIGNAL(textChanged(const QString &)),
            this, SLOT(logoTextChanged(const QString&)));
    connect(dialog_->logoStretch, SIGNAL(activated(int)),
            this, SLOT(selectionChanged(int)));
    connect(dialog_->logoActive, SIGNAL(stateChanged(int)),
            this, SLOT(selectionChanged(int)));
	connect(dialog_->logoButton,SIGNAL(clicked(void)),this,SLOT(loadLogo(void)));
	connect(dialog_->logoDistance,SIGNAL(valueChanged(int)),this,SLOT(selectionChanged(int)));
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
    QColor color(255,255,255);

	config_->setGroup("General");
    
    QString value = config_->readEntry("TitleAlignment", "AlignHCenter");
    QRadioButton *button = (QRadioButton*)dialog_->titlealign->child(value);
    if (button) button->setChecked(true);
    
    dialog_->textshadow->setChecked(config_->readBoolEntry("TextShadow",true));
	dialog_->tooltip->setChecked(config_->readBoolEntry("CaptionTooltip",true));
	dialog_->wheelTask->setChecked(config_->readBoolEntry("WheelTask",true));

    dialog_->trackdesktop->setChecked(config_->readBoolEntry("TrackDesktop",false));
    
    dialog_->frame1->setChecked(config_->readBoolEntry("ActiveFrame",true));
	color=QColor(192,192,192);
    dialog_->frameColor1->setColor(config_->readColorEntry("FrameColor1",&color));
    dialog_->frame2->setChecked(config_->readBoolEntry("InactiveFrame",true));
	color=QColor(192,192,192);
    dialog_->frameColor2->setColor(config_->readColorEntry("FrameColor2",&color));

    dialog_->inline1->setChecked(config_->readBoolEntry("ActiveInline",false));
	color=QColor(192,192,192);
    dialog_->inlineColor1->setColor(config_->readColorEntry("InlineColor1",&color));
    dialog_->inline2->setChecked(config_->readBoolEntry("InactiveInline",false));
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
	
    dialog_->updateTime->setValue(config_->readNumEntry("RepaintTime",200));
	button=(QRadioButton*)dialog_->repaintMode->find(config_->readNumEntry("RepaintMode",1));
	if (button)button->setChecked(true);

	dialog_->active_blur->setValue(config_->readNumEntry("ActiveBlur",0));
	dialog_->inactive_blur->setValue(config_->readNumEntry("InactiveBlur",0));

	button=(QRadioButton*)dialog_->overlay_active->find(config_->readNumEntry("OverlayModeActive",0));
	if (button)button->setChecked(true);
    dialog_->overlay_active_file->setText(config_->readEntry("OverlayFileActive",""));

	button=(QRadioButton*)dialog_->overlay_inactive->find(config_->readNumEntry("OverlayModeInactive",0));
	if (button)button->setChecked(true);
    dialog_->overlay_inactive_file->setText(config_->readEntry("OverlayFileInactive",""));

	dialog_->logoEnabled->setButton(config_->readNumEntry("LogoAlignment",1));
	dialog_->logoFile->setText(config_->readEntry("LogoFile",""));
	dialog_->logoActive->setChecked(config_->readBoolEntry("LogoActive",1));
	dialog_->logoStretch->setCurrentItem(config_->readNumEntry("LogoStretch",0));
	dialog_->logoDistance->setValue(config_->readNumEntry("LogoDistance",0));
	updateLogo();
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
	config_->writeEntry("CaptionTooltip",dialog_->tooltip->isChecked());
	config_->writeEntry("WheelTask",dialog_->wheelTask->isChecked());
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

    config_->writeEntry("ActiveInline",dialog_->inline1->isChecked());
    config_->writeEntry("InlineColor1",dialog_->inlineColor1->color());
    config_->writeEntry("InactiveInline",dialog_->inline2->isChecked());
    config_->writeEntry("InlineColor2",dialog_->inlineColor2->color());

    config_->writeEntry("ButtonColor",dialog_->buttonColor->color());
	
    int cornersFlag = 0;
    if(dialog_->tlc->isChecked()) cornersFlag += TOP_LEFT;
    if(dialog_->trc->isChecked()) cornersFlag += TOP_RIGHT;
    if(dialog_->blc->isChecked()) cornersFlag += BOT_LEFT;
    if(dialog_->brc->isChecked()) cornersFlag += BOT_RIGHT;
    config_->writeEntry("RoundCorners", cornersFlag );
	
	config_->writeEntry("HoverEffect",dialog_->hover->isChecked());
	config_->writeEntry("TintButtons",dialog_->tintButtons->isChecked());
	
	config_->writeEntry("ButtonTheme",dialog_->buttonTheme->currentItem());
	config_->writeEntry("RepaintMode",dialog_->repaintMode->selectedId());
    config_->writeEntry("RepaintTime",dialog_->updateTime->value());

	config_->writeEntry("ActiveBlur",dialog_->active_blur->value());
	config_->writeEntry("InactiveBlur",dialog_->inactive_blur->value());

	config_->writeEntry("OverlayModeActive",dialog_->overlay_active->selectedId());
	config_->writeEntry("OverlayFileActive",dialog_->overlay_active_file->text());
	config_->writeEntry("OverlayModeInactive",dialog_->overlay_inactive->selectedId());
	config_->writeEntry("OverlayFileInactive",dialog_->overlay_inactive_file->text());

	config_->writeEntry("LogoAlignment",dialog_->logoEnabled->selectedId());
	config_->writeEntry("LogoFile",dialog_->logoFile->text());
	config_->writeEntry("LogoActive",dialog_->logoActive->isChecked());
	config_->writeEntry("LogoStretch",dialog_->logoStretch->currentItem());
	config_->writeEntry("LogoDistance",dialog_->logoDistance->value());

    config_->sync();
}

void ExampleConfig::infoDialog()
{
	InfoDialog d(dialog_);
	d.exec();
}

void ExampleConfig::loadOverlayActive()
{
	KURL s=KFileDialog::getImageOpenURL();
    if (!s.isEmpty())
	{
		dialog_->overlay_active_file->setText( s.path() );
		dialog_->overlay_active_userdefined->setChecked(true);
        emit changed();
    }
}

void ExampleConfig::loadOverlayInactive()
{
	KURL s=KFileDialog::getImageOpenURL();
    if (!s.isEmpty())
	{
		dialog_->overlay_inactive_file->setText( s.path() );
		dialog_->overlay_inactive_userdefined->setChecked(true);
        emit changed();
    }
}

void ExampleConfig::loadLogo()
{
	KURL s=KFileDialog::getImageOpenURL();
    if (!s.isEmpty())
	{
		dialog_->logoFile->setText( s.path() );
        emit changed();
		updateLogo();
    }
}

void ExampleConfig::logoTextChanged(const QString&)
{
	updateLogo();
	emit changed();
}

void ExampleConfig::updateLogo()
{
	QPixmap pic;
	pic.load(dialog_->logoFile->text());
	dialog_->logoPreview->setPixmap(pic);
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
