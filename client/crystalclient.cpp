/***************************************************************************
 *   crystalclient.cpp                                                     *
 *   -----------------------                                               *
 *   Copyright (C) 2006-2011 by Sascha Hlusiak                             *
 *   Spam84@gmx.de                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


/***************************************************************************
 *   Contributers include:                                                 *
 *                                                                         *
 *     Christian Quante, 2009                                              *
 ***************************************************************************/


#include <QTime>
#include <klocale.h>
#include <kdebug.h>

#include <qbitmap.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qimage.h>
#include <kwindowsystem.h>
#include <kwindowinfo.h>
#include <kprocess.h>

#include "crystalfactory.h"
#include "crystalclient.h"
#include "crystalbutton.h"



static QColor separatorColorHi(180,180,180,180);
static QColor separatorColorLo(32, 32, 32, 128);;



CrystalClient::CrystalClient(KDecorationBridge *b,CrystalFactory *f)
: KDecorationUnstable(b,f)
{
	::factory->clients.append(this);
	click_in_progress = false;
	drag_in_progress = false;
	sourceTab = -1;
	targetTab = -1;
}

CrystalClient::~CrystalClient()
{
	::factory->clients.removeAll(this);
	for (int n=0; n<NumButtons; n++) {
		if (button[n]) delete button[n];
	}
}

void CrystalClient::init()
{
	createMainWidget();
	widget()->setAttribute(Qt::WA_TranslucentBackground);	
	widget()->setAttribute(Qt::WA_NoSystemBackground);
	widget()->setAttribute(Qt::WA_StaticContents);
	widget()->setAttribute(Qt::WA_OpaquePaintEvent);
	
	widget()->setAutoFillBackground(false);
	
	widget()->installEventFilter(this);
	widget()->setAcceptDrops(true);
	
	FullMax=false;
	if (!options()->moveResizeMaximizedWindows())
		FullMax=(maximizeMode()==MaximizeFull);
	
	// setup layout
	mainlayout = new QGridLayout(widget());
	titlelayout = new QHBoxLayout();
	titlebar_ = new QSpacerItem(1, ::factory->titlesize-1, QSizePolicy::Expanding,
					QSizePolicy::Fixed);
	titlelayout->setMargin(0);
	titlelayout->setSpacing(0);

	mainlayout->setSizeConstraint(QLayout::SetNoConstraint);
	mainlayout->setRowMinimumHeight(0, (::factory->buttontheme==5)?0:1);
	mainlayout->setRowMinimumHeight(1, 0);
	mainlayout->setRowMinimumHeight(2, 0);
	mainlayout->setRowMinimumHeight(3, 0/*::factory->borderwidth*1*/);

	mainlayout->setColumnMinimumWidth(0,borderSpacing());
	mainlayout->setColumnMinimumWidth(1,0);
	mainlayout->setColumnMinimumWidth(2,borderSpacing());

	mainlayout->setRowStretch(0,0);
	mainlayout->setRowStretch(1,0);
	mainlayout->setRowStretch(2,10);
	mainlayout->setRowStretch(3,0);
	mainlayout->setColumnStretch(1, 10);
	
	mainlayout->setMargin(15);
	mainlayout->setSpacing(0);
	mainlayout->addLayout(titlelayout, 1, 1);

	if (isPreview()) {
		char c[512];
		QLabel *label;
		sprintf(c,"<center><b>Crystal %s Preview</b><br>Built: %s</center>",VERSION, __DATE__);
		mainlayout->addItem(new QSpacerItem(1, 1,QSizePolicy::Expanding,QSizePolicy::Fixed), 0, 1);
		mainlayout->addItem(new QSpacerItem(1, ::factory->borderwidth,QSizePolicy::Expanding,QSizePolicy::Expanding), 3, 1);
		label = new QLabel(i18n(c), widget());
		label->setAutoFillBackground(true);
		mainlayout->addWidget(label, 2, 1);
	} else {
		mainlayout->addItem(new QSpacerItem(0, 0), 2, 1);
	}
	
	updateMask();

	for (int n=0; n<NumButtons; n++) button[n] = 0;
	addButtons(titlelayout, options()->titleButtonsLeft());

	titlelayout->addItem(titlebar_);

	{
		CrystalButton* lastbutton=addButtons(titlelayout, options()->titleButtonsRight());
		if (lastbutton)lastbutton->setFirstLast(false,true);
	}

	connect( this, SIGNAL( keepAboveChanged( bool )), SLOT( keepAboveChange( bool )));
	connect( this, SIGNAL( keepBelowChanged( bool )), SLOT( keepBelowChange( bool )));
	
	widget()->setToolTip(caption());
	updateLayout();
}

void CrystalClient::updateMask()
{
	setMask(getMask());
}

QRegion CrystalClient::getMask() {
	if ((::factory->roundCorners==0) || (!options()->moveResizeMaximizedWindows() && maximizeMode() & MaximizeFull ) ) 
	{
		return QRegion(widget()->rect());
	}
	
	int cornersFlag = ::factory->roundCorners;
	int r(width());
	int b(height());
	
	QRegion mask(widget()->rect());

	// Remove top-left corner.
	if(cornersFlag) {
		mask -= QRegion(0, 0, 5, 1);
		mask -= QRegion(0, 1, 3, 1);
		mask -= QRegion(0, 2, 2, 1);
		mask -= QRegion(0, 3, 1, 2);
	
	// Remove top-right corner.
		mask -= QRegion(r - 5, 0, 5, 1);
		mask -= QRegion(r - 3, 1, 3, 1);
		mask -= QRegion(r - 2, 2, 2, 1);
		mask -= QRegion(r - 1, 3, 1, 2);
	
	// Remove bottom-left corner.
		mask -= QRegion(0, b - 5, 1, 3);
		mask -= QRegion(0, b - 3, 2, 1);
		mask -= QRegion(0, b - 2, 3, 1);
		mask -= QRegion(0, b - 1, 5, 1);
	
	// Remove bottom-right corner.
		mask -= QRegion(r - 5, b - 1, 5, 1);
		mask -= QRegion(r - 3, b - 2, 3, 1);
		mask -= QRegion(r - 2, b - 3, 2, 1);
		mask -= QRegion(r - 1, b - 5, 1, 2);
	}
	int paddingLeft, paddingRight, paddingTop, paddingBottom;
	padding(paddingLeft, paddingRight, paddingTop, paddingBottom);	
	mask.translate(paddingLeft, paddingRight);
	
	return mask;
}

