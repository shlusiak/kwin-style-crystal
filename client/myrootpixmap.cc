/* vi: ts=8 sts=4 sw=4
 *
 * $Id: krootpixmap.cpp,v 1.20 2003/06/01 01:49:31 hadacek Exp $
 *
 * This file is part of the KDE project, module kdeui.
 * Copyright (C) 1999,2000 Geert Jansen <jansen@kde.org>
 *
 * You can Freely distribute this program under the GNU Library
 * General Public License. See the file "COPYING.LIB" for the exact
 * licensing terms.
 */
 
/* Modified by Sascha Hlusiak */

#include <qwidget.h>
#include <qtimer.h>
#include <qrect.h>
#include <qimage.h>

#ifndef Q_WS_QWS //FIXME
#include <kapplication.h>
#include <kimageeffect.h>
#include <kpixmapio.h>
#include <kwinmodule.h>
#include <kdebug.h>
#include <netwm.h>
#include <dcopclient.h>
#include <qpainter.h>

#include <ksharedpixmap.h>
#include "myrootpixmap.h"



KMyRootPixmap::KMyRootPixmap( QWidget *widget, const char *name )
    : QObject(widget, name ? name : "KMyRootPixmap" ) {
    init();
}

KMyRootPixmap::KMyRootPixmap( QWidget *widget, QObject *parent, const char *name )
    : QObject( parent, name ? name : "KMyRootPixmap" ) 
{
    init();
}

void KMyRootPixmap::init()
{
//    d = new KMyRootPixmapData;
//    m_Fade = 0;
    m_pPixmap = new KSharedPixmap;
//    m_pTimer = new QTimer( this );
    m_bInit = false;
    m_bActive = false;
    m_Desk=-1;
//    m_bCustomPaint = false;

//    connect(kapp, SIGNAL(backgroundChanged(int)), SLOT(slotBackgroundChanged(int)));
    connect(m_pPixmap, SIGNAL(done(bool)), SLOT(slotDone(bool)));
//    connect(m_pTimer, SIGNAL(timeout()), SLOT(repaint()));

//    d->toplevel = m_pWidget->topLevelWidget();
//    d->toplevel->installEventFilter(this);
}

KMyRootPixmap::~KMyRootPixmap()
{
    delete m_pPixmap;
//    delete d;
}

int KMyRootPixmap::currentDesktop() const
{
    NETRootInfo rinfo( qt_xdisplay(), NET::CurrentDesktop );
    rinfo.activate();
    return rinfo.currentDesktop();
}

void KMyRootPixmap::start()
{
    if (m_bActive)
    return;

    m_bActive = true;
    enableExports();
    return;
//    if (m_bInit)
//    repaint(true);
}

void KMyRootPixmap::stop()
{
    m_bActive = false;
//    m_pTimer->stop();
}


void KMyRootPixmap::repaint()
{
    repaint(false);
}

void KMyRootPixmap::repaint(bool force)
{
//	printf("KMyRootPixmap::repaint(%s)\n",force?"true":"false");
    if ((!force) && (m_Desk==currentDesktop()))return;
    
    m_Desk = currentDesktop();

    if (!isAvailable())
    {
    	emit backgroundUpdated(NULL);
    }else{
    	// KSharedPixmap will correctly generate a tile for us.
    	m_pPixmap->loadFromShared(pixmapName(m_Desk));
    	updateBackground( m_pPixmap );
    }
}

bool KMyRootPixmap::isAvailable() 
{
    return m_pPixmap->isAvailable(pixmapName(m_Desk));
}

QString KMyRootPixmap::pixmapName(int desk) 
{
    QString pattern = QString("DESKTOP%1");
    int screen_number = DefaultScreen(qt_xdisplay());
    if (screen_number) {
        pattern = QString("SCREEN%1-DESKTOP").arg(screen_number) + "%1";
    }
    return pattern.arg( desk );
}


void KMyRootPixmap::enableExports()
{
//    kdDebug(270) << k_lineinfo << "activating background exports.\n";
    DCOPClient *client = kapp->dcopClient();
    if (!client->isAttached())
    client->attach();
    QByteArray data;
    QDataStream args( data, IO_WriteOnly );
    args << 1;

    QCString appname( "kdesktop" );
    int screen_number = DefaultScreen(qt_xdisplay());
    if ( screen_number )
        appname.sprintf("kdesktop-screen-%d", screen_number );

    client->send( appname, "KBackgroundIface", "setExport(int)", data );
}


void KMyRootPixmap::slotDone(bool success)
{
    if (!success)
    {
//    	kdWarning(270) << k_lineinfo << "loading of desktop background failed.\n";
    	return;
    }

    // We need to test active as the pixmap might become available
    // after the widget has been destroyed.
    if ( m_bActive )
    updateBackground( m_pPixmap );
}

void KMyRootPixmap::updateBackground( KSharedPixmap *spm )
{
//	printf("KMyRootPixmap::updateBackground(%p)\n",spm);
    QPixmap *px=spm;
    if (px->isNull() || px->width()==0 || px->height()==0) 
    {	// This is NOT an image, something went wrong, update to plain
    	emit backgroundUpdated(NULL);
	return;
    }
    KPixmapIO io;
    QSize desktopsize(QApplication::desktop()->width(),QApplication::desktop()->height());
    
    if (px->rect().size()==desktopsize)
    {	// Image has already the right dimension, make a quick update
    	QImage img = io.convertToImage(*spm);
    	emit backgroundUpdated(&img);
	return;  
    }else{	// we need to create a tiled pixmap and then the image to update
    	QPixmap pix(desktopsize,spm->depth());
    	QPainter pufferPainter(&pix);
    
    	pufferPainter.drawTiledPixmap(pix.rect(),*spm);
    
    	pufferPainter.end();
    
    	QImage img=io.convertToImage(pix);
    	emit backgroundUpdated(&img);
    }
}

// #include "krootpixmap.moc"
#endif
