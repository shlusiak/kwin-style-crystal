/***************************************************************************
 *   buttonimage.h                                                         *
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



#ifndef _BUTTON_IMAGE_INCLUDED_
#define _BUTTON_IMAGE_INCLUDED_

#include <qimage.h>
#include "crystalclient.h"

#define DEFAULT_IMAGE_SIZE 14


class ButtonImage
{
public:
	QImage *normal,*hovered,*pressed;
	int image_width,image_height;
	int hSpace,vSpace;
	int drawMode;
	QColor normal_color,hovered_color,pressed_color;
	
	QImage *animated;
	QRgb *normal_data,*hovered_data,*animated_data,*pressed_data;
	QRgb *org_normal_data,*org_hovered_data;
	
	ButtonImage(const QRgb *d_normal=NULL,int w=DEFAULT_IMAGE_SIZE,int h=DEFAULT_IMAGE_SIZE);
	virtual ~ButtonImage();
	
	void SetNormal(const QRgb *d_normal,int w=DEFAULT_IMAGE_SIZE,int h=DEFAULT_IMAGE_SIZE);
	void SetHovered(const QRgb *d_hovered=NULL);
	void SetPressed(const QRgb *d_pressed=NULL);
	void reset();
	void finish();
	bool initialized();

	void setSpace(int hS,int vS) { hSpace=hS; vSpace=vS; }
	void setDrawMode(int dm) { drawMode=dm; }
	void setColors(QColor n,QColor h,QColor p) { normal_color=n; hovered_color=h; pressed_color=p; }

	QImage* getAnimated(float anim);
	
private:
	QImage CreateImage(QRgb *data,QColor color);
	void tint(QRgb *data,QColor color);
};

#endif
