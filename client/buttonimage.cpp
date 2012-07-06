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





ButtonImage::ButtonImage(const QRgb *d_normal,int w,int h)
{ 
	normal=hovered=pressed=animated=NULL;
	image_width=w;
	image_height=h;
	normal_data=hovered_data=animated_data=pressed_data=NULL;
	normal_color=hovered_color=pressed_color=QColor(255,255,255);
	reset();
	if (d_normal)SetNormal(d_normal,w,h);
}

ButtonImage::~ButtonImage()
{
	if (normal)delete normal;
	if (hovered)delete hovered;
	if (pressed)delete pressed;
	if (animated)delete animated;
	if (animated_data)delete[] animated_data;
	if (pressed_data)delete[] pressed_data;
	if (hovered_data)delete[] hovered_data;
	if (normal_data)delete[] normal_data;
}

QImage ButtonImage::CreateImage(QRgb *data,QColor color)
{
	tint(data,color);
	QImage img=QImage((uchar*)data,image_width,image_height,32,NULL,0,QImage::LittleEndian),img2;
	img.setAlphaBuffer(true);
	
	return img;
}

void ButtonImage::reset()
{
	if (normal)delete normal;
	if (hovered)delete hovered;
	if (pressed)delete pressed;
	if (animated)delete animated;
	normal=hovered=pressed=animated=NULL;

	if (normal_data)delete[] normal_data;
	if (hovered_data)delete[] hovered_data;
	if (pressed_data)delete[] pressed_data;
	if (animated_data)delete[] animated_data;
	normal_data=hovered_data=animated_data=pressed_data=NULL;
	org_normal_data=org_hovered_data=NULL;

	hSpace=vSpace=2;
	drawMode=0;
}

void ButtonImage::SetNormal(const QRgb *d_normal,int w,int h)
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
	if (pressed_data)delete[] pressed_data;
	pressed_data=NULL;
	if (normal_data)delete[] normal_data;

	org_normal_data=d_normal;
	normal_data=new QRgb[image_width*image_height];
	memcpy(normal_data,d_normal,sizeof(QRgb)*image_width*image_height);
	normal=new QImage(CreateImage(normal_data,normal_color));
}

void ButtonImage::SetHovered(const QRgb *d_hovered)
{
	if (hovered)delete hovered;
	if (hovered_data)delete[] hovered_data;
	if (d_hovered)
	{
		org_hovered_data=d_hovered;
		hovered_data=new QRgb[image_width*image_height];
		memcpy(hovered_data,d_hovered,sizeof(QRgb)*image_width*image_height);
		hovered=new QImage(CreateImage(hovered_data,hovered_color));
	}else{
		hovered=NULL;
		hovered_data=NULL;
		org_hovered_data=NULL;
	}
}

void ButtonImage::SetPressed(const QRgb *d_pressed)
{
	if (pressed)delete pressed;
	if (pressed_data)delete[] pressed_data;
	if (d_pressed)
	{
		pressed_data=new QRgb[image_width*image_height];
		memcpy(pressed_data,d_pressed,sizeof(QRgb)*image_width*image_height);
		pressed=new QImage(CreateImage(pressed_data,pressed_color));
	}else{
		pressed=NULL;
		pressed_data=NULL;
	}
}

void ButtonImage::finish()
{
	if (!org_normal_data)
	{
		printf("ERROR: No org_normal_data set!\n");
		return;
	}

	if (!hovered_data){
		hovered_data=new QRgb[image_width*image_height];
		float faktor=::factory->hovereffect?0.5:1.0;
		for (int i=0;i<image_width*image_height;i++)
		{
			hovered_data[i]=qRgba(qRed(org_normal_data[i]),qGreen(org_normal_data[i]),qBlue(org_normal_data[i]),
				(int)(255.0*pow((float)qAlpha(org_normal_data[i])/255.0,faktor)));
		}
		hovered=new QImage(CreateImage(hovered_data,hovered_color));
	}

	if (!pressed_data){
		float faktor=::factory->hovereffect?0.5:0.4;
		pressed_data=new QRgb[image_width*image_height];
		if (!org_hovered_data)
		{
			org_hovered_data=hovered_data;
		}

		for (int i=0;i<image_width*image_height;i++)
		{
			pressed_data[i]=qRgba(qRed(org_hovered_data[i]),qGreen(org_hovered_data[i]),qBlue(org_hovered_data[i]),
				(int)(255.0*pow((float)qAlpha(org_hovered_data[i])/255.0,faktor)));
		}
		pressed=new QImage(CreateImage(pressed_data,pressed_color));
	}

	if (!animated_data)animated_data=new QRgb[image_width*image_height];
	if (!animated)
	{
		animated=new QImage((uchar*)animated_data,image_width,image_height,32,NULL,0,QImage::LittleEndian);
		animated->setAlphaBuffer(true);
	}
}

QImage* ButtonImage::getAnimated( float anim)
{
	if (!normal_data)return NULL;
	if (!animated_data)return NULL;

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

void ButtonImage::tint(QRgb *data,QColor color)
{
	float f_r=(float)color.red()/255.0;
	float f_g=(float)color.green()/255.0;
	float f_b=(float)color.blue()/255.0;
	for (int i=0;i<image_width*image_height;i++)
	{
		float r=(float)qRed(data[i])/255.0;
		float g=(float)qGreen(data[i])/255.0;
		float b=(float)qBlue(data[i])/255.0;

		r*=f_r;
		g*=f_g;
		b*=f_b;

		data[i]=qRgba(
			(int)(r*255.0),
			(int)(g*255.0),
			(int)(b*255.0),
			qAlpha(data[i]));
	}
}