CrystalButton* CrystalClient::addButtons(QBoxLayout *layout, const QString& s)
{
	ButtonImage *bitmap;
	QString tip;
	CrystalButton *lastone=NULL;

	if (s.length() > 0) 
	{
		for (int n=0; n < s.length(); n++)
		{
			CrystalButton *current=NULL;
			switch (s[n].toAscii()) {
			case 'M': // Menu button
				if (!button[MenuButton]) {
					button[MenuButton] = current = new CrystalButton(this, "menu", i18n("Menu"), MenuButton, ::factory->buttonImages[ButtonImageMenu]);
					connect(button[MenuButton], SIGNAL(pressed()), this, SLOT(menuButtonPressed()));
				}
				break;

			case 'S': // Sticky button
				if (!button[OnAllDesktopsButton]) {
					if (isOnAllDesktops()) {
						bitmap = ::factory->buttonImages[ButtonImageSticky];
						tip = i18n("Not on all desktops");
					} else {
						bitmap = ::factory->buttonImages[ButtonImageUnSticky];
						tip = i18n("On All Desktops");
					}
					button[OnAllDesktopsButton] =current=new CrystalButton(this, "sticky", tip,OnAllDesktopsButton, bitmap);
					connect(button[OnAllDesktopsButton], SIGNAL(clicked()),this, SLOT(toggleOnAllDesktops()));
				}
				break;

			case 'H': // Help button
				if (providesContextHelp()) {
					button[HelpButton] =current=
						new CrystalButton(this, "help", i18n("Help"),HelpButton, ::factory->buttonImages[ButtonImageHelp]);
					connect(button[HelpButton], SIGNAL(clicked()),this, SLOT(showContextHelp()));
				}
				break;

			case 'I': // Minimize button
				if ((!button[MinButton]) && isMinimizable())  {
					button[MinButton] =current=
						new CrystalButton(this, "iconify", i18n("Minimize"), MinButton, ::factory->buttonImages[ButtonImageMin]);
					connect(button[MinButton], SIGNAL(clicked()),this, SLOT(minButtonPressed()));
				}
				break;

			case 'F': // Above button
				if (!button[AboveButton]) {
					button[AboveButton] =current=
						new CrystalButton(this, "above", i18n("Keep Above Others"), AboveButton, ::factory->buttonImages[keepAbove()?ButtonImageUnAbove:ButtonImageAbove]);
					connect(button[AboveButton], SIGNAL(clicked()),this, SLOT(aboveButtonPressed()));
				}
				break;
		
			case 'B': // Below button
				if ((!button[BelowButton])) {
					button[BelowButton] =current=
						new CrystalButton(this, "below", i18n("Keep Below Others"),BelowButton, ::factory->buttonImages[keepBelow()?ButtonImageUnBelow:ButtonImageBelow]);
					connect(button[BelowButton], SIGNAL(clicked()),this, SLOT(belowButtonPressed()));
				}
				break;

			case 'L': // Shade button
				if ((!button[ShadeButton]) && isShadeable())  {
					button[ShadeButton] =current=
						new CrystalButton(this, "shade", i18n("Shade"), ShadeButton, ::factory->buttonImages[ButtonImageShade]);
					connect(button[ShadeButton], SIGNAL(clicked()),this, SLOT(shadeButtonPressed()));
				}
				break;
		  
			case 'A': // Maximize button
				if ((!button[MaxButton]) && isMaximizable()) 
				{
					if (maximizeMode() == MaximizeFull) 
					{
						bitmap = ::factory->buttonImages[ButtonImageRestore];
						tip = i18n("Restore");
					} else {
						bitmap = ::factory->buttonImages[ButtonImageMax];
						tip = i18n("Maximize");
					}
					button[MaxButton]  =current=
						new CrystalButton(this, "maximize", tip, MaxButton, bitmap);
					connect(button[MaxButton], SIGNAL(clicked()),this, SLOT(maxButtonPressed()));
				}
				break;

			case 'X': // Close button
				if (isCloseable()) {
					button[CloseButton] =current=
					new CrystalButton(this, "close", i18n("Close"), CloseButton, ::factory->buttonImages[ButtonImageClose]);
					connect(button[CloseButton], SIGNAL(clicked()),this, SLOT(closeButtonPressed()));
				}
				break;

			case '_': // Spacer item
				layout->addSpacing(4);
				current=NULL;
				break;
			}
			
			if (current)
			{
				layout->addWidget(current);
				if (layout->indexOf(current)==0)current->setFirstLast(true,false);
			}
			lastone=current;
		}
	}
	return lastone;
}

void CrystalClient::activeChange()
{
	Repaint();
	if (isActive()) ::factory->clients.at(::factory->clients.indexOf(this));
}

void CrystalClient::captionChange()
{
	if (::factory->drawcaption) widget()->repaint(titlebar_->geometry());
	widget()->setToolTip(caption());
}

void CrystalClient::desktopChange()
{
	bool d = isOnAllDesktops();
	if (button[OnAllDesktopsButton]) {
		button[OnAllDesktopsButton]->setBitmap(::factory->buttonImages[d ? ButtonImageSticky : ButtonImageUnSticky ]);
		button[OnAllDesktopsButton]->setToolTip(d ? i18n("Not on all desktops") : i18n("On All Desktops"));
	}
}

