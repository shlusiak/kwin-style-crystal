#include <qimage.h>
#include <qtooltip.h>
#include <qpainter.h>
#include <kimageeffect.h>

#include "crystalclient.h"
#include "crystalbutton.h"
#include "imageholder.h"





ButtonImage::ButtonImage(const QRgb *d_normal,bool blend,QColor color)
{ 
	normal=hovered=pressed=NULL;
	if (d_normal)SetNormal(d_normal,blend,color);
}

ButtonImage::~ButtonImage()
{
	if (normal)delete normal;
	if (hovered)delete hovered;
	if (pressed)delete pressed;
}

QImage ButtonImage::CreateImage(const QRgb *data,bool blend,QColor color)
{
	QImage img=QImage((uchar*)data,DECOSIZE,DECOSIZE,32,NULL,0,QImage::LittleEndian),img2;
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
	normal=hovered=pressed=NULL;
}

void ButtonImage::SetNormal(const QRgb *d_normal,bool blend,QColor color)
{
	if (normal)delete normal;
	normal=new QImage(CreateImage(d_normal,blend,color));
}

void ButtonImage::SetHovered(const QRgb *d_hovered,bool blend,QColor color)
{
	if (hovered)delete hovered;
	if (d_hovered)
	{
		hovered=new QImage(CreateImage(d_hovered,blend,color));
	}else{
		hovered=NULL;
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





CrystalButton::CrystalButton(CrystalClient *parent, const char *name,
                             const QString& tip, ButtonType type,
                             ButtonImage *vimage)
    : QButton(parent->widget(), name), client_(parent), type_(type),
      image(vimage), lastmouse_(0)
{
    setBackgroundMode(NoBackground);
    setFixedSize(buttonSizeH(), buttonSizeV());
    setCursor(arrowCursor);
	
	hover=first=last=false;
    QToolTip::add(this, tip);
}

CrystalButton::~CrystalButton()
{ }

void CrystalButton::resetSize(bool FullSize)
{
	if (FullSize)
	{
		setFixedSize(buttonSizeH(),factory->titlesize+1);
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
	//return (factory->titlesize-1-FRAMESIZE>DECOSIZE)?BUTTONSIZE:factory->titlesize-1-FRAMESIZE+(BUTTONSIZE-DECOSIZE);
	return (factory->titlesize-1-FRAMESIZE>=DECOSIZE)?BUTTONSIZE:buttonSizeV()+2;
}

int CrystalButton::buttonSizeV() const
{
	return (factory->titlesize-1-FRAMESIZE>DECOSIZE)?DECOSIZE:factory->titlesize-1-FRAMESIZE;
}

//////////////////////////////////////////////////////////////////////////////
// enterEvent()
// ------------
// Mouse has entered the button

void CrystalButton::enterEvent(QEvent *e)
{
    // if we wanted to do mouseovers, we would keep track of it here
	hover=true;
	if (factory->hovereffect)repaint(false);
    QButton::enterEvent(e);
}

//////////////////////////////////////////////////////////////////////////////
// leaveEvent()
// ------------
// Mouse has left the button

void CrystalButton::leaveEvent(QEvent *e)
{
    // if we wanted to do mouseovers, we would keep track of it here
	hover=false;
	if (factory->hovereffect)repaint(false);
    QButton::leaveEvent(e);
}

//////////////////////////////////////////////////////////////////////////////
// mousePressEvent()
// -----------------
// Button has been pressed

void CrystalButton::mousePressEvent(QMouseEvent* e)
{
    lastmouse_ = e->button();
    int button;
    // translate and pass on mouse event
    switch(e->button())
    {
    	case LeftButton:
			button=LeftButton;
			break;
		case RightButton:
			if ((type_ == ButtonMax) || (type_ == ButtonMin) || (type_ == ButtonMenu))
				button=LeftButton; else button=NoButton;
			break;
		case MidButton:
			if ((type_ == ButtonMax) || (type_ == ButtonMin))
				button=LeftButton; else button=NoButton;
			break;
	
		default:button=NoButton;
			break;
    }
    
    
    QMouseEvent me(e->type(), e->pos(), e->globalPos(),
                   button, e->state());
    QButton::mousePressEvent(&me);
}

//////////////////////////////////////////////////////////////////////////////
// mouseReleaseEvent()
// -----------------
// Button has been released

void CrystalButton::mouseReleaseEvent(QMouseEvent* e)
{
    lastmouse_ = e->button();
    int button;
    // translate and pass on mouse event
    switch(e->button())
    {
    	case LeftButton:
		button=LeftButton;
		break;
	case RightButton:
		if ((type_ == ButtonMax) || (type_ == ButtonMin) || (type_ == ButtonMenu))
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

//////////////////////////////////////////////////////////////////////////////
// drawButton()
// ------------
// Draw the button

void CrystalButton::drawButton(QPainter *painter)
{
    if (!CrystalFactory::initialized()) return;
    
    QColorGroup group;
    float dx, dy;

    QPixmap pufferPixmap;
    pufferPixmap.resize(width(), height());
    QPainter pufferPainter(&pufferPixmap);
    
    // paint a plain box with border
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

	int m=(rect().width()-2<rect().height())?rect().width()-2:rect().height();
    QRect r((rect().width()-m)/2,(rect().height()-m)/2,m,m);

    if (type_ == ButtonMenu) {
        // we paint the mini icon (which is 16 pixels high)
        dx = float(width() - 16) / 2.0;
        dy = float(height() - 16) / 2.0;

		if (dx<1 || dy<=1)
		{
// 			if (isDown()) { r.moveBy(1,1); }
        		pufferPainter.drawPixmap(r, client_->icon().pixmap(QIconSet::Small,
                                                           QIconSet::Normal));
		}else{
//         	if (isDown()) { dx++; dy++; }
			pufferPainter.drawPixmap((int)dx, (int)dy, client_->icon().pixmap(QIconSet::Small,
                                                           QIconSet::Normal));
		}
    } else if (image) 
	{
        // otherwise we paint the deco
        dx = float(width() - DECOSIZE) / 2.0;
        dy = float(height() - DECOSIZE) / 2.0;
//		if (client_->FullMax)dy+=1;
		
		QImage *img=image->normal;
		int count=1;
		if (hover && ::factory->hovereffect)
		{
			count=1;
			if (image->hovered)img=image->hovered; else count=2;
		}
		if (isDown())
		{
			if (image->pressed)img=image->pressed; else count=3;
		}
		
		if (dx<1 || dy<0)
		{	// Deco size is smaller than image, we need to stretch it
			for (int i=0;i<count;i++)
				pufferPainter.drawImage(r,*img);
		}else{
			// Otherwise we just paint it
			for (int i=0;i<count;i++)
				pufferPainter.drawImage(QPoint((int)dx,(int)dy),*img);
	    }
	}
	

	if (wndcfg->frame && client_->FullMax && client_->isShade())
	{
		group = client_->options()->colorGroup(KDecoration::ColorFrame, client_->isActive());

    	// outline the frame
		pufferPainter.setPen(wndcfg->frameColor);
		pufferPainter.drawLine(0,0,width(),0);
		pufferPainter.drawLine(0,height()-1,width(),height()-1);
		
		if (first)pufferPainter.drawLine(0,0,0,height());
		if (last)pufferPainter.drawLine(width()-1,0,width()-1,height());
	}
	
	pufferPainter.end();
	painter->drawPixmap(0,0, pufferPixmap);    
}


