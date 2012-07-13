#include <qimage.h>
// #include <qtooltip.h>
#include <qpainter.h>
#include <kimageeffect.h>

#include "crystalclient.h"
#include "crystalbutton.h"
#include "imageholder.h"





ButtonImage::ButtonImage(const QRgb *d_normal,bool blend,QColor color)
{ 
	normal=hovered=pressed=NULL;
	t_normal=t_hovered=t_pressed=0;
	if (d_normal)SetNormal(d_normal,blend,color);
}

ButtonImage::~ButtonImage()
{
	reset();
}

QImage ButtonImage::CreateImage(const QRgb *data,bool blend,QColor color)
{
	QImage img=QImage((uchar*)data,DECOSIZE,DECOSIZE,32,NULL,0,QImage::LittleEndian),img2;
	img.setAlphaBuffer(true);

	if (blend)
	{
		img2=img.copy();
		img=KImageEffect::blend(color,img2,1.0f);
	}
	return CrystalFactory::convertToGLFormat(img.smoothScale(32,32));
}

GLuint ButtonImage::CreateTexture(QImage *img)
{	
	GLuint texture;
	glGenTextures(1,&texture);
	
	activate(img,texture);
	return texture;
}

void ButtonImage::reset()
{
	if (normal)delete normal;
	if (hovered)delete hovered;
	if (pressed)delete pressed;
	normal=hovered=pressed=NULL;
	resetTextures();
}

void ButtonImage::resetTextures()
{
	if (t_normal)glDeleteTextures(1,&t_normal);
	if (t_hovered)glDeleteTextures(1,&t_hovered);
	if (t_pressed)glDeleteTextures(1,&t_pressed);
	t_normal=t_hovered=t_pressed=0;
}

void ButtonImage::SetNormal(QImage image)
{
	if (normal)delete normal;
	if (t_normal)glDeleteTextures(1,&t_normal);
	t_normal=0;	
	normal=new QImage(CrystalFactory::convertToGLFormat(image.smoothScale(32,32)));
}

void ButtonImage::SetNormal(const QRgb *d_normal,bool blend,QColor color)
{
	if (normal)delete normal;
	if (t_normal)glDeleteTextures(1,&t_normal);
	t_normal=0;	
	normal=new QImage(CreateImage(d_normal,blend,color));
}

void ButtonImage::SetHovered(const QRgb *d_hovered,bool blend,QColor color)
{
	if (hovered)delete hovered;
	if (t_hovered)glDeleteTextures(1,&t_hovered);
	t_hovered=0;
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
	if (t_pressed)glDeleteTextures(1,&t_pressed);
	t_pressed=0;
	if (d_pressed)
	{
		pressed=new QImage(CreateImage(d_pressed,blend,color));
	}else{
		pressed=NULL;
	}
}

void ButtonImage::activate(QImage *img,GLuint texture)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, 4, img->width(), img->height(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, img->bits() );
}

void ButtonImage::check()
{
	if (normal && !t_normal)
		t_normal=CreateTexture(normal);
	if (hovered && !t_hovered)
		t_hovered=CreateTexture(hovered);
	if (pressed && !t_pressed)
		t_pressed=CreateTexture(pressed);
}




CrystalButton::CrystalButton(CrystalClient *parent, const char *name,
                             const QString& tip, ButtonType type,
                             ButtonImage *vimage)
  : QObject(parent->widget(),name),
	QSpacerItem(buttonSizeH(),buttonSizeV()),
	client_(parent), type_(type),
	image(vimage), lastmouse_(0)
{
//    QToolTip::add(this, tip);
	hover=false;
	lastmouse_=NoButton;
	
	if (image==NULL)
	{
		image=&menuimage;
		menuimage.SetNormal(client_->icon().pixmap(QIconSet::Small,QIconSet::Normal).convertToImage());
	}
}

CrystalButton::~CrystalButton()
{ }

void CrystalButton::resetSize(bool FullSize)
{
	if (FullSize)
	{
		changeSize(buttonSizeH(),factory->titlesize+1);
	}else changeSize(buttonSizeH(),buttonSizeV());
}

void CrystalButton::setBitmap(ButtonImage *newimage)
{
	image=newimage;
	if (image==NULL)
	{
		image=&menuimage;
		menuimage.SetNormal(client_->icon().pixmap(QIconSet::Small,QIconSet::Normal).convertToImage());
	}
	repaint();
}

int CrystalButton::buttonSizeH() const
{
	int v=buttonSizeV();
	return v+4;
}

int CrystalButton::buttonSizeV() const
{
	return (factory->titlesize-FRAMESIZE*2>DECOSIZE)?DECOSIZE:factory->titlesize-FRAMESIZE*2;
}