void CrystalClient::iconChange()
{
	if (button[MenuButton]) {
		button[MenuButton]->setBitmap(::factory->buttonImages[ButtonImageMenu]);
	}
}

void CrystalClient::maximizeChange()
{
	bool m = (maximizeMode() == MaximizeFull);

	if (button[MaxButton]) {
		button[MaxButton]->setBitmap(::factory->buttonImages[ m ? ButtonImageRestore : ButtonImageMax ]);
		button[MaxButton]->setToolTip(m ? i18n("Restore") : i18n("Maximize"));
	}
		
	if (!options()->moveResizeMaximizedWindows())
	{
		FullMax=m;
		updateLayout();
		Repaint();
	}
}

void CrystalClient::updateLayout()
{
	if (FullMax)
	{
		mainlayout->setColumnMinimumWidth(0,0);
		mainlayout->setColumnMinimumWidth(2,0);
	}else{
		mainlayout->setColumnMinimumWidth(2,borderSpacing());
		mainlayout->setColumnMinimumWidth(0,borderSpacing());
	}
	
	mainlayout->setRowMinimumHeight(0, (FullMax||::factory->buttontheme==5)?0:1);
	for (int i=0; i < NumButtons; i++)if (button[i])
		button[i]->resetSize(FullMax);
	widget()->layout()->activate();
}

int CrystalClient::borderSpacing()
{
	if (::factory->roundCorners)
		return (::factory->borderwidth <= 5) ? 5 : ::factory->borderwidth;
	return (::factory->borderwidth <= 1) ? 1 : ::factory->borderwidth;
}

void CrystalClient::shadeChange()
{ 
	if (button[ShadeButton])
	{
		button[ShadeButton]->setBitmap(::factory->buttonImages[isShade()?ButtonImageUnShade:ButtonImageShade]);
	}
	Repaint();
}

void CrystalClient::borders(int &l, int &r, int &t, int &b) const
{
	l = r = ::factory->borderwidth;
	t = ::factory->titlesize;
	if (!isShade())b = ::factory->borderwidth; else b=0;
	
	if (!options()->moveResizeMaximizedWindows() )
	{
		if ( (maximizeMode() & MaximizeFull)==MaximizeFull)
			l=r=b=0;
	}
}

void CrystalClient::padding(int &left, int &right, int &top, int &bottom) const
{
    left = 15;
    right = 15;
    top = 15;
    bottom = 15;
  
}

void CrystalClient::resize(const QSize &size)
{
	widget()->resize(size);
}

QSize CrystalClient::minimumSize() const
{
	return widget()->minimumSize();
}

KDecoration::Position CrystalClient::mousePosition(const QPoint &point) const
{
	int paddingLeft, paddingTop, paddingRight, paddingBottom;
	const int corner = 20;
	padding(paddingLeft, paddingRight, paddingTop, paddingBottom);
	int px = point.x() - paddingLeft;
	int py = point.y() - paddingTop;
	Position pos;
	const int RESIZESIZE=::factory->borderwidth;
	
	
	if (isShade() || !isResizable()) pos=PositionCenter; 
	else if (py <= 3) {
		// inside top frame
		if (px <= corner)   pos = PositionTopLeft;
		else if (px >= (width()-corner))  pos = PositionTopRight;
		else                                     pos = PositionTop;
	} else if (py >= (height()-RESIZESIZE)) {
		// inside handle
		if (px <= corner)                 pos = PositionBottomLeft;
		else if (px >= (width()-corner))  pos = PositionBottomRight;
		else                                     pos = PositionBottom;
	} else if (px <= RESIZESIZE) {
		// on left frame
		if (py <= corner)                 pos = PositionTopLeft;
		else if (py >= (height()-corner)) pos = PositionBottomLeft;
		else                                     pos = PositionLeft;
	} else if (px >= width()-RESIZESIZE) {
		// on right frame
		if (py <= corner)                 pos = PositionTopRight;
		else if (py >= (height()-corner)) pos = PositionBottomRight;
		else                                     pos = PositionRight;
	} else {
		// inside the frame
		pos = PositionCenter;
	}
	return pos;
}

bool CrystalClient::eventFilter(QObject *obj, QEvent *e)
{
	if (obj != widget()) {
		return false;
	}
	
	switch (e->type()) {
	case QEvent::MouseButtonDblClick:
		mouseDoubleClickEvent(static_cast<QMouseEvent *>(e));
		return true;
	case QEvent::MouseButtonPress:
		if (!mousePressEvent(static_cast<QMouseEvent *>(e)))
			processMousePressEvent(static_cast<QMouseEvent *>(e));
		return true;
	case QEvent::MouseButtonRelease:
		return mouseReleaseEvent(static_cast<QMouseEvent *>(e));
	case QEvent::MouseMove:
		return mouseMoveEvent(static_cast<QMouseEvent *>(e));
		
	case QEvent::Paint:
		paintEvent(static_cast<QPaintEvent *>(e));
		return true;
	case QEvent::Wheel:
		mouseWheelEvent(static_cast<QWheelEvent *>(e));
		return true;

	case QEvent::Resize: 
		resizeEvent(static_cast<QResizeEvent *>(e));
		return true;

	case QEvent::Show:
		showEvent(static_cast<QShowEvent *>(e));
		return true;
	case QEvent::Move:
		moveEvent(static_cast<QMoveEvent *>(e));
		return true;
		
	case QEvent::DragEnter:
		dragEnterEvent( static_cast<QDragEnterEvent*>( e ));
		return true;
	case QEvent::DragMove:
		dragMoveEvent( static_cast<QDragMoveEvent*>( e ));
		return true;
	case QEvent::DragLeave:
		dragLeaveEvent( static_cast<QDragLeaveEvent*>( e ));
		return true;
	case QEvent::Drop:
		dropEvent( static_cast<QDropEvent*>( e ));
		return true;			
		
	default:return false;
	}

	return false;
}

