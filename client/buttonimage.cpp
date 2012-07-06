//
// C++ Implementation: buttonimage
//
// Description: 
//
//
// Author: Sascha Hlusiak <Spam84@gmx.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <qimage.h>
#include <math.h>
#include <kimageeffect.h>

#include "buttonimage.h"





ButtonImage::ButtonImage(const QRgb *d_normal,bool blend,QColor color,int w,int h)
{ 
	normal=hovered=pressed=animated=NULL;
	image_width=w;
	image_height=h;
	normal_data=hovered_data=animated_data=NULL;
	if (d_normal)SetNormal(d_normal,w,h,blend,color);
}

ButtonImage::~ButtonImage()
{
	if (normal)delete normal;
	if (hovered)delete hovered;
	if (pressed)delete pressed;
	if (animated)delete animated;
	if (animated_data)delete[] animated_data;
	if (hovered_data)delete[] hovered_data;
}

QImage ButtonImage::CreateImage(const QRgb *data,bool blend,QColor color)
{
	QImage img=QImage((uchar*)data,image_width,image_height,32,NULL,0,QImage::LittleEndian),img2;
	img.setAlphaBuffer(true);
	
	if (!blend)return img;
	img2=img.copy();
	return KImageEffect::blend(color,img2,1.0f);
}

void ButtonImage::reset()
{
	if (normal)delete normal;
	if (hovered)delete hovered;
	if (pressed)delete pressed;
	if (animated)delete animated;
	normal=hovered=pressed=animated=NULL;

	if (hovered_data)delete[] hovered_data;
	if (animated_data)delete[] animated_data;
	normal_data=hovered_data=animated_data=NULL;
}

void ButtonImage::SetNormal(const QRgb *d_normal,int w,int h,bool blend,QColor color)
{
	image_width=w;
	image_height=h;
	if (normal)delete normal;
	if (animated)delete animated;
	animated=NULL;
	if (animated_data)delete[] animated_data;
	animated_data=NULL;
	if (hovered_data)delete[] hovered_data;
	hovered_data=NULL;

	normal_data=d_normal;
	normal=new QImage(CreateImage(d_normal,blend,color));
}

void ButtonImage::SetHovered(const QRgb *d_hovered,bool blend,QColor color)
{
	if (hovered)delete hovered;
	if (hovered_data)delete[] hovered_data;
	if (d_hovered)
	{
		hovered_data=new QRgb[image_width*image_height];
		memcpy(hovered_data,d_hovered,sizeof(QRgb)*image_width*image_height);
		hovered=new QImage(CreateImage(d_hovered,blend,color));
	}else{
		hovered=NULL;
		hovered_data=NULL;
	}
}

void ButtonImage::SetPressed(const QRgb *d_pressed,bool blend,QColor color)
{
	if (pressed)delete pressed;
	if (d_pressed)
	{
		pressed=new QImage(CreateImage(d_pressed,blend,color));
	}else{
		pressed=NULL;
	}
}

QImage* ButtonImage::getAnimated( float anim)
{
	if (!normal_data)return NULL;
	if (!animated_data)animated_data=new QRgb[image_width*image_height];
	if (!animated)
	{
		animated=new QImage((uchar*)animated_data,image_width,image_height,32,NULL,0,QImage::LittleEndian);
		animated->setAlphaBuffer(true);
	}	

	if (!hovered_data){
		hovered_data=new QRgb[image_width*image_height];
		for (int i=0;i<image_width*image_height;i++)
		{
			hovered_data[i]=qRgba(qRed(normal_data[i]),qGreen(normal_data[i]),qBlue(normal_data[i]),
				(int)(255.0*pow((float)qAlpha(normal_data[i])/255.0,0.50)));
		}
	}

	for (int i=0;i<image_width*image_height;i++)
	{
		float r1=(float)qRed(normal_data[i])/255.0;
		float r2=(float)qRed(hovered_data[i])/255.0;
		float g1=(float)qGreen(normal_data[i])/255.0;
		float g2=(float)qGreen(hovered_data[i])/255.0;
		float b1=(float)qBlue(normal_data[i])/255.0;
		float b2=(float)qBlue(hovered_data[i])/255.0;
		float a1=(float)qAlpha(normal_data[i])/255.0;
		float a2=(float)qAlpha(hovered_data[i])/255.0;
		
		animated_data[i]=qRgba(
			(int)((r1*(1.0-anim)+r2*anim)*255.0),
			(int)((g1*(1.0-anim)+g2*anim)*255.0),
			(int)((b1*(1.0-anim)+b2*anim)*255.0),
			(int)((a1*(1.0-anim)+a2*anim)*255.0));
	}
	
	return animated;
}

