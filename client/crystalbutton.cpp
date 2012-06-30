/***************************************************************************
 *   Copyright (C) 2006-2009 by Sascha Hlusiak                             *
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

#include <qwidget.h>
#include <qimage.h>
#include <qtooltip.h>
#include <qpainter.h>
#include <qrect.h>

#include "crystalfactory.h"
#include "crystalclient.h"
#include "crystalbutton.h"
#include "buttonimage.h"

CrystalButton::CrystalButton(CrystalClient *parent, const char */*name*/,
				const QString& tip, ButtonType type,
				ButtonImage *vimage)
: QAbstractButton(parent->widget()), client_(parent), type_(type), image(vimage), lastmouse_(0)
{
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_OpaquePaintEvent, false);
	setAutoFillBackground(false);
	resetSize(false);
	setCursor(Qt::ArrowCursor);
	
	hover=first=last=false;
	animation=0.0;
	setToolTip(tip);
	connect ( &animation_timer,SIGNAL(timeout()),this,SLOT(animate()));
}

CrystalButton::~CrystalButton()
{
}

void CrystalButton::resetSize(bool FullSize)
{
	if (FullSize || (image && image->drawMode==1))
	{
		setFixedSize(buttonSizeH(),factory->titlesize);
	} else setFixedSize(buttonSizeH(),buttonSizeV());
}

void CrystalButton::setBitmap(ButtonImage *newimage)
{
	image=newimage;
	repaint();
}

QSize CrystalButton::sizeHint() const
{
	return QSize(buttonSizeH(),buttonSizeV());
}

int CrystalButton::buttonSizeH() const
{
	int w=image?image->image_width:DEFAULT_IMAGE_SIZE;
	int h=image?image->image_height:DEFAULT_IMAGE_SIZE;
	int vS=image?image->vSpace:2;
	int hS=image?image->hSpace:2;
	return (factory->titlesize-1-vS>=h)?
		w+hS*2:
		(int)(((float)buttonSizeV()/(float)h)*(float)w)+hS;
}

int CrystalButton::buttonSizeV() const
{
	int h=image?image->image_height:DEFAULT_IMAGE_SIZE;
	int vS=image?image->vSpace:2;
	return (factory->titlesize-1-vS>h)?h:factory->titlesize-1-vS;
}

void CrystalButton::enterEvent(QEvent *e)
{
	hover=true;
	if (factory->hovereffect)repaint();
	if (factory->animateHover)animation_timer.start(60);
	QAbstractButton::enterEvent(e);
}

void CrystalButton::leaveEvent(QEvent *e)
{
	hover=false;
	if (factory->hovereffect)repaint();
	if (factory->animateHover)animation_timer.start(80);
	QAbstractButton::leaveEvent(e);
}

void CrystalButton::mousePressEvent(QMouseEvent* e)
{
	lastmouse_ = e->button();
	Qt::MouseButton button;
	switch(e->button())
	{
	case Qt::LeftButton:
		button=Qt::LeftButton;
		break;
	case Qt::RightButton:
		if ((type_ == MaxButton) || (type_ == MinButton) || (type_ == MenuButton) || (type_ == CloseButton))
			button=Qt::LeftButton; else button=Qt::NoButton;
		break;
	case Qt::MidButton:
		if ((type_ == MaxButton) || (type_ == MinButton))
			button=Qt::LeftButton; else button=Qt::NoButton;
		break;

	default:button=Qt::NoButton;
		break;
	}
	QMouseEvent me(e->type(), e->pos(), e->globalPos(),button, e->buttons(), e->modifiers());
	QAbstractButton::mousePressEvent(&me);
}

void CrystalButton::mouseReleaseEvent(QMouseEvent* e)
{
	lastmouse_ = e->button();
	Qt::MouseButton button;
	switch(e->button())
	{
	case Qt::LeftButton:
		button=Qt::LeftButton;
		break;
	case Qt::RightButton:
		if ((type_ == MaxButton) || (type_ == MinButton) || (type_ == MenuButton) || (type_ == CloseButton))
			button=Qt::LeftButton; else button=Qt::NoButton;
		break;
	case Qt::MidButton:
		if ((type_ == MaxButton) || (type_ == MinButton))
			button=Qt::LeftButton; else button=Qt::NoButton;
		break;
	
	default:button=Qt::NoButton;
		break;
	}
	QMouseEvent me(e->type(), e->pos(), e->globalPos(), button, e->buttons(), e->modifiers());
	QAbstractButton::mouseReleaseEvent(&me);
}

void CrystalButton::paintEvent(QPaintEvent* /*event*/)
{
	QPainter p(this);
	drawButton(&p);
}

