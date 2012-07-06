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


#include <kapp.h>
#include <qimage.h>
#include <kimageeffect.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include "imageholder.h"
#include "crystalclient.h"


QImageHolder::QImageHolder(QImage act,QImage inact)
:img_active(NULL),img_inactive(NULL)
{
	rootpixmap=NULL;
	setUserdefinedPictures( act,inact);
	initialized=userdefinedActive && userdefinedInactive;

	emit repaintNeeded();
}

QImageHolder::~QImageHolder()
{
	if (rootpixmap)delete rootpixmap;
	if (img_active && !userdefinedActive)delete img_active;
	if (img_inactive && !userdefinedInactive)delete img_inactive;
}

void QImageHolder::setUserdefinedPictures( QImage act,QImage inact)
{
	int w=QApplication::desktop()->width();
	int h=QApplication::desktop()->height();
	if (img_active && !userdefinedActive)
	{
		delete img_active;	
		img_active=NULL;
	}
	if (img_inactive && !userdefinedInactive)
	{
		delete img_inactive;
		img_inactive=NULL;
	}

	if (!act.isNull())
	{
		act=act.smoothScale(w,h);
		img_active=ApplyEffect(act,&::factory->active,factory->options()->colorGroup(KDecoration::ColorTitleBar, true)); 
	}else img_active=NULL;
	if (!inact.isNull())
	{
		inact=inact.smoothScale(w,h);
		img_inactive=ApplyEffect(inact,&::factory->inactive,factory->options()->colorGroup(KDecoration::ColorTitleBar, false)); 
	}else img_inactive=NULL;

	userdefinedActive=(img_active!=NULL);
	userdefinedInactive=(img_inactive!=NULL);

	CheckSanity();
}

void QImageHolder::Init()
{
	if (initialized)return;
	
	rootpixmap=new KMyRootPixmap(NULL/*,this*/);
	rootpixmap->start();
	rootpixmap->repaint(true);
	connect( rootpixmap,SIGNAL(backgroundUpdated(const QImage*)),this, SLOT(BackgroundUpdated(const QImage*)));
	connect(kapp, SIGNAL(backgroundChanged(int)),SLOT(handleDesktopChanged(int)));
	
	initialized=true;
}

void QImageHolder::repaint(bool force)
{
	Init(); 
	if (rootpixmap)rootpixmap->repaint(force);
}

void QImageHolder::handleDesktopChanged(int)
{
	repaint(true);
}

void QImageHolder::CheckSanity()
{
	if (!initialized)return;
	if (userdefinedActive && userdefinedInactive)return;
	if (img_active!=NULL && !userdefinedActive)return;
	if (img_inactive!=NULL && !userdefinedInactive)return;

	if (rootpixmap)delete rootpixmap;
	rootpixmap=NULL;
	
	initialized=false;
}

QPixmap* QImageHolder::ApplyEffect(QImage &src,WND_CONFIG* cfg,QColorGroup colorgroup)
{
	QImage dst;
	
	switch(cfg->mode)
	{
	case 0:	if (cfg->amount>0.99)return new QPixmap();
		dst=KImageEffect::fade(src, cfg->amount, colorgroup.background());
		break;
	case 1:dst=KImageEffect::channelIntensity(src,cfg->amount,KImageEffect::All);
		break;
	case 2:dst=KImageEffect::intensity(src,cfg->amount);
		break;
	case 3:dst=KImageEffect::desaturate(src,cfg->amount);
		break;
	case 4: dst=src;
		KImageEffect::solarize(dst,cfg->amount*100.0);
		break;

	default:dst=src;
		break;	
	}

	if (cfg->blur>0)dst=KImageEffect::blur(dst,0,cfg->blur);
	
	return new QPixmap(dst);
}

void QImageHolder::BackgroundUpdated(const QImage *src)
{
	if (img_active && !userdefinedActive)
	{
		delete img_active;
		img_active=NULL;
	}
	if (img_inactive && !userdefinedInactive)
	{
		delete img_inactive;
		img_inactive=NULL;
	}
	
	if (src && !src->isNull())
	{
		QImage tmp=src->copy();

		if (!userdefinedInactive)
			img_inactive=ApplyEffect(tmp,&::factory->inactive,factory->options()->colorGroup(KDecoration::ColorTitleBar, false));

		tmp=src->copy();		
		if (!userdefinedActive)
			img_active=ApplyEffect(tmp,&::factory->active,factory->options()->colorGroup(KDecoration::ColorTitleBar, true));
	}
	
	emit repaintNeeded();
}
