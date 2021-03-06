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


#ifndef CRYSTALCONFIG_H
#define CRYSTALCONFIG_H

#define TOP_LEFT 1
#define TOP_RIGHT 2
#define BOT_LEFT 4
#define BOT_RIGHT 8

#include <QObject>
#include <kconfig.h>
#include <kurlrequester.h>
#include <kurllabel.h>
#include <klocale.h>

#include "ui_configdialog.h"
#include "ui_infodialog.h"


class ConfigDialog : public QWidget, public Ui::ConfigDialog
{
public:
  ConfigDialog( QWidget *parent ) : QWidget( parent ) {
    setupUi( this );
  }
};

class InfoDialog : public QDialog, public Ui::InfoDialog
{
public:
  InfoDialog( QWidget *parent ) : QDialog( parent ) {
    setupUi( this );
  }
};

class CrystalConfig : public QObject
{
	Q_OBJECT
public:
	CrystalConfig(KConfig* config, QWidget* parent);
	~CrystalConfig();
    
signals:
	void changed();

public slots:
	void load(KConfigGroup&);
	void save(KConfigGroup&);
	void infoDialog();

protected slots:
	void selectionChanged(int);
	void overlay_active_changed(int);
	void overlay_inactive_changed(int);
	void boolChanged(bool) { selectionChanged(0); }
	void colorChanged(const QColor&) { selectionChanged(0); }
	void textChanged(const QString&) { selectionChanged(0); }
	void logoTextChanged(const QString&);
	void logoIndexChanged(int);

private:
	KConfig *config_;
	ConfigDialog *dialog_;

	void updateLogo();
};

#endif // CRYSTALCONFIG_H
