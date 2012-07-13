#include <kapp.h>
#include <qimage.h>
#include <kimageeffect.h>
#include "imageholder.h"
#include "crystalclient.h"


QImageHolder::QImageHolder()
{
	rootpixmap=NULL;
	mytexture=0;
	vscreenwidth=vscreenheight=1.0;
	initialized=false;
}

QImageHolder::~QImageHolder()
{
	if (rootpixmap)delete rootpixmap;
	if (mytexture)glDeleteTextures(1,&mytexture);
}

void QImageHolder::Init()
{
	if (initialized)return;
	
	rootpixmap=new KMyRootPixmap(NULL/*,this*/);
	connect( rootpixmap,SIGNAL(backgroundUpdated(const QImage*)),this, SLOT(BackgroundUpdated(const QImage*)));
	connect(kapp, SIGNAL(backgroundChanged(int)),SLOT(handleDesktopChanged(int)));
	rootpixmap->start();
	rootpixmap->repaint(true);
	
	initialized=true;
}

void QImageHolder::repaint(bool force)
{
	Init(); 
	rootpixmap->repaint(force);
}

void QImageHolder::handleDesktopChanged(int)
{
	repaint(true);
}

void QImageHolder::CheckSanity()
{
	if (!initialized)return;
	if (texture())return;
	
//	printf("SanityCheck failed, uninitializing\n");	
	delete rootpixmap;
	rootpixmap=NULL;
	
	initialized=false;
//	printf("Uninitialized.\n");	
}

void QImageHolder::BackgroundUpdated(const QImage *src)
{
	if (src==NULL)return;
	if (src->isNull())return;
	QImage img=*src,img2;
	if (::factory->brightness!=100)
		img=KImageEffect::intensity(img,(float)(::factory->brightness-100)/100.0f);
	img=img.smoothScale(::factory->textureSize,::factory->textureSize);
	img2=CrystalFactory::convertToGLFormat(img);
	vscreenwidth=src->width();
	vscreenheight=src->height();

	::factory->makeCurrent();
	if (mytexture==0)
		glGenTextures(1,&mytexture);

	glBindTexture(GL_TEXTURE_2D,mytexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, img2.width(), img2.height(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, img2.bits() );
	
	emit repaintNeeded();
}

void QImageHolder::activateTexture()
{
	Init();
	glBindTexture(GL_TEXTURE_2D,mytexture);
}

