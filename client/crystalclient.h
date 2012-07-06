//////////////////////////////////////////////////////////////////////////////
// crystalclient.h
// -------------------
// Crystal window decoration for KDE
// -------------------
// Copyright (c) 2006-2008 Sascha Hlusiak <spam84@gmx.de>
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

#ifndef CRYSTALCLIENT_H
#define CRYSTALCLIENT_H

#include <qlayout.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>
#include <qtimer.h>
#include <qlist.h>
#include <X11/Xlib.h>


class QSpacerItem;
class QPoint;

class CrystalClient;
class CrystalFactory;
class CrystalButton;
class ButtonImage;

extern CrystalFactory *factory;

#define TOP_LEFT 1
#define TOP_RIGHT 2
#define BOT_LEFT 4
#define BOT_RIGHT 8

struct WND_CONFIG
{
	int outlineMode,inlineMode;
	QColor frameColor,inlineColor;
	QPixmap overlay;
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
	ButtonImageMenu=0,
	ButtonImageHelp,
	ButtonImageMax,
	ButtonImageRestore,
	ButtonImageMin,
	ButtonImageClose, 
	ButtonImageSticky,
	ButtonImageUnSticky,
	ButtonImageShade,
	ButtonImageUnShade,
	ButtonImageBelow,
	ButtonImageUnBelow,
	ButtonImageAbove,
	ButtonImageUnAbove,
	ButtonImageCount
};


class CrystalFactory: public KDecorationFactory
{
public:
	CrystalFactory();
	virtual ~CrystalFactory();
	virtual KDecoration *createDecoration(KDecorationBridge *b);
	virtual bool reset(unsigned long changed);
	virtual bool supports(Ability ability) const;

	static bool initialized() { return initialized_; }
	static Qt::Alignment titleAlign() { return titlealign_; }
public:
	QPixmap logo;
	int logoEnabled,logoStretch,logoActive,logoDistance;
	
	int titlesize;
	bool hovereffect,tintButtons,animateHover,menuImage,wheelTask;

	QColor buttonColor_normal,buttonColor_hovered,buttonColor_pressed;
	QColor minColor_normal,minColor_hovered,minColor_pressed;
	QColor maxColor_normal,maxColor_hovered,maxColor_pressed;
	QColor closeColor_normal,closeColor_hovered,closeColor_pressed;

	int borderwidth;
	bool drawcaption,textshadow,captiontooltip;
	int roundCorners;
	WND_CONFIG active,inactive;
	int buttontheme;
	
	ButtonImage *buttonImages[ButtonImageCount];
	QList <CrystalClient*> clients;
private:
	bool readConfig();
	void CreateButtonImages();
private:
	static bool initialized_;
	static Qt::AlignmentFlag titlealign_;
};


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
	
	void ClientWindows(Window* frame,Window* wrapper,Window* client);
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
	
private slots:
	void Repaint();
	void maxButtonPressed();
	void minButtonPressed();
	void shadeButtonPressed();
	void aboveButtonPressed();
	void belowButtonPressed();
	void menuButtonPressed();
	void closeButtonPressed();
	
	void keepAboveChange( bool );
	void keepBelowChange( bool );
	void menuPopUp();
private:
	CrystalButton *button[ButtonTypeCount];
	QGridLayout *mainlayout;
	QHBoxLayout *titlelayout;
public:
	bool FullMax;
	QSpacerItem *titlebar_;
};

#endif