bool CrystalClient::mousePressEvent(QMouseEvent *e)
{
	int item = itemClicked( widget()->mapToParent( e->pos() ) );
	if( item >= 0 && (buttonToWindowOperation( e->button() ) != OperationsOp) )
	{
		click_in_progress = true;
		mousebutton = e->button();
		return true;
	}
	click_in_progress = false;
	return false;
}

bool CrystalClient::mouseReleaseEvent(QMouseEvent *e)
{
	int item = itemClicked( e->pos() );
	if( click_in_progress && item >= 0 )
	{
		click_in_progress = false;
		setCurrentTab(tabId(item));
		return true;
	}
	click_in_progress = false;
	return false;
}

void CrystalClient::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (/*(titlebar_->geometry().contains(e->pos()))&&*/(e->button()==Qt::LeftButton)) titlebarDblClickOperation();
	else {
		QMouseEvent me(QEvent::MouseButtonPress,e->pos(),e->button(),e->buttons(),e->modifiers());
		processMousePressEvent(&me);
	}
}

void CrystalClient::mouseWheelEvent(QWheelEvent *e)
{
	if (::factory->wheelTask)
	{
		QList<CrystalClient*> *l=&(::factory->clients);
		QList<CrystalClient*> ::iterator i = l->begin(), activeOne;
		Window client;
		if (l->begin() == l->end()) return;
		activeOne = l->begin();
		while (i < l->end())
		{
		    if (*i == NULL) return;
		    if ((*i)->isActive()) activeOne = i;
		    i++;
		}
		i = activeOne;

		do
		{
			if(e->delta()>0)
			{
				i++;
				if (i >= l->end()) i = l->begin();
			}else{
				i--;
				if (i < l->begin()) i = l->end()-1;
			}
			if (*i == NULL) {
				printf("*i == NULL\n");
				return;
			}
			
			client = (*i)->windowId();
			if (client == 0) { // FALLBACK
				titlebarMouseWheelOperation(e->delta());
				return;
			}
			KWindowInfo info=KWindowSystem::windowInfo(client, NET::WMState|NET::XAWMState);
			if (((*i)->desktop()==desktop()) && !info.isMinimized())break;
		}while(i != activeOne);
			
		if (client)
			KWindowSystem::forceActiveWindow(client);
	}else{
		titlebarMouseWheelOperation(e->delta());
	}
}

void CrystalClient::paintTab(QPainter &painter, const QRect &rect, int idx, bool active, bool drawSeparators)
{
	WND_CONFIG* wndcfg=(active?&::factory->active:&::factory->inactive);
	QColor color = options()->color(KDecoration::ColorTitleBar, active);
	QColor c1, c2;
	
	if (compositingActive()) {
		color.setAlpha((wndcfg->transparency*255)/100);
	}
	
	/* Draw transparent background */
	painter.fillRect(rect, color);
	
	/* Draw Overlay */
	if (!wndcfg->overlay.isNull())
	{
		if (wndcfg->stretch_overlay == false)
			painter.drawTiledPixmap(rect,wndcfg->overlay);
		else
			painter.drawPixmap(rect, wndcfg->overlay, wndcfg->overlay.rect());
	}
	
	
	
	/* Draw caption */
	if (::factory->drawcaption && idx >= 0)
	{
		long id;
	
		id = tabId(idx);
//		printf("tab %d id = %ld, title = %s\n", idx, id, caption(id).toLocal8Bit().data());
		
		// draw title text
		painter.setFont(options()->font(active, false));
	
		QColor color=options()->color(KDecoration::ColorFont, active);
		QRect r=rect;
		r.setWidth(r.width()-2);
		r.translate(1,0);
		
		int logowidth=::factory->logo.width()+::factory->logoDistance;
		if (::factory->logoEnabled!=1 && (active ||!::factory->logoActive))
		{
			r.setWidth(r.width()-logowidth);
			if (::factory->logoEnabled==0)r.moveLeft(r.left()+logowidth);
		}
		QFontMetrics metrics(options()->font(active, false));
		int textwidth=metrics.width(caption(idx));
		int textalign=CrystalFactory::titleAlign();
		if (textwidth>r.width())
			textalign=Qt::AlignLeft, textwidth=r.width();
		if (::factory->textshadow && active)
		{
			painter.translate(1,1);
			painter.setPen(color.dark(500));
			painter.drawText(r, textalign | Qt::AlignVCenter, caption(idx));
			painter.translate(-1,-1);
		}
	
		painter.setPen(color);
		painter.drawText(r,
			textalign | Qt::AlignVCenter,
			caption(idx));

		/* Draw Logo */
		if (::factory->logoEnabled!=1 && (active ||!::factory->logoActive))
		{
			int x=0;
			if (::factory->logoEnabled==0 && textalign==Qt::AlignLeft)x=r.left()-logowidth;
			if (::factory->logoEnabled==2 && textalign==Qt::AlignLeft)x=r.left()+textwidth+::factory->logoDistance;

			if (::factory->logoEnabled==0 && textalign==Qt::AlignRight)x=r.right()-textwidth-logowidth;
			if (::factory->logoEnabled==2 && textalign==Qt::AlignRight)x=r.right()+::factory->logoDistance;

			if (::factory->logoEnabled==0 && textalign==Qt::AlignHCenter)x=(r.right()+r.left()-textwidth)/2-logowidth;
			if (::factory->logoEnabled==2 && textalign==Qt::AlignHCenter)x=(r.right()+r.left()+textwidth)/2+::factory->logoDistance;
			painter.drawPixmap(x,(::factory->titlesize-::factory->logo.height())/2,::factory->logo);
		}
	}else 
	  /* Draw Logo without titlebar */
	  if (::factory->logoEnabled!=1 && (active||!::factory->logoActive)) {
		int x=0;
		if (::factory->logoEnabled==0) x=rect.left();
		if (::factory->logoEnabled==2) x=rect.right()-::factory->logo.width();
		painter.drawPixmap(x,(::factory->titlesize-::factory->logo.height())/2,::factory->logo);

	}
	
	if (drawSeparators) {
		/* Separators between tabs */
		painter.setPen(separatorColorHi);
		painter.drawLine(rect.left(), rect.top() + 1, rect.left(), rect.bottom());

		painter.setPen(separatorColorLo);
		painter.drawLine(rect.right(), rect.top() + 1, rect.right(), rect.bottom());
	}
}