bool CrystalButton::isInside(QPoint point)
{
	return geometry().contains(point);
}

void CrystalButton::repaint()
{
	client_->Repaint();
}

void CrystalButton::enterEvent()
{
	if (hover)return;
	hover=true;
	if (factory->hovereffect)repaint();
}

void CrystalButton::leaveEvent()
{
	if (!hover)return;
	hover=false;
	if (factory->hovereffect)repaint();
}

void CrystalButton::mouseMoveEvent(QMouseEvent* e)
{
	if (isInside(e->pos()))enterEvent();
		else leaveEvent();
}

bool CrystalButton::handleMouseButton(int button)
{
    switch(button)
    {
    case LeftButton:
			return true;
	case RightButton:
		if ((type_ == ButtonMax) || (type_ == ButtonMin) || (type_ == ButtonMenu))
			return true; else return false;
		break;
	case MidButton:
		if ((type_ == ButtonMax) || (type_ == ButtonMin))
			return true; else return false;
		break;
	default:return false;
    }
}

bool CrystalButton::mousePressEvent(QMouseEvent* e)
{
	if (!isInside(e->pos()))return false;
	if (!handleMouseButton(e->button()))return true;
	if (lastmouse_!=NoButton)return true;
	
	lastmouse_=e->button();
	repaint();
	emit pressed();
	return true;
}

void CrystalButton::mouseReleaseEvent(QMouseEvent* e)
{
	if (!isInside(e->pos()))
	{
		lastmouse_=NoButton;
		repaint();
		return;
	}
	if (lastmouse_==NoButton)return;
	repaint();
	emit clicked();
	lastmouse_=NoButton;
}

//////////////////////////////////////////////////////////////////////////////
// drawButton()
// ------------
// Draw the button

void CrystalButton::drawButton(double alpha)
{
    if (!CrystalFactory::initialized()) return;
	
	QRect r2=geometry();
	int bla=buttonSizeV();
	QRect r(r2.center().x()-bla/2,r2.center().y()-bla/2,bla+1,bla+1);

	glEnable(GL_TEXTURE_2D);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	int count=1;
	if (image)
	{
		image->check();
		image->activate(image->normal,image->t_normal);
		
		if (hover && ::factory->hovereffect)
		{
			count=1;
			if (image->hovered)
				image->activate(image->hovered,image->t_hovered);
				else count=2;
		}
		if (lastmouse_!=NoButton && hover)
		{
			if (image->pressed)
				image->activate(image->pressed,image->t_pressed);
				else count=3;
		}
	}else{
		
	
	}
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	
	glBegin(GL_QUADS);
	
	
	glColor4f(1,1,1,alpha);
	
	for (int i=0;i<count;i++)
	{
		glTexCoord2f(0,0);	glVertex3f(r.left(),r.bottom(),   0);		// Bottom left
		glTexCoord2f(1,0);	glVertex3f(r.right(),r.bottom(), 0);		// Bottom right
		glTexCoord2f(1,1);	glVertex3f(r.right(),r.top(), 0);		// Top right
		glTexCoord2f(0,1);	glVertex3f(r.left(),r.top(), 0);		// Top left		
	}	
	glEnd();

	glDisable(GL_BLEND);

	glBindTexture(GL_TEXTURE_2D,0);

/*    QColorGroup group;
    float dx, dy;

    QPixmap pufferPixmap;
    pufferPixmap.resize(width(), height());
    QPainter pufferPainter(&pufferPixmap);
    
    // paint a plain box with border
    QPixmap *background=((CrystalFactory*)client_->factory())->image_holder->image(client_->isActive());
	
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

	int m=(rect().width()-2<rect().height())?rect().width()-2:rect().height();
    QRect r((rect().width()-m)/2,(rect().height()-m)/2,m,m);

// TODO: Das hier richtig machen, wenn maximiert!

//	if (client_->FullMax)
//	{
//		r.setSize(
//		r.moveBy(0,FRAMESIZE);
//	}
	
    if (type_ == ButtonMenu) {
        // we paint the mini icon (which is 16 pixels high)
        dx = float(width() - 16) / 2.0;
        dy = float(height() - 16) / 2.0;

		if (dx<1 || dy<=1)
		{
			if (isDown()) { r.moveBy(1,1); }
        		pufferPainter.drawPixmap(r, client_->icon().pixmap(QIconSet::Small,
                                                           QIconSet::Normal));
		}else{
        	if (isDown()) { dx++; dy++; }
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
	
	WND_CONFIG* wndcfg=(client_->isActive()?&::factory->active:&::factory->inactive);
	// draw frame
	
	if (wndcfg->frame && client_->FullMax)
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
	painter->drawPixmap(0,0, pufferPixmap);    */
}


