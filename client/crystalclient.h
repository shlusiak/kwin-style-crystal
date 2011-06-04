/***************************************************************************
 *   crystalclient.h                                                     *
 *   -----------------------                                               *
 *   Copyright (C) 2006-2011 by Sascha Hlusiak                             *
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


#ifndef CRYSTALCLIENT_H
#define CRYSTALCLIENT_H

#include <qlayout.h>
#include <kdecoration.h>
#include <kcommondecoration.h>
#include <kdecorationfactory.h>
#include <qtimer.h>
#include <qlist.h>
#include <X11/Xlib.h>

#include "common.h"


class QSpacerItem;
class QPoint;

class CrystalClient;
class CrystalFactory;
class CrystalButton;
class ButtonImage;



class CrystalClient : public KDecoration
{
	Q_OBJECT
public:
	CrystalClient(KDecorationBridge *b,CrystalFactory *f);
	virtual ~CrystalClient();
	
	virtual void init();
	
	virtual void activeChange();
	virtual void desktopChange();
	virtual void captionChange();
	virtual void iconChange();
	virtual void maximizeChange();
	virtual void shadeChange();
	
	virtual void borders(int &l, int &r, int &t, int &b) const;
	virtual void resize(const QSize &size);
	virtual QSize minimumSize() const;
	virtual Position mousePosition(const QPoint &point) const;
	
private:
	CrystalButton* addButtons(QBoxLayout* layout, const QString& buttons);
	void updateMask();
	int borderSpacing();
	void updateLayout();
	
	bool eventFilter(QObject *obj, QEvent *e);
	void mouseDoubleClickEvent(QMouseEvent *e);
	void paintEvent(QPaintEvent *e);
	void resizeEvent(QResizeEvent *);
	void moveEvent(QMoveEvent *);
	void showEvent(QShowEvent *);
	void mouseWheelEvent(QWheelEvent *e);
	
private slots:
	void Repaint();
	void maxButtonPressed();
	void minButtonPressed();
	void shadeButtonPressed();
	void aboveButtonPressed();
	void belowButtonPressed();
	void menuButtonPressed();
	void closeButtonPressed();
	
	void keepAboveChange( bool );
	void keepBelowChange( bool );
	void menuPopUp();
private:
	CrystalButton *button[NumButtons];
	QGridLayout *mainlayout;
	QHBoxLayout *titlelayout;
public:
	bool FullMax;
	QSpacerItem *titlebar_;
};

#endif
