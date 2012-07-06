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


#include <qimage.h>
#include <qtooltip.h>
#include <qpainter.h>

#include "crystalclient.h"
#include "crystalbutton.h"
#include "buttonimage.h"
#include "imageholder.h"


CrystalButton::CrystalButton(CrystalClient *parent, const char *name,
				const QString& tip, ButtonType type,
				ButtonImage *vimage)
: QButton(parent->widget(), name), client_(parent), type_(type), image(vimage), lastmouse_(0)
{
	setBackgroundMode(NoBackground);
	resetSize(false);
	setCursor(arrowCursor);
	
	hover=first=last=false;
	animation=0.0;
	QToolTip::add(this, tip);
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
	}else setFixedSize(buttonSizeH(),buttonSizeV());
}

void CrystalButton::setBitmap(ButtonImage *newimage)
{
	image=newimage;
	repaint(false);
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
	if (factory->hovereffect)repaint(false);
	if (factory->animateHover)animation_timer.start(60);
	QButton::enterEvent(e);
}

void CrystalButton::leaveEvent(QEvent *e)
{
	hover=false;
	if (factory->hovereffect)repaint(false);
	if (factory->animateHover)animation_timer.start(80);
	QButton::leaveEvent(e);
}

void CrystalButton::mousePressEvent(QMouseEvent* e)
{
	lastmouse_ = e->button();
	int button;
	switch(e->button())
	{
	case LeftButton:
		button=LeftButton;
		break;
	case RightButton:
		if ((type_ == ButtonMax) || (type_ == ButtonMin) || (type_ == ButtonMenu) /*|| (type_ == ButtonClose)*/)
			button=LeftButton; else button=NoButton;
		break;
	case MidButton:
		if ((type_ == ButtonMax) || (type_ == ButtonMin))
			button=LeftButton; else button=NoButton;
		break;

	default:button=NoButton;
		break;
	}
	QMouseEvent me(e->type(), e->pos(), e->globalPos(),button, e->state());
	QButton::mousePressEvent(&me);
}

void CrystalButton::mouseReleaseEvent(QMouseEvent* e)
{
	lastmouse_ = e->button();
	int button;
	switch(e->button())
	{
	case LeftButton:
		button=LeftButton;
		break;
	case RightButton:
		if ((type_ == ButtonMax) || (type_ == ButtonMin) || (type_ == ButtonMenu)/* || (type_==ButtonClose)*/)
			button=LeftButton; else button=NoButton;
		break;
	case MidButton:
		if ((type_ == ButtonMax) || (type_ == ButtonMin))
			button=LeftButton; else button=NoButton;
		break;
	
	default:button=NoButton;
		break;
	}
	QMouseEvent me(e->type(), e->pos(), e->globalPos(), button, e->state());
	QButton::mouseReleaseEvent(&me);
}

void CrystalButton::drawButton(QPainter *painter)
{
	if (!CrystalFactory::initialized()) return;

	QColorGroup group;
	float dx, dy;
	int dm=0;

	QPixmap pufferPixmap;
	pufferPixmap.resize(width(), height());
	QPainter pufferPainter(&pufferPixmap);
	
	CrystalFactory *f=((CrystalFactory*)client_->factory());
	QPixmap *background=f->image_holder->image(client_->isActive());
	WND_CONFIG *wndcfg=client_->isActive()?&f->active:&f->inactive;

	if (background && !background->isNull())
	{
		QRect r=rect();
		QPoint p=mapToGlobal(QPoint(0,0));
		r.moveBy(p.x(),p.y());
	
		pufferPainter.drawPixmap(QPoint(0,0),*background,r);
	}else{
		group = client_->options()->colorGroup(KDecoration::ColorTitleBar, client_->isActive());
		pufferPainter.fillRect(rect(), group.background());
	}

	if (!wndcfg->overlay.isNull())
	{
		pufferPainter.drawTiledPixmap(rect(),wndcfg->overlay,QPoint(x(),y()));
	}

	dm=0;
	if (image && (image->drawMode==1))dm=1;
	if (wndcfg->frame && (client_->FullMax|| (dm==1)) && (client_->isShade()||(dm==1)))
	{
		// outline the frame
		pufferPainter.setPen(wndcfg->frameColor);
		pufferPainter.drawLine(0,0,width(),0);
		if (client_->isShade())pufferPainter.drawLine(0,height()-1,width(),height()-1);
		
		if (first && client_->FullMax)pufferPainter.drawLine(0,0,0,height());
		if (last && client_->FullMax)pufferPainter.drawLine(width()-1,0,width()-1,height());
	}
	if (wndcfg->inlineFrame && (client_->FullMax||dm==1) && !client_->isShade())
	{
		// inline the frame
		pufferPainter.setPen(wndcfg->inlineColor);
		pufferPainter.drawLine(0,height()-1,width(),height()-1);
	}


	if (type_ == ButtonMenu) {
		// we paint the mini icon (which is 16 pixels high)
		dx = float(width() - 16) / 2.0;
		dy = float(height() - 16) / 2.0;

		if (dx<1 || dy<=1)
		{
			int m=(rect().width()-2<rect().height())?rect().width()-2:rect().height();
			QRect r((rect().width()-m)/2,(rect().height()-m)/2,m,m);
// 			if (isDown()) { r.moveBy(1,1); }
        		pufferPainter.drawPixmap(r, client_->icon().pixmap(QIconSet::Small,
                                                           QIconSet::Normal));
		}else{
//         	if (isDown()) { dx++; dy++; }
			pufferPainter.drawPixmap((int)dx, (int)dy, client_->icon().pixmap(QIconSet::Small,
                                                           QIconSet::Normal));
		}
	} else if (image) {
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
	
		if (img)
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

			pufferPainter.drawImage(r,*img);
		}else{
			// Otherwise we just paint it
			if (image->drawMode==1)dy=0;
			pufferPainter.drawImage(QPoint((int)dx,(int)dy),*img);
	    }
	}
	
	pufferPainter.end();
	painter->drawPixmap(0,0, pufferPixmap);
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
	repaint(false);
}

#include "crystalbutton.moc"
