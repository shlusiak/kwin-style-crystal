//
// C++ Interface: buttonimage
//
// Description: 
//
//
// Author: Sascha Hlusiak <Spam84@gmx.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution



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
	const QRgb *org_normal_data,*org_hovered_data;
	
	ButtonImage(const QRgb *d_normal=NULL,int w=DEFAULT_IMAGE_SIZE,int h=DEFAULT_IMAGE_SIZE);
	virtual ~ButtonImage();
	
	void SetNormal(const QRgb *d_normal,int w=DEFAULT_IMAGE_SIZE,int h=DEFAULT_IMAGE_SIZE);
	void SetHovered(const QRgb *d_hovered=NULL);
	void SetPressed(const QRgb *d_pressed=NULL);
	void reset();
	void finish();

	void setSpace(int hS,int vS) { hSpace=hS; vSpace=vS; }
	void setDrawMode(int dm) { drawMode=dm; }
	void setColors(QColor n,QColor h,QColor p) { normal_color=n; hovered_color=h; pressed_color=p; }

	QImage* getAnimated(float anim);
	
private:
	QImage CreateImage(QRgb *data,QColor color);
	void tint(QRgb *data,QColor color);
};



#endif
