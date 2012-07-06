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

#include <qlayout.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>
#include <qtimer.h>
#include <qptrlist.h>
#include <X11/Xlib.h>


class QSpacerItem;
class QPoint;

class CrystalClient;
class CrystalFactory;
class CrystalButton;
class QImageHolder;

class ButtonImage;

extern CrystalFactory *factory;


#define TOP_LEFT 1
#define TOP_RIGHT 2
#define BOT_LEFT 4
#define BOT_RIGHT 8

struct WND_CONFIG
{
	int mode;	// The mode (fade, emboss, ...)
	
	double amount;
	bool frame;
	QColor frameColor;
};


enum ButtonType {
    ButtonHelp=0,
    ButtonMax,
    ButtonMin,
    ButtonClose, 
    ButtonMenu,
    ButtonSticky,
    ButtonShade,
	ButtonAbove,
	ButtonBelow,
    ButtonTypeCount
};


enum ButtonImageTypes {
    ButtonImageHelp=0,
    ButtonImageMax,
	ButtonImageRestore,
    ButtonImageMin,
    ButtonImageClose, 
    ButtonImageSticky,
	ButtonImageUnSticky,
    ButtonImageShade,
	ButtonImageBelow,
	ButtonImageUnBelow,
	ButtonImageAbove,
	ButtonImageUnAbove,
    ButtonImageCount
};



// ExampleFactory /////////////////////////////////////////////////////////////


class CrystalFactory: public KDecorationFactory
{
public:
    CrystalFactory();
    virtual ~CrystalFactory();
    virtual KDecoration *createDecoration(KDecorationBridge *b);
    virtual bool reset(unsigned long changed);

    static bool initialized() { return initialized_; }
    static Qt::AlignmentFlags titleAlign() { return titlealign_; }
public:
	QImageHolder *image_holder;
	
	int titlesize;
	bool hovereffect,tintButtons;
	QColor buttonColor;
	int borderwidth;
	bool textshadow;
	bool trackdesktop;
	int roundCorners;
	int repaintMode,repaintTime;
	WND_CONFIG active,inactive;
	int buttontheme;
	
	
	ButtonImage *buttonImages[ButtonImageCount];
	QPtrList <CrystalClient> clients;
	
private:
    bool readConfig();
	void CreateButtonImages();
private:
    static bool initialized_;
    static Qt::AlignmentFlags titlealign_;
};



// ExampleClient //////////////////////////////////////////////////////////////

class CrystalClient : public KDecoration
{
    Q_OBJECT
public:
    CrystalClient(KDecorationBridge *b,CrystalFactory *f);
    virtual ~CrystalClient();

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
    CrystalButton* addButtons(QBoxLayout* layout, const QString& buttons);
    void updateMask();
	int borderSpacing();
	void updateLayout();
	
    bool eventFilter(QObject *obj, QEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *);
    void moveEvent(QMoveEvent *);
    void showEvent(QShowEvent *);
	void mouseWheelEvent(QWheelEvent *e);
	
	void ShowTabMenu(QMouseEvent *e);
	void ClientWindows(Window* frame,Window* wrapper,Window* client);
private slots:
    void Repaint();
    void maxButtonPressed();
    void minButtonPressed();
    void shadeButtonPressed();
	void aboveButtonPressed();
	void belowButtonPressed();
    void menuButtonPressed();
	
	void keepAboveChange( bool );
	void keepBelowChange( bool );
	void menuPopUp();
private:
    CrystalButton *button[ButtonTypeCount];
    QSpacerItem *titlebar_;
	QGridLayout *mainlayout;
	QHBoxLayout *titlelayout;
    QTimer timer;
	
public:
	bool FullMax;
};


#endif // EXAMPLECLIENT_H
