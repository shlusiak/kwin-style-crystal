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


#ifndef _IMAGEHOLDER_INCLUDED_
#define _IMAGEHOLDER_INCLUDED_


#include <kwinmodule.h>
#include "myrootpixmap.h"

struct WND_CONFIG;

class QImageHolder:public QObject
{
	Q_OBJECT
public:
	QImageHolder(QImage act,QImage inact);
	virtual ~QImageHolder();
	
	void Init();
	QPixmap *image(bool active) { Init(); return active?img_active:img_inactive; }
	void repaint(bool force);

	void setUserdefinedPictures(QImage act,QImage inact);

private:
	bool initialized;
	KMyRootPixmap *rootpixmap;
	QPixmap *img_active,*img_inactive;
	bool userdefinedActive,userdefinedInactive;

	QPixmap* ApplyEffect(QImage &src,WND_CONFIG* cfg,QColorGroup colorgroup);
	
public slots:
	void BackgroundUpdated(const QImage *);
	void handleDesktopChanged(int desk);
	void CheckSanity();
	
signals:
	void repaintNeeded();
};


#endif