void CrystalClient::paintShadow(QPainter &painter) {
	int paddingLeft, paddingRight, paddingTop, paddingBottom;
	padding(paddingLeft, paddingRight, paddingTop, paddingBottom);
	/* padding is assumed to be 15 */
	
	int shadowSize = 12;

	QColor c1(0, 0, 0, 0);
	QColor c2(0, 0, 0, 24);
	QColor c3(0, 0, 0, 64);

	painter.save();
	painter.translate(0, 1);
	
	QLinearGradient linearGrad(QPointF(paddingLeft - shadowSize, 0), QPointF(paddingLeft, 0));
	linearGrad.setColorAt(0, c1);
	linearGrad.setColorAt(0.7, c2);
	linearGrad.setColorAt(1, c3);
	/* left + center */
	{
		QBrush brush(linearGrad);
	
		painter.fillRect(QRect(paddingLeft - shadowSize, paddingTop, 
			width() + shadowSize,
			height()),
			brush);
	}
	
	/* right */
	linearGrad.setStart(paddingLeft + width() + shadowSize, 0);
	linearGrad.setFinalStop(paddingLeft + width(), 0);
	{
		QBrush brush(linearGrad);

		painter.fillRect(QRect(paddingLeft + width(), paddingTop, 
			shadowSize,
			height()),
			brush);
	}
	
	/* top */
	linearGrad.setStart(0, paddingTop - shadowSize);
	linearGrad.setFinalStop(0, paddingTop);
	{
		QBrush brush(linearGrad);

		painter.fillRect(QRect(paddingLeft, paddingTop - shadowSize, 
			width(),
			shadowSize),
			brush);
	}
	
	/* bottom */
	linearGrad.setStart(0, paddingTop + height() + shadowSize);
	linearGrad.setFinalStop(0, paddingTop + height());
	{
		QBrush brush(linearGrad);

		painter.fillRect(QRect(paddingLeft, height() + paddingTop, 
			width(),
			height() + paddingTop + shadowSize),
			brush);
	}
	QRadialGradient radialGrad(paddingLeft, paddingTop, shadowSize);
	radialGrad.setColorAt(0, c3);
	radialGrad.setColorAt(0.3, c2);
	radialGrad.setColorAt(1, c1);
	
	/* top-left corner */
	{
		QBrush brush(radialGrad);
		painter.fillRect(
			QRect(
				paddingLeft - shadowSize, paddingTop - shadowSize, 
				shadowSize, shadowSize
			),
			brush);	
		
	}

	/* top-right corner */
	{
		radialGrad.setCenter(paddingLeft + width(), paddingTop);
		radialGrad.setFocalPoint(paddingLeft + width(), paddingTop);
		QBrush brush(radialGrad);
		painter.fillRect(
			QRect(
				paddingLeft + width(), paddingTop - shadowSize, 
				shadowSize, shadowSize
			),
			brush);	
		
	}
	
	/* bottom-left corner */
	{
		radialGrad.setCenter(paddingLeft, paddingTop + height());
		radialGrad.setFocalPoint(paddingLeft, paddingTop + height());
		QBrush brush(radialGrad);
		painter.fillRect(
			QRect(
				paddingLeft - shadowSize, paddingTop + height(), 
				shadowSize, shadowSize
			),
			brush);	
		
	}

	/* bottom-right corner */
	{
		radialGrad.setCenter(paddingLeft + width(), paddingTop + height());
		radialGrad.setFocalPoint(paddingLeft + width(), paddingTop + height());
		QBrush brush(radialGrad);
		painter.fillRect(
			QRect(
				paddingLeft + width(), paddingTop + height(), 
				shadowSize, shadowSize
			),
			brush);	
		
	}

	painter.restore();
}

void CrystalClient::paintEvent(QPaintEvent* event)
{
	if (!CrystalFactory::initialized()) return;

	if (compositingActive()) {
		QPainter painter(widget());
		paint(painter);
	} else {
		QPixmap pixmap( widget()->size() );
		{
			QPainter painter( &pixmap );
			painter.setClipRegion( event->region() );
			paint( painter );
			
			QList<CrystalButton*> buttons( widget()->findChildren<CrystalButton*>() );
			foreach( CrystalButton* button, buttons )
			{
				if( event->rect().intersects( button->geometry() ) )
				{ button->update(); }
			}
		}
		
		
		QPainter painter( widget() );
		painter.drawPixmap( QPoint(), pixmap );
		
	}
}

