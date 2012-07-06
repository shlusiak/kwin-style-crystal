//////////////////////////////////////////////////////////////////////////////
// exampleclient.h
// -------------------
// Example window decoration for KDE
// -------------------
// Copyright (c) 2003, 2004 David Johnson <david@usermode.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#ifndef EXAMPLECLIENT_H
#define EXAMPLECLIENT_H

#include <qbutton.h>
#include <qtimer.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>
#include <kwinmodule.h>
#include "myrootpixmap.h"

class QSpacerItem;
class QPoint;

namespace Crystal {

class ExampleClient;
class ExampleFactory;

enum ButtonType {
    ButtonHelp=0,
    ButtonMax,
    ButtonMin,
    ButtonClose, 
    ButtonMenu,
    ButtonSticky,
    ButtonShade,
    ButtonTypeCount
};

// ExampleFactory /////////////////////////////////////////////////////////////

class QImageHolder:public QObject
{
	Q_OBJECT
public:
	QImageHolder(ExampleFactory *vfactory);
	virtual ~QImageHolder();
	
	void Init();
	QImage *image(bool active) { Init(); return active?img_active:img_inactive; }
	void repaint(bool force);

private:
	ExampleFactory *factory;
	bool initialized;
	KMyRootPixmap *rootpixmap;
	QImage *img_active,*img_inactive;
	
public slots:
	void BackgroundUpdated(const QImage *);
	void handleDesktopChanged(int desk);
	void CheckSanity();
	
signals:
	void repaintNeeded();
};

class ExampleFactory: public KDecorationFactory
{
public:
    ExampleFactory();
    virtual ~ExampleFactory();
    virtual KDecoration *createDecoration(KDecorationBridge *b);
    virtual bool reset(unsigned long changed);

    static bool initialized();
    static Qt::AlignmentFlags titleAlign();
public:
	QImageHolder *image_holder;
	
private:
    void initButtonPixmaps();
    bool readConfig();

private:
    static bool initialized_;
    static Qt::AlignmentFlags titlealign_;
};

inline bool ExampleFactory::initialized()
    { return initialized_; }

inline Qt::AlignmentFlags ExampleFactory::titleAlign()
    { return titlealign_; }

// ExampleButton //////////////////////////////////////////////////////////////

class ExampleButton : public QButton
{
public:
    ExampleButton(ExampleClient *parent=0, const char *name=0,
                  const QString &tip=NULL,
                  ButtonType type=ButtonHelp,
                  QImage *bitmap=0);
    ~ExampleButton();

    void setBitmap(QImage *bitmap) {deco_=bitmap; repaint(false); }
    QSize sizeHint() const;
    int lastMousePress() const;
    void reset();

private:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void drawButton(QPainter *painter);
    
    int buttonSize() const;

private:
    ExampleClient *client_;
    ButtonType type_;
    QImage *deco_;
    int lastmouse_;
};

inline int ExampleButton::lastMousePress() const
    { return lastmouse_; }

inline void ExampleButton::reset()
    { repaint(false); }

// ExampleClient //////////////////////////////////////////////////////////////

class ExampleClient : public KDecoration
{
    Q_OBJECT
public:
    ExampleClient(KDecorationBridge *b, ExampleFactory *f);
    virtual ~ExampleClient();

    virtual void init();

    virtual void activeChange();
    virtual void desktopChange();
    virtual void captionChange();
    virtual void iconChange();
    virtual void maximizeChange();
    virtual void shadeChange();

    virtual void borders(int &l, int &r, int &t, int &b) const;
    virtual void resize(const QSize &size);
    virtual QSize minimumSize() const;
    virtual Position mousePosition(const QPoint &point) const;

private:
    void addButtons(QBoxLayout* layout, const QString& buttons);
    void updateMask();

    bool eventFilter(QObject *obj, QEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *);
    void moveEvent(QMoveEvent *);
    void showEvent(QShowEvent *);
    

private slots:
    void Repaint();
    void maxButtonPressed();
    void minButtonPressed();
    void shadeButtonPressed();
    void menuButtonPressed();
//    void BackgroundUpdated(const QImage&);
//    void DesktopChanged(int desktop);    

private:
    ExampleButton *button[ButtonTypeCount];
    QSpacerItem *titlebar_;
    QRegion mask;
    ExampleFactory* my_factory;
    QTimer timer;
};

} // namespace Example

#endif // EXAMPLECLIENT_H