void CrystalButton::drawButton(QPainter *painter)
{
	if (!CrystalFactory::initialized()) return;

	float dx, dy;
	int dm=0;

// 	QPixmap pufferPixmap(width(), height());
// 	QPainter pufferPainter(&pufferPixmap);
	
	CrystalFactory *f=((CrystalFactory*)client_->factory());
	WND_CONFIG *wndcfg=client_->isActive()?&f->active:&f->inactive;

	if (! client_->compositingActive()) {
		QColor color = client_->options()->color(KDecoration::ColorTitleBar, client_->isActive());
// 		color.setAlpha(96);
		painter->fillRect(rect(), color);

		if (!wndcfg->overlay.isNull())
		{
			if (wndcfg->stretch_overlay == false)
				painter->drawTiledPixmap(rect(),wndcfg->overlay,QPoint(x(),y()));
			else
			{
				QRectF src(
					(float)x() * (float)wndcfg->overlay.width() / (float)client_->widget()->width(),
					(float)y(),
					(float)width() * (float)wndcfg->overlay.width() / (float)client_->widget()->width(),
					(float)height()
				);
				painter->drawPixmap(QRect(0,0,width(),height()), wndcfg->overlay, src);
			}
		}
	}

	painter->setClipRect(this->rect());

	dm=0;
	if (image && (image->drawMode==1))dm=1;
	if (wndcfg->outlineMode)
	{
		// outline the frame
		painter->setPen(wndcfg->frameColor);

		if (wndcfg->outlineMode==2)painter->setPen(wndcfg->frameColor.dark(150));
		if (wndcfg->outlineMode==3)painter->setPen(wndcfg->frameColor.light(150));
		// top
		if ((client_->FullMax && client_->isShade() && (dm==0)) ||
			((dm==1)&&(!client_->FullMax || client_->isShade()))) painter->drawLine(0,0,width(),0);
		// left
		if (first && client_->FullMax && client_->isShade()) painter->drawLine(0,0,0,height());

		if (wndcfg->outlineMode==2)painter->setPen(wndcfg->frameColor.light(150));
		if (wndcfg->outlineMode==3)painter->setPen(wndcfg->frameColor.dark(150));
		// bottom
		if (client_->isShade() && ((dm==1)||(client_->FullMax))) painter->drawLine(0,height()-1,width(),height()-1);
		
		// right
		if (last && client_->FullMax && client_->isShade()) painter->drawLine(width()-1,0,width()-1,height());
	}
	if (wndcfg->inlineMode && (client_->FullMax||dm==1) && !client_->isShade())
	{
		// inline the frame
		if (wndcfg->inlineMode==1) painter->setPen(wndcfg->inlineColor);
		if (wndcfg->inlineMode==2) painter->setPen(wndcfg->inlineColor.dark(150));
		if (wndcfg->inlineMode==3) painter->setPen(wndcfg->inlineColor.light(150));
		// buttons just need to draw the bottom line
		painter->drawLine(0,height()-1,width(),height()-1);
	}


	if (type_ == MenuButton && (!::factory->menuImage || image==NULL || (image!=NULL && !image->initialized()))) {
		// we paint the mini icon (which is 16 pixels high)
		dx = float(width() - 16) / 2.0;
		dy = float(height() - 16) / 2.0;

		if (dx<1 || dy<=1)
		{
			int m=(rect().width()-2<rect().height())?rect().width()-2:rect().height();
			QRect r((rect().width()-m)/2,(rect().height()-m)/2,m,m);
// 			if (isDown()) { r.moveBy(1,1); }
        		 painter->drawPixmap(r, client_->icon().pixmap(16));
		}else{
//         	if (isDown()) { dx++; dy++; }
			 painter->drawPixmap((int)dx, (int)dy, client_->icon().pixmap(16));
		}
	} else if (image && image->initialized()) {
		// otherwise we paint the deco
		dx = float(width() - image->image_width) / 2.0;
		dy = float(height() - image->image_height) / 2.0;
		
		QImage *img=image->normal;

		if (::factory->hovereffect)
		{
			if (hover)
			{
				img=image->hovered; 
			}
			if (::factory->animateHover)
			{
				img=image->getAnimated(animation);
			}
		}
		if (isDown())
		{
			img=image->pressed;
		}
	
		if (img) {
			if (dx<image->hSpace/2 || dy<0)
			{	// Deco size is smaller than image, we need to stretch it
				int w,h;

				if (rect().width()-image->hSpace<rect().height())
				{
					w=rect().width()-image->hSpace;
					h=(int)((float)w*(float)image->image_height/(float)image->image_width);
				}else{
					h=rect().height();
					w=(int)((float)h*(float)image->image_width/(float)image->image_height);
				}

				QRect r((rect().width()-w)/2,(rect().height()-h)/2,w,h);

				painter->drawImage(r,*img);
				if (type_ == MenuButton) drawMenuImage(painter, r);
			}else{
				// Otherwise we just paint it
				if (image->drawMode==1)dy=0;
				painter->drawImage(QPoint((int)dx,(int)dy),*img);

				if (type_ == MenuButton) drawMenuImage(painter, 
					QRect((int)dx,(int)dy,image->image_width,image->image_height));
			}
		}
	}
	
// 	pufferPainter.end();
// 	painter->drawPixmap(0,0, pufferPixmap);
}

void CrystalButton::drawMenuImage(QPainter* painter, QRect r)
{
	if (type_ != MenuButton) return;
	// we paint the mini icon (which is 16 pixels high)
	r.setTop(r.top()+1);
	r.setBottom(r.bottom()-1);
	float dx = float(r.width() - 16) / 2.0;
	float dy = float(r.height() - 16) / 2.0;
		
	if (dx<1 || dy<=1)
	{
		int m=(r.width()-2<r.height())?r.width()-2:r.height();
		QRect r2(r.left()+(r.width()-m)/2,r.top()+(r.height()-m)/2,m,m);
		painter->drawPixmap(r2, client_->icon().pixmap(16));
	}else{
		painter->drawPixmap(r.left()+(int)dx, r.top()+(int)dy, client_->icon().pixmap(16));
	}
}

void CrystalButton::animate()
{
	if (hover)
	{
		animation+=0.25;
		if (animation>1.0)
		{
			animation=1.0;
			animation_timer.stop();
		}
	}else{
		animation-=0.15;
		if (animation<0.0)
		{
			animation=0.0;
			animation_timer.stop();
		}
	}
	repaint();
}