void CrystalClient::paint(QPainter &painter) {
	int paddingLeft, paddingRight, paddingBottom, paddingTop;
	padding(paddingLeft, paddingRight, paddingTop, paddingBottom);


	// draw the titlebar
	WND_CONFIG* wndcfg=(isActive()?&::factory->active:&::factory->inactive);

	QRect r;
	int i;
	
	QColor color = options()->color(KDecoration::ColorTitleBar, isActive());
	if (compositingActive()) {
		paintShadow(painter);
		color.setAlpha((wndcfg->transparency*255)/100);
	}
	painter.setClipRegion(getMask());
	
	int bl, br, bt, bb;
	borders(bl, br, bt, bb);
	
	QRect allTabs = titlebar_->geometry();
	allTabs.setTop(0);
	int tabCount = this->tabCount();
	
	if (drag_in_progress && sourceTab == -1)
		tabCount++;
	if (drag_in_progress && tabCount > 1 && sourceTab != -1 && targetTab == -1)
		tabCount--;
	
	if (tabCount>1) {
		allTabs.setLeft(allTabs.left()+3);
		allTabs.setRight(allTabs.right()-3);
	}
	
	painter.fillRect(QRect(paddingLeft,paddingTop,allTabs.x() - paddingLeft, bt), color);
	painter.fillRect(QRect(allTabs.x()+allTabs.width(),
			       paddingTop,
			       widget()->width()-(allTabs.x()+allTabs.width()) - paddingRight,
			       bt), 
			 color);
	painter.fillRect(QRect(paddingLeft, bt + paddingTop, bl, widget()->height() - bt - paddingBottom - paddingTop), color);
	painter.fillRect(QRect(widget()->width()-br - paddingRight,bt + paddingTop ,br,widget()->height() - bt - bb - paddingBottom - paddingTop), color);
	painter.fillRect(QRect(bl + paddingLeft ,widget()->height()-bb - paddingBottom,widget()->width()-bl - paddingRight - paddingLeft,bb), color);
	
	/* Draw Overlays */
	if (!wndcfg->overlay.isNull())
	{
		if (wndcfg->stretch_overlay == false) {
			painter.drawTiledPixmap(QRect(paddingLeft,paddingTop,allTabs.x() - paddingLeft,bt), wndcfg->overlay);
			painter.drawTiledPixmap(QRect(allTabs.x()+allTabs.width(),
			       paddingTop,
			       widget()->width()-(allTabs.x()+allTabs.width()) - paddingRight,
			       bt), 
			   wndcfg->overlay);
		}
		else {} ; /* NOT IMPLEMENTED YET */
// 			painter.drawPixmap(rect, wndcfg->overlay, wndcfg->overlay.rect());
	}
		

	
	int tabIndex = 0;
	
	for (i=0; i<tabCount; i++) {
		int tabwidth = allTabs.width() / tabCount;
		int idx;
		bool active = (tabId(i) == currentTabId());
		if (tabCount == 1)
			active = isActive();
		
		if (i == targetTab && drag_in_progress) {
			idx = -1;
			if (sourceTab != -1) {
				idx = sourceTab;
				tabIndex++;
			}
			active = true;
		} else if (this->tabCount() > 1 && i == sourceTab && drag_in_progress && targetTab == -1) {
			tabIndex++;
			idx = tabIndex;
			tabIndex++;
		} else {
			idx = tabIndex;
			tabIndex++;
			if (drag_in_progress && targetTab != -1)
				active = false;
		}

		r = QRect(i * tabwidth + allTabs.x(), paddingTop, tabwidth, bt);
		if( i == tabCount - 1 )
			r.setWidth( allTabs.width() - r.left() + allTabs.x());
		paintTab(painter, r, idx, active, tabCount > 1);
	}
	
	if (tabCount > 1) {
		painter.setPen(separatorColorLo);
		painter.drawLine(allTabs.left()-1, 1 + paddingTop, allTabs.left()-1, allTabs.bottom());
		painter.setPen(separatorColorHi);
		painter.drawLine(allTabs.right()+1, 1 + paddingTop, allTabs.right()+1, allTabs.bottom());
	}
	

	/* Draw Inline */
	if (!isShade() && wndcfg->inlineMode >=1 )
	{
		QRect r;
		int bl,br,bt,bb;
		borders(bl,br,bt,bb);

		if (wndcfg->inlineMode==1) {
			painter.setPen(wndcfg->inlineColor);
			painter.drawRect(bl-1 + paddingLeft,bt-1 + paddingTop,widget()->width()-bl-br-paddingRight+2,widget()->height()-bt-bb+2-paddingBottom);
		}
		if (wndcfg->inlineMode==2) {
			painter.setPen(wndcfg->inlineColor.dark(150));
			painter.drawLine(bl-1+paddingLeft,bt-1+paddingTop,widget()->width()-br-paddingRight,bt-1-paddingBottom);
			painter.drawLine(bl-1+paddingLeft,bt-1+paddingTop,bl-1+paddingLeft,widget()->height()-bb-paddingBottom);
			painter.setPen(wndcfg->inlineColor.light(150));
			painter.drawLine(widget()->width()-br-paddingRight,bt-1+paddingTop,widget()->width()-br-paddingRight,widget()->height()-bb-paddingBottom);
			painter.drawLine(bl-1+paddingLeft,widget()->height()-bb-paddingBottom,widget()->width()-br-1-paddingRight,widget()->height()-bb-paddingBottom);
		}
		if (wndcfg->inlineMode==3) {
			painter.setPen(wndcfg->inlineColor.light(150));
			painter.drawLine(bl-1+paddingLeft,bt-1+paddingTop,widget()->width()-br-paddingRight,bt-1-paddingBottom);
			painter.drawLine(bl-1+paddingLeft,bt-1+paddingTop,bl-1+paddingLeft,widget()->height()-bb-paddingBottom);
			painter.setPen(wndcfg->inlineColor.dark(150));
			painter.drawLine(widget()->width()-br-paddingRight,bt-1+paddingTop,widget()->width()-br-paddingRight,widget()->height()-bb-paddingBottom);
			painter.drawLine(bl-1+paddingLeft,widget()->height()-bb-paddingBottom,widget()->width()-br-1-paddingRight,widget()->height()-bb-paddingBottom);
		}
	}
	
	/* Draw outline frame */
	if (wndcfg->outlineMode && 
	    (options()->moveResizeMaximizedWindows() || isShade() || (maximizeMode() & MaximizeFull)!=MaximizeFull))
	{
		// outline the frame
		QColor c1,c2;
		c1=c2=wndcfg->frameColor;
		if (wndcfg->outlineMode==2)c1=c1.dark(140),c2=c2.light(140);
		if (wndcfg->outlineMode==3)c1=c1.light(140),c2=c2.dark(140);


		if ((::factory->roundCorners) && !(!options()->moveResizeMaximizedWindows() && maximizeMode() & MaximizeFull))
		{
			int l = paddingLeft;
			int t = paddingTop;
			int r=(paddingLeft + width()-1);
			int b=(paddingTop + height()-1);
			
			// Draw edge of top-left corner inside the area removed by the mask.
			painter.setPen(c1);
			painter.drawLine(l+5,t+0,r-5,t+0);
			painter.drawLine(l+0,t+5,l+0,b-5);

			painter.setPen(c2);
			painter.drawLine(r,t+5,r,b-5);
			painter.drawLine(l+5,b,r-5,b);

			painter.setPen(c1);
			painter.drawPoint(l+3, t+1);
			painter.drawPoint(l+4, t+1);
			painter.drawPoint(l+2, t+2);
			painter.drawPoint(l+1, t+3);
			painter.drawPoint(l+1, t+4);
		
			// Draw edge of top-right corner inside the area removed by the mask.
			painter.setPen(c1);
			painter.drawPoint(r - 4, t+1);
			painter.drawPoint(r - 3, t+1);
			painter.drawPoint(r - 2, t+2);
			painter.drawPoint(r - 1, t+3);
			painter.drawPoint(r - 1, t+4);

			// Draw edge of bottom-left corner inside the area removed by the mask.
			painter.setPen(c2);
			painter.drawPoint(l+1, b - 4);
			painter.drawPoint(l+1, b - 3);
			painter.drawPoint(l+2, b - 2);
			painter.drawPoint(l+3, b - 1);
			painter.drawPoint(l+4, b - 1);
		
			// Draw edge of bottom-right corner inside the area removed by the mask.
			painter.setPen(c2);
			painter.drawPoint(r - 1, b - 4);
			painter.drawPoint(r - 1, b - 3);
			painter.drawPoint(r - 2, b - 2);
			painter.drawPoint(r - 3, b - 1);
			painter.drawPoint(r - 4, b - 1);
		}else {
			QRect r=widget()->rect().adjusted(paddingLeft, paddingTop, -paddingRight, -paddingBottom);
		  
			painter.setPen(c1);
			painter.drawLine(r.left(),r.top(),r.right(),r.top());
			painter.drawLine(r.left(),r.top(),r.left(),r.bottom());

			painter.setPen(c2);
			painter.drawLine(r.right(),r.top(),r.right(),r.bottom());
			painter.drawLine(r.left(),r.bottom(),r.right(),r.bottom());
		}
	}
}

