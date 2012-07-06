#include <kapp.h>
#include <qimage.h>
#include <kimageeffect.h>
#include "imageholder.h"
#include "crystalclient.h"



QImageHolder::QImageHolder()
:img_active(NULL),img_inactive(NULL)
{
	rootpixmap=NULL;
	initialized=false;
}

QImageHolder::~QImageHolder()
{
	if (rootpixmap)delete rootpixmap;
	if (img_active)delete img_active;
	if (img_inactive)delete img_inactive;
}

void QImageHolder::Init()
{
	if (initialized)return;
	
//	printf("Calling Init\n");	
	
	rootpixmap=new KMyRootPixmap(NULL/*,this*/);
	rootpixmap->start();
	rootpixmap->repaint(true);
	connect( rootpixmap,SIGNAL(backgroundUpdated(const QImage*)),this, SLOT(BackgroundUpdated(const QImage*)));
	connect(kapp, SIGNAL(backgroundChanged(int)),SLOT(handleDesktopChanged(int)));
	
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
	if (img_active!=NULL)return;
	if (img_inactive!=NULL)return;

//	printf("SanityCheck failed, uninitializing\n");	
	delete rootpixmap;
	rootpixmap=NULL;
	
	initialized=false;
//	printf("Uninitialized.\n");	
}

QPixmap *ApplyEffect(QImage &src,WND_CONFIG* cfg,QColorGroup colorgroup)
{
	QImage dst;
	
	switch(cfg->mode)
	{
	case 0:	if (cfg->amount>0.99)return new QPixmap();
		dst=KImageEffect::fade(src, cfg->amount, colorgroup.background());
		break;
	case 1:dst=KImageEffect::channelIntensity(src,cfg->amount,KImageEffect::All);
		break;
	case 2:dst=KImageEffect::intensity(src,cfg->amount);
		break;
	case 3:dst=KImageEffect::desaturate(src,cfg->amount);
		break;
	case 4: dst=src;
		KImageEffect::solarize(dst,cfg->amount*100.0);
		break;
//	case 5:dst=KImageEffect::emboss(src);
//		break;
//	case 6:dst=KImageEffect::charcoal(src);
//		break;

	default:dst=src;
		break;	
	}
	
	return new QPixmap(dst);
}

void QImageHolder::BackgroundUpdated(const QImage *src)
{
	if (img_active)
	{
		delete img_active;
		img_active=NULL;
	}
	if (img_inactive)
	{
		delete img_inactive;
		img_inactive=NULL;
	}
	
	if (src && !src->isNull())
	{
		src->scale(1024,768);
		QImage tmp=src->copy();

//		if (!img_active)img_active=new QImage;
//		if (!img_inactive)img_inactive=new QImage;

		img_inactive=ApplyEffect(tmp,&::factory->inactive,factory->options()->colorGroup(KDecoration::ColorTitleBar, false));
		tmp=src->copy();
		img_active=ApplyEffect(tmp,&::factory->active,factory->options()->colorGroup(KDecoration::ColorTitleBar, true));
	}
	
	emit repaintNeeded();	
}

