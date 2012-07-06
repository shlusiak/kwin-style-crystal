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
//#include <kdebug.h>
#include <netwm.h>
#include <dcopclient.h>

#include <ksharedpixmap.h>
#include "myrootpixmap.h"

class KMyRootPixmapData
{
public:
    QWidget *toplevel;
};


KMyRootPixmap::KMyRootPixmap( QWidget *widget, const char *name )
    : QObject(widget, name ? name : "KMyRootPixmap" ), m_pWidget(widget)
{
    init();
}

KMyRootPixmap::KMyRootPixmap( QWidget *widget, QObject *parent, const char *name )
    : QObject( parent, name ? name : "KMyRootPixmap" ), m_pWidget(widget)
{
    init();
}

void KMyRootPixmap::init()
{
    d = new KMyRootPixmapData;
//    m_Fade = 0;
    m_pPixmap = new KSharedPixmap;
    m_pTimer = new QTimer( this );
    m_bInit = false;
    m_bActive = false;
//    m_bCustomPaint = false;

    connect(kapp, SIGNAL(backgroundChanged(int)), SLOT(slotBackgroundChanged(int)));
    connect(m_pPixmap, SIGNAL(done(bool)), SLOT(slotDone(bool)));
    connect(m_pTimer, SIGNAL(timeout()), SLOT(repaint()));

    d->toplevel = m_pWidget->topLevelWidget();
    d->toplevel->installEventFilter(this);
}

KMyRootPixmap::~KMyRootPixmap()
{
    delete m_pPixmap;
    delete d;
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
    if ( !isAvailable() )
    {
    // We will get a KIPC message when the shared pixmap is available.
    enableExports();
    return;
    }
    if (m_bInit)
    repaint(true);
}


void KMyRootPixmap::stop()
{
    m_bActive = false;
    m_pTimer->stop();
}


/*void KMyRootPixmap::setFadeEffect(double fade, const QColor &color)
{
    if (fade < 0)
    m_Fade = 0;
    else if (fade > 1)
    m_Fade = 1;
    else
    m_Fade = fade;
    m_FadeColor = color;

    if ( m_bActive && m_bInit ) repaint(true);
}*/


bool KMyRootPixmap::eventFilter(QObject *, QEvent *event)
{
    // Initialise after the first show or paint event on the managed widget.
    if (!m_bInit && ((event->type() == QEvent::Show) || (event->type() == QEvent::Paint)))
    {
    m_bInit = true;
    m_Desk = currentDesktop();
    }

    if (!m_bActive)
    return false;

    switch (event->type())
    {
    case QEvent::Resize:
    case QEvent::Move:
    m_pTimer->start(150, true);
    break; 

    case QEvent::Paint:
    m_pTimer->start(0, true);
    break;

    case QEvent::Reparent:
        d->toplevel->removeEventFilter(this);
        d->toplevel = m_pWidget->topLevelWidget();
        d->toplevel->installEventFilter(this);
        break;

    default:
    break;
    }

    return false; // always continue processing
}


void KMyRootPixmap::repaint()
{
    repaint(false);
}


void KMyRootPixmap::repaint(bool force)
{
    QPoint p1 = m_pWidget->mapToGlobal(m_pWidget->rect().topLeft());
    QPoint p2 = m_pWidget->mapToGlobal(m_pWidget->rect().bottomRight());
    if (!force && (m_Rect == QRect(p1, p2)))
    return;

    // Due to northwest bit gravity, we don't need to do anything if the
    // bottom right corner of the widget is moved inward.
    // That said, konsole clears the background when it is resized, so
    // we have to reset the background pixmap.
    if ((p1 == m_Rect.topLeft()) && (m_pWidget->width() < m_Rect.width()) &&
    (m_pWidget->height() < m_Rect.height())
       )
    {
    updateBackground( m_pPixmap );
    return;
    }
    m_Rect = QRect(p1, p2);
    m_Desk = currentDesktop();

    // KSharedPixmap will correctly generate a tile for us.
    m_pPixmap->loadFromShared(pixmapName(m_Desk), m_Rect);
}

bool KMyRootPixmap::isAvailable() const
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
//    kdWarning(270) << k_lineinfo << "loading of desktop background failed.\n";
    return;
    }

    // We need to test active as the pixmap might become available
    // after the widget has been destroyed.
    if ( m_bActive )
    updateBackground( m_pPixmap );
}

void KMyRootPixmap::updateBackground( KSharedPixmap *spm )
{
    KPixmapIO io;
    QImage img = io.convertToImage(*spm);
    
    emit applyEffect(img,img);
    emit backgroundUpdated(img);
}


void KMyRootPixmap::slotBackgroundChanged(int desk)
{
    if (!m_bInit || !m_bActive)
    return;

    if (desk == m_Desk)
    repaint(true);
}

// #include "krootpixmap.moc"
#endif