void CrystalClient::resizeEvent(QResizeEvent *e)
{
	if (e->size()!=e->oldSize())
	{
		updateMask();
	}
}

void CrystalClient::moveEvent(QMoveEvent *)
{
  
}

bool CrystalClient::mouseMoveEvent(QMouseEvent *e)
{
	QPoint c = e->pos();
	int item = itemClicked( c );
	if( item >= 0 && click_in_progress && buttonToWindowOperation( mousebutton ) == TabDragOp)
        {
		int bl,br,bt,bb;
		borders(bl, br, bt, bb);
		
		click_in_progress = false;
		drag_in_progress = true;
		QDrag *drag = new QDrag( widget() );
		QMimeData *group_data = new QMimeData();
		group_data->setData( tabDragMimeType(), QString().setNum( tabId( item )).toAscii() );
		drag->setMimeData( group_data );
				
		sourceTab = item;

		// Create draggable tab pixmap
 		QRect frame( 0, 0, titlebar_->geometry().width()/tabCount(), bt );

		QImage pix( frame.size(), QImage::Format_ARGB32);
		QPainter painter( &pix );

		QPainter::CompositionMode old = painter.compositionMode();
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		

		painter.fillRect(frame, QColor(128,128,128,255));
		
		painter.setCompositionMode(old);
		paintTab( painter, frame, item, isActive(), true);
		painter.setPen(separatorColorHi);
		painter.drawLine(frame.left(), 0, frame.right(), 0);
		painter.setPen(separatorColorLo);
		painter.drawLine(frame.left(), frame.bottom(), frame.right(), frame.bottom());

		drag->setPixmap( QPixmap::fromImage(pix) );

		drag->setHotSpot( QPoint( c.x() - (titlebar_->geometry().x() + item * (titlebar_->geometry().width()/tabCount())), -1 ));

		drag->exec( Qt::MoveAction );
		drag_in_progress = false;
		
		if( drag->target() == 0 && tabCount() > 1 )
		{ // Remove window from group and move to where the cursor is located
			QPoint pos = QCursor::pos();
			frame = widget()->geometry();
			frame.moveTo( pos.x() - c.x(), pos.y() - c.y() );
			untab( tabId(sourceTab), frame );
		}
		return true;
        }
	return false;
}

void CrystalClient::showEvent(QShowEvent *)
{

}

void CrystalClient::Repaint()
{
	widget()->repaint();
	for (int n=0; n<NumButtons; n++)
		if (button[n])
			button[n]->reset();
}

void CrystalClient::maxButtonPressed()
{
	if (button[MaxButton])
	{
		switch (button[MaxButton]->lastMousePress()) 
		{
		case Qt::MidButton:
			maximize(maximizeMode() ^ MaximizeVertical);
			break;
		case Qt::RightButton:
			maximize(maximizeMode() ^ MaximizeHorizontal);
			break;
		default:
			maximize((maximizeMode() == MaximizeFull) ? MaximizeRestore: MaximizeFull);
		}
	}
}

void CrystalClient::minButtonPressed()
{
	if (button[MinButton]) {
		switch (button[MinButton]->lastMousePress()) {
		case Qt::MidButton:{
			performWindowOperation(LowerOp);
			break;
		}
		case Qt::RightButton:
			if (isShadeable()) setShade(!isShade());
			break;
		default:
			minimize();
		}
	}
}

