//
// C++ Interface: buttonimage
//
// Description: 
//
//
// Author: Sascha Hlusiak <Spam84@gmx.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//


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
	
	QImage *animated;
	const QRgb* normal_data;
	QRgb* hovered_data;
	QRgb* animated_data;
	
	ButtonImage(const QRgb *d_normal=NULL,bool blend=true,QColor color=::factory->buttonColor,int w=DEFAULT_IMAGE_SIZE,int h=DEFAULT_IMAGE_SIZE);
	virtual ~ButtonImage();
	
	void SetNormal(const QRgb *d_normal,int w=DEFAULT_IMAGE_SIZE,int h=DEFAULT_IMAGE_SIZE,bool blend=true,QColor color=::factory->buttonColor);
	void SetHovered(const QRgb *d_hovered=NULL,bool blend=true,QColor color=::factory->buttonColor);
	void SetPressed(const QRgb *d_pressed=NULL,bool blend=true,QColor color=::factory->buttonColor);
	void reset();

	QImage* getAnimated(float anim);
	
private:
	QImage CreateImage(const QRgb *data,bool blend,QColor color);
};



#endif
