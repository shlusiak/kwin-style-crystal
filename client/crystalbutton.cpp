#include <qimage.h>
// #include <qtooltip.h>
#include <qpainter.h>

#include "crystalclient.h"
#include "crystalbutton.h"
#include "imageholder.h"





ButtonImage::ButtonImage()
{ 
	t_normal=t_hovered=t_pressed=0;
	color_normal=color_hovered=color_pressed=Qt::white;
}

ButtonImage::~ButtonImage()
{
	reset();
}

QImage ButtonImage::CreateImage(const QRgb *data)
{
	QImage img=QImage((uchar*)data,DECOSIZE,DECOSIZE,32,NULL,0,QImage::LittleEndian),img2;
	img.setAlphaBuffer(true);

	return CrystalFactory::convertToGLFormat(img.smoothScale(32,32));
}

GLuint ButtonImage::CreateTexture(QImage img)
{	
	GLuint texture;
	glGenTextures(1,&texture);
	
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, 4, img.width(), img.height(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, img.bits() );
	
	return texture;
}

void ButtonImage::reset()
{
	if (t_normal)glDeleteTextures(1,&t_normal);
	if (t_hovered)glDeleteTextures(1,&t_hovered);
	if (t_pressed)glDeleteTextures(1,&t_pressed);
	t_normal=t_hovered=t_pressed=0;
}

void ButtonImage::SetNormal(QImage image)
{
	if (t_normal)glDeleteTextures(1,&t_normal);
	t_normal=0;
	t_normal=CreateTexture(CrystalFactory::convertToGLFormat(image.smoothScale(32,32)));
	color_normal=color_hovered=color_pressed=Qt::white;
}

void ButtonImage::SetNormal(const QRgb *d_normal,QColor colornormal,QColor colorhovered,QColor colorpressed)
{
	if (t_normal)glDeleteTextures(1,&t_normal);
	t_normal=CreateTexture(CreateImage(d_normal));
	color_normal=colornormal;
	color_hovered=colorhovered;
	color_pressed=colorpressed;
}

void ButtonImage::SetHovered(const QRgb *d_hovered,QColor color)
{
	if (t_hovered)glDeleteTextures(1,&t_hovered);
	if (d_hovered)t_hovered=CreateTexture(CreateImage(d_hovered));
		else t_hovered=0;
	color_hovered=color;
}

void ButtonImage::SetPressed(const QRgb *d_pressed,QColor color)
{
	if (t_pressed)glDeleteTextures(1,&t_pressed);
	if (d_pressed)t_pressed=CreateTexture(CreateImage(d_pressed));
		else t_pressed=0;
	color_pressed=color;
}

void ButtonImage::drawNormal(QRect r,double alpha)
{
	glBindTexture(GL_TEXTURE_2D, t_normal);
	glColorQ(color_normal,alpha);
	draw(r);
}

void ButtonImage::drawHovered(QRect r,double alpha)
{
	if (t_hovered)glBindTexture(GL_TEXTURE_2D, t_hovered);
		else glBindTexture(GL_TEXTURE_2D,t_normal);
		
	glColorQ(color_hovered,alpha);
	draw(r);
}

void ButtonImage::drawPressed(QRect r,double alpha)
{
	glColorQ(color_pressed,alpha);
	
	if (t_pressed)
	{
		glBindTexture(GL_TEXTURE_2D, t_pressed);
		draw(r);
	}else if (t_hovered)
	{
		glBindTexture(GL_TEXTURE_2D,t_hovered);
		draw(r);
		draw(r);
	}else{
		glBindTexture(GL_TEXTURE_2D,t_normal);
		draw(r);
		draw(r);
		draw(r);
	}
}

void ButtonImage::draw(QRect r)
{
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);	glVertex3f(r.left(),r.bottom(),   0);		// Bottom left
	glTexCoord2f(1,0);	glVertex3f(r.right(),r.bottom(), 0);		// Bottom right
	glTexCoord2f(1,1);	glVertex3f(r.right(),r.top(), 0);		// Top right
	glTexCoord2f(0,1);	glVertex3f(r.left(),r.top(), 0);		// Top left		
	glEnd();
}






CrystalButton::CrystalButton(CrystalClient *parent, const char *name,
                             const QString& tip, ButtonType type,
                             ButtonImage *vimage)
  : QObject(/*parent->widget()*/NULL,name),
	client_(parent), type_(type),
	image(vimage)
{
//    QToolTip::add(this, tip);
	hover=false;
	animation=0.0;
	lastmouse=_lastmouse=NoButton;
	
	spacer=new QSpacerItem(buttonSizeH(),buttonSizeV());
	
	if (image==NULL)
	{
		::factory->makeCurrent();
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
		spacer->changeSize(buttonSizeH(),factory->titlesize+1);
	}else spacer->changeSize(buttonSizeH(),buttonSizeV());
}

void CrystalButton::setBitmap(ButtonImage *newimage)
{
	image=newimage;
	if (image==NULL)
	{
		::factory->makeCurrent();
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
	if (factory->hovereffect)
	{
		if (animate())client_->startAnimation();
		repaint();
	}
}

void CrystalButton::leaveEvent()
{
	if (!hover)return;
	hover=false;
	
	if (factory->hovereffect)
	{
		if (animate())client_->startAnimation();
		repaint();
	}
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
	if (_lastmouse!=NoButton)return true;
	
	_lastmouse=e->button();
	repaint();
	emit pressed();
	return true;
}

void CrystalButton::mouseReleaseEvent(QMouseEvent* e)
{
	if (!isInside(e->pos()))
	{
		_lastmouse=NoButton;
		repaint();
		return;
	}
	if (_lastmouse==NoButton)return;
 	repaint();
	lastmouse=_lastmouse;
	_lastmouse=NoButton;
	emit clicked();
}

bool CrystalButton::animate()
{
	if (!::factory->animateHover)
	{
		if (hover)animation=1.0; else animation=0.0;
		return false;
	}
	if (hover)
	{
		animation+=0.25;
		if (animation>1.0)animation=1.0;
		else return true;	// Finished this animation
	}else{
		animation-=0.12;
		if (animation<0.0)animation=0.0;
		else return true;	// Finished this animation
	}
	return false;	// Do not proceed animating this button
}

//////////////////////////////////////////////////////////////////////////////
// drawButton()
// ------------
// Draw the button

void CrystalButton::drawButton(double alpha)
{
    if (!CrystalFactory::initialized()) return;
	if (!image)return;
	
	if (type_==ButtonMenu)alpha*=0.8; // The menu image shall be at least a little translucent
	
	QRect r2=geometry();
	int bla=buttonSizeV();
	QRect r(r2.center().x()-bla/2,r2.center().y()-bla/2,bla+1,bla+1);

	glEnable(GL_TEXTURE_2D);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	if (_lastmouse!=NoButton && hover)
		image->drawPressed(r,alpha);
	else {
		if (image->hasHovered() && ::factory->hovereffect){
			image->drawNormal(r,alpha*(1.0-animation));
			image->drawHovered(r,alpha*animation);
		}else{
			if (::factory->hovereffect)
			{
				image->drawNormal(r,alpha*(1.0-animation));
				image->drawHovered(r,alpha*animation);
				image->drawHovered(r,alpha*animation);
			}else image->drawNormal(r,alpha);
		}
	}
	
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D,0);
}