void CrystalClient::aboveButtonPressed()
{
	setKeepAbove(!keepAbove());
}

void CrystalClient::belowButtonPressed()
{
	setKeepBelow(!keepBelow());
}

void CrystalClient::keepAboveChange(bool /*set*/)
{
	if (button[AboveButton])
	{
		button[AboveButton]->setBitmap(::factory->buttonImages[keepAbove()?ButtonImageUnAbove:ButtonImageAbove]);
	}
}

void CrystalClient::keepBelowChange(bool /*set*/)
{
	if (button[BelowButton])
	{
		button[BelowButton]->setBitmap(::factory->buttonImages[keepBelow()?ButtonImageUnBelow:ButtonImageBelow]);
	}
}

void CrystalClient::closeButtonPressed()
{
	if (button[CloseButton])
	switch (button[CloseButton]->lastMousePress()) {
		case Qt::RightButton:
		{
			Window client;
			char param[20];
			client = windowId();
			if (client != 0) {
				KProcess *proc = new KProcess;

				*proc << "kdocker";
				sprintf(param,"0x%lx",client);
				*proc << "-w" << param;
				proc->start();
			} else { /* Sorry man */ }
			break;
		}
		default:
			closeWindow();
			break;
	}
}

void CrystalClient::shadeButtonPressed()
{
	if (button[ShadeButton]) {
		switch (button[ShadeButton]->lastMousePress()) {
		case Qt::MidButton:
		case Qt::RightButton:
			break;
		default:
			if (isShadeable()) setShade(!isShade());
		}
	}
}

void CrystalClient::menuButtonPressed()
{
	if (!button[MenuButton])return;
	
	static QTime* t = 0;
	static CrystalClient* lastClient = 0;
	if (t == 0) 
		t = new QTime;
	bool dbl = (lastClient == this && t->elapsed() <= QApplication::doubleClickInterval());
	lastClient = this;
	t->start();
	
	if (dbl)
	{	// Double Click on Symbol, close window
		closeWindow();
		return;
	}

	menuPopUp();
}

void CrystalClient::menuPopUp()
{
	KDecorationFactory* f = factory();
	showWindowMenu(QRect(button[MenuButton]->mapToGlobal(
	  button[MenuButton]->rect().topLeft()), button[MenuButton]->mapToGlobal(button[MenuButton]->rect().bottomRight())));
	if (!f->exists(this)) return; // decoration was destroyed
	button[MenuButton]->setDown(false);
}

bool CrystalClient::dragEnterEvent( QDragEnterEvent* e )
{
	if( e->source() != 0 && e->mimeData()->hasFormat( tabDragMimeType() ) )
	{
		widget()->update();
		drag_in_progress = true;
		e->acceptProposedAction();
		return true;
	}
	return false;
}

bool CrystalClient::dropEvent( QDropEvent* e )
{
	QPoint point = widget()->mapToParent( e->pos() );
	drag_in_progress = false;
// 	int tabClick = itemClicked( point );
	/* Dropping on border space will append tab at the end */
// 	if( tabClick >= 0 )
	{
		const QMimeData *group_data = e->mimeData();
		if( group_data->hasFormat( tabDragMimeType() ) )
		{
			if( widget() == e->source() )
			{
				int from = sourceTab;
				int item = itemClicked( point, false );
				if (from < item)
					item++;
				if (item == tabCount())
					tab_A_behind_B( tabId(from), tabId(tabCount() -1 ) );
				else
					tab_A_before_B( tabId(from), tabId(item) );
			}
			else
			{
				int item = itemClicked( point, true );
				long source = QString( group_data->data( tabDragMimeType() ) ).toLong();
				if (item == -1) {					
					tab_A_behind_B( source, tabId(tabCount() -1) );
				} else {
					tab_A_before_B( source, tabId(item) );
				}
			}
			widget()->update();
			return true;
		}
	}
	return false; 
}

bool CrystalClient::dragMoveEvent( QDragMoveEvent* e)
{
	if( !e->mimeData()->hasFormat( tabDragMimeType() ) ) return false;
	if (!drag_in_progress) return false;
	
	QPoint point = widget()->mapToParent( e->pos() );

	if (e->source() == widget()) { /* same window, rearrange tab */
		if (tabCount() <= 1)
			targetTab = -1;
		else 
			targetTab = itemClicked( point, false );
		
		if (sourceTab != targetTab && targetTab >= 0 && sourceTab >= 0)
		{
			int item = targetTab;
			if (sourceTab < item)
				item++;
			if (item == tabCount())
				tab_A_behind_B( tabId(sourceTab), tabId(tabCount() -1) );
			else
				tab_A_before_B( tabId(sourceTab), tabId(item) );
			sourceTab = targetTab;

		}
	} else {
		sourceTab = -1;
		targetTab = itemClicked( point, true );
		if (targetTab == -1)
			targetTab = tabCount();
		widget()->update();
	}

	
	return true;
}

bool CrystalClient::dragLeaveEvent( QDragLeaveEvent* )
{
	if (sourceTab == -1)
		drag_in_progress = false;
	targetTab = -1;
	widget()->update();
	return false;
}

int CrystalClient::itemClicked( const QPoint &point, bool between )
{
	QRect frame = widget()->frameGeometry();
	QRect r = titlebar_->geometry();
	r.setTop(0);
	int tabs = tabCount();
	int tabWidth = r.width()/(between?tabs+1:tabs);
	int t_x = r.left();
	int rem = r.width()%tabs;
	int tab_x = t_x;
	for( int i = 0; i < tabs; ++i )
	{
		QRect tabRect( tab_x, r.y(), i<rem?tabWidth+1:tabWidth, r.height() );
		if( tabRect.contains( point ) )
			return i;
		tab_x += tabRect.width();
	}
	return -1;
}

#include "crystalclient.moc"
