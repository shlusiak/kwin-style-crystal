/* vi: ts=8 sts=4 sw=4
 *
 * $Id: krootpixmap.h,v 1.15 2003/05/19 08:02:48 coolo Exp $
 * This file is part of the KDE project, module kdesktop.
 * Copyright (C) 1999,2000 Geert Jansen <jansen@kde.org>
 *
 * You can Freely distribute this program under the GNU Library General
 * Public License. See the file "COPYING.LIB" for the exact licensing terms.
 */

/* Modified by Sascha Hlusiak */
 

#ifndef __KRootPixmap_h_Included__
#define __KRootPixmap_h_Included__

#include <qobject.h>
#include <qcolor.h>

#ifndef Q_WS_QWS //FIXME

class QRect;
class QWidget;
class QTimer;
class KSharedPixmap;
class KMyRootPixmapData;

class KMyRootPixmap: public QObject
{
    Q_OBJECT

public:
    KMyRootPixmap( QWidget *target, const char *name=0 );

    KMyRootPixmap( QWidget *target, QObject *parent, const char *name=0 );

    virtual ~KMyRootPixmap();

    bool isAvailable() const;

    bool isActive() const { return m_bActive; }

    int currentDesktop() const;

//    bool customPainting() const { return m_bCustomPaint; }

#ifndef KDE_NO_COMPAT

    bool checkAvailable(bool) { return isAvailable(); }
#endif

public slots:
    virtual void start();

    virtual void stop();

//    void setFadeEffect(double strength, const QColor &color);

    void repaint( bool force );

    void repaint();

//    void setCustomPainting( bool enable ) { m_bCustomPaint = enable; }

    void enableExports();

    static QString pixmapName(int desk);
    
signals:
    void backgroundUpdated( const QImage &pm );
    void applyEffect(QImage &src,QImage &dst);

protected:
    virtual bool eventFilter(QObject *, QEvent *);

    virtual void updateBackground( KSharedPixmap * );

private slots:
    void slotBackgroundChanged(int);
    void slotDone(bool);

private:
    bool m_bActive, m_bInit/*, m_bCustomPaint*/;
    int m_Desk;

//    double m_Fade;
//    QColor m_FadeColor;

    QRect m_Rect;
    QWidget *m_pWidget;
    QTimer *m_pTimer;
    KSharedPixmap *m_pPixmap;
    KMyRootPixmapData *d;

    void init();
};

#endif // ! Q_WS_QWS
#endif // __KRootPixmap_h_Included__
