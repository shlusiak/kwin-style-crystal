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
	widget()->installEventFilter(this);

	widget()->setAttribute( Qt::WA_StaticContents );
	widget()->setAttribute( Qt::WA_OpaquePaintEvent );
	widget()->setAttribute( Qt::WA_NoSystemBackground );
	widget()->setAutoFillBackground(false);
	
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
	
	mainlayout->setMargin(0);
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
	if ((::factory->roundCorners==0) || (!options()->moveResizeMaximizedWindows() && maximizeMode() & MaximizeFull ) ) 
	{
		setMask(QRegion(widget()->rect()));
		return;
	}
	
	int cornersFlag = ::factory->roundCorners;
	int r(width());
	int b(height());
	
	QRegion mask(widget()->rect());

	// Remove top-left corner.
	if(cornersFlag & TOP_LEFT) {
		mask -= QRegion(0, 0, 5, 1);
		mask -= QRegion(0, 1, 3, 1);
		mask -= QRegion(0, 2, 2, 1);
		mask -= QRegion(0, 3, 1, 2);
	}
	
	// Remove top-right corner.
	if(cornersFlag & TOP_RIGHT) {
		mask -= QRegion(r - 5, 0, 5, 1);
		mask -= QRegion(r - 3, 1, 3, 1);
		mask -= QRegion(r - 2, 2, 2, 1);
		mask -= QRegion(r - 1, 3, 1, 2);
	}
	
	// Remove bottom-left corner.
	if(cornersFlag & BOT_LEFT) {
		mask -= QRegion(0, b - 5, 1, 3);
		mask -= QRegion(0, b - 3, 2, 1);
		mask -= QRegion(0, b - 2, 3, 1);
		mask -= QRegion(0, b - 1, 5, 1);
	}
	
	// Remove bottom-right corner.
	if(cornersFlag & BOT_RIGHT) {
		mask -= QRegion(r - 5, b - 1, 5, 1);
		mask -= QRegion(r - 3, b - 2, 3, 1);
		mask -= QRegion(r - 2, b - 3, 2, 1);
		mask -= QRegion(r - 1, b - 5, 1, 2);
	}
	
	setMask(mask);
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
	const int corner = 20;
	Position pos;
	const int RESIZESIZE=::factory->borderwidth;
	
	if (isShade() || !isResizable()) pos=PositionCenter; 
	else if (point.y() <= 3) {
		// inside top frame
		if (point.x() <= corner)                 pos = PositionTopLeft;
		else if (point.x() >= (width()-corner))  pos = PositionTopRight;
		else                                     pos = PositionTop;
	} else if (point.y() >= (height()-RESIZESIZE)) {
		// inside handle
		if (point.x() <= corner)                 pos = PositionBottomLeft;
		else if (point.x() >= (width()-corner))  pos = PositionBottomRight;
		else                                     pos = PositionBottom;
	} else if (point.x() <= RESIZESIZE) {
		// on left frame
		if (point.y() <= corner)                 pos = PositionTopLeft;
		else if (point.y() >= (height()-corner)) pos = PositionBottomLeft;
		else                                     pos = PositionLeft;
	} else if (point.x() >= width()-RESIZESIZE) {
		// on right frame
		if (point.y() <= corner)                 pos = PositionTopRight;
		else if (point.y() >= (height()-corner)) pos = PositionBottomRight;
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
		setVisibleClientGroupItem( item );
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

void CrystalClient::paintTab(QPainter &painter, const QRect &rect, ClientGroupItem *item, bool active)
{
	WND_CONFIG* wndcfg=(active?&::factory->active:&::factory->inactive);
	QColor color = options()->color(KDecoration::ColorTitleBar, active);
	if (KWindowSystem::compositingActive()) {
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
	if (::factory->drawcaption && item!=NULL)
	{
		// draw title text
		painter.setFont(options()->font(active, false));
	
		QColor color=options()->color(KDecoration::ColorFont, active);
		QRect r=rect;
		int logowidth=::factory->logo.width()+::factory->logoDistance;
		if (::factory->logoEnabled!=1 && (active ||!::factory->logoActive))
		{
			r.setWidth(r.width()-logowidth);
			if (::factory->logoEnabled==0)r.moveLeft(r.left()+logowidth);
		}
		QFontMetrics metrics(options()->font(active, false));
		int textwidth=metrics.width(item->title());
		int textalign=CrystalFactory::titleAlign();
		if (textwidth>r.width())
			textalign=Qt::AlignLeft, textwidth=r.width();
		if (::factory->textshadow && active)
		{
			painter.translate(1,1);
			painter.setPen(color.dark(500));
			painter.drawText(r, textalign | Qt::AlignVCenter, item->title());
			painter.translate(-1,-1);
		}
	
		painter.setPen(color);
		painter.drawText(r,
			textalign | Qt::AlignVCenter,
			item->title());

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
}

void CrystalClient::paintEvent(QPaintEvent*)
{
	if (!CrystalFactory::initialized()) return;

	QPainter painter(widget());

	// draw the titlebar
	WND_CONFIG* wndcfg=(isActive()?&::factory->active:&::factory->inactive);

	QRect r;
	QList< ClientGroupItem > tabList = clientGroupItems();
	int tabCount = tabList.count();
	int i;
	
	QColor color = options()->color(KDecoration::ColorTitleBar, isActive());
	if (KWindowSystem::compositingActive()) {
		color.setAlpha((wndcfg->transparency*255)/100);
	}
	painter.setClipRegion(widget()->rect());
	
	int bl, br, bt, bb;
	borders(bl, br, bt, bb);
	
	QColor c1, c2;
	c1=QColor(180,180,180,128);
	c2=QColor(32, 32, 32, 128);
	QRect allTabs = titlebar_->geometry();
	allTabs.setTop(0);
	
	if (drag_in_progress && sourceTab == -1)
		tabCount++;
	
	if (tabCount>1) {
		allTabs.setLeft(allTabs.left()+3);
		allTabs.setRight(allTabs.right()-3);
		
	}
	
	painter.fillRect(QRect(0,0,allTabs.x(),bt), color);
	painter.fillRect(QRect(allTabs.x()+allTabs.width(),
			       0,
			       widget()->width()-(allTabs.x()+allTabs.width()),
			       bt), 
			 color);
	painter.fillRect(QRect(0,bt,bl,widget()->height()), color);
	painter.fillRect(QRect(widget()->width()-br,bt,br,widget()->height()), color);
	painter.fillRect(QRect(bl,widget()->height()-bb,widget()->width()-bl-br,bb), color);
	
	/* Draw Overlays */
	if (!wndcfg->overlay.isNull())
	{
		if (wndcfg->stretch_overlay == false) {
			painter.drawTiledPixmap(QRect(0,0,allTabs.x(),bt), wndcfg->overlay);
			painter.drawTiledPixmap(QRect(allTabs.x()+allTabs.width(),
			       0,
			       widget()->width()-(allTabs.x()+allTabs.width()),
			       bt), 
			   wndcfg->overlay);
		}
		else {} ; /* NOT IMPLEMENTED YET */
// 			painter.drawPixmap(rect, wndcfg->overlay, wndcfg->overlay.rect());
	}
		

	
	int tabIndex = 0;
	

	if (tabCount > 1) {
		painter.setPen(c1);
		painter.drawLine(allTabs.left(), 1, allTabs.left(), allTabs.bottom());
		painter.drawLine(allTabs.right()+1, 1, allTabs.right()+1, allTabs.bottom());
		painter.setPen(c2);
		painter.drawLine(allTabs.left()-1, 1, allTabs.left()-1, allTabs.bottom());
		painter.drawLine(allTabs.right(), 1, allTabs.right(), allTabs.bottom());
	}
	
	for (i=0; i<tabCount; i++) {
		int tabwidth = allTabs.width() / tabCount;
		ClientGroupItem *item;
		bool active = isActive() && visibleClientGroupItem() == i;
		
		if (i == targetTab && drag_in_progress) {
			item = NULL;
			if (sourceTab != -1) {
				item = &tabList[sourceTab];
				tabIndex++;
			}
			active = true;
		}
		else {
			item = &tabList[tabIndex];
			tabIndex++;
			if (drag_in_progress && targetTab != -1)
				active = false;
		}

		r = QRect(i * tabwidth + allTabs.x(), 0, tabwidth, bt);
		if( i == tabCount - 1 )
			r.setWidth( allTabs.width() - r.left() + allTabs.x());
		paintTab(painter, r, item, active);
		
		/* Separators between tabs */
		if (i > 0) {
			painter.setPen(c1);
			painter.drawLine(r.left(), 1, r.left(), r.bottom());
		}
		if (i < tabCount-1) {
			painter.setPen(c2);
			painter.drawLine(r.right(), 1, r.right(), r.bottom());
		}
	}

	/* Draw Inline */
	if (!isShade() && wndcfg->inlineMode >=1 )
	{
		QRect r;
		int bl,br,bt,bb;
		borders(bl,br,bt,bb);

		if (wndcfg->inlineMode==1) {
			painter.setPen(wndcfg->inlineColor);
			painter.drawRect(bl-1,bt-1,widget()->width()-bl-br+2,widget()->height()-bt-bb+2);
		}
		if (wndcfg->inlineMode==2) {
			painter.setPen(wndcfg->inlineColor.dark(150));
			painter.drawLine(bl-1,bt-1,widget()->width()-br,bt-1);
			painter.drawLine(bl-1,bt-1,bl-1,widget()->height()-bb);
			painter.setPen(wndcfg->inlineColor.light(150));
			painter.drawLine(widget()->width()-br,bt-1,widget()->width()-br,widget()->height()-bb);
			painter.drawLine(bl-1,widget()->height()-bb,widget()->width()-br-1,widget()->height()-bb);
		}
		if (wndcfg->inlineMode==3) {
			painter.setPen(wndcfg->inlineColor.light(150));
			painter.drawLine(bl-1,bt-1,widget()->width()-br,bt-1);
			painter.drawLine(bl-1,bt-1,bl-1,widget()->height()-bb);
			painter.setPen(wndcfg->inlineColor.dark(150));
			painter.drawLine(widget()->width()-br,bt-1,widget()->width()-br,widget()->height()-bb);
			painter.drawLine(bl-1,widget()->height()-bb,widget()->width()-br-1,widget()->height()-bb);
		}
	}


	/* Draw outline frame */
	if (wndcfg->outlineMode && 
	    (options()->moveResizeMaximizedWindows() || isShade() || (maximizeMode() & MaximizeFull)!=MaximizeFull))
	{
		// outline the frame
		QRect r=widget()->rect();
		QColor c1,c2;
		c1=c2=wndcfg->frameColor;
		if (wndcfg->outlineMode==2)c1=c1.dark(140),c2=c2.light(140);
		if (wndcfg->outlineMode==3)c1=c1.light(140),c2=c2.dark(140);

		painter.setPen(c1);
		painter.drawLine(r.left(),r.top(),r.right(),r.top());
		painter.drawLine(r.left(),r.top(),r.left(),r.bottom());

		painter.setPen(c2);
		painter.drawLine(r.right(),r.top(),r.right(),r.bottom());
		painter.drawLine(r.left(),r.bottom(),r.right(),r.bottom());

		if ((::factory->roundCorners) && !(!options()->moveResizeMaximizedWindows() && maximizeMode() & MaximizeFull))
		{
			int cornersFlag = ::factory->roundCorners;
			int r=(width());
			int b=(height());

			// Draw edge of top-left corner inside the area removed by the mask.
			if(cornersFlag & TOP_LEFT) {
				painter.setPen(c1);
				painter.drawPoint(3, 1);
				painter.drawPoint(4, 1);
				painter.drawPoint(2, 2);
				painter.drawPoint(1, 3);
				painter.drawPoint(1, 4);
			}
		
			// Draw edge of top-right corner inside the area removed by the mask.
			if(cornersFlag & TOP_RIGHT) {
				painter.setPen(c1);
				painter.drawPoint(r - 5, 1);
				painter.drawPoint(r - 4, 1);
				painter.drawPoint(r - 3, 2);
				painter.drawPoint(r - 2, 3);
				painter.drawPoint(r - 2, 4);
			}
		
			// Draw edge of bottom-left corner inside the area removed by the mask.
			if(cornersFlag & BOT_LEFT) {
				painter.setPen(c2);
				painter.drawPoint(1, b - 5);
				painter.drawPoint(1, b - 4);
				painter.drawPoint(2, b - 3);
				painter.drawPoint(3, b - 2);
				painter.drawPoint(4, b - 2);
			}
		
			// Draw edge of bottom-right corner inside the area removed by the mask.
			if(cornersFlag & BOT_RIGHT) {
				painter.setPen(c2);
				painter.drawPoint(r - 2, b - 5);
				painter.drawPoint(r - 2, b - 4);
				painter.drawPoint(r - 3, b - 3);
				painter.drawPoint(r - 4, b - 2);
				painter.drawPoint(r - 5, b - 2);
			}
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
	if( item >= 0 && click_in_progress && buttonToWindowOperation( mousebutton ) == ClientGroupDragOp)
        {
		click_in_progress = false;
		drag_in_progress = true;
		QDrag *drag = new QDrag( widget() );
		QMimeData *group_data = new QMimeData();
		group_data->setData( clientGroupItemDragMimeType(), QString().setNum( itemId( item )).toAscii() );
		drag->setMimeData( group_data );
		
		sourceTab = item;

		// Create draggable tab pixmap
 		QList< ClientGroupItem > tabList = clientGroupItems();
// 		const int tabCount = tabList.count();
 		QRect frame( QPoint( 0, 0 ), widget()->frameGeometry().size() );
// 		QRect titlebar( frame.topLeft(), QSize( frame.width(),
// 		    layoutMetric( LM_TitleEdgeTop ) + layoutMetric( LM_TitleHeight ) +
// 		    layoutMetric( LM_TitleEdgeBottom ) - 1 // Titlebar and main frame overlap by 1px
// 		    ));
// 		QRect geom = titleRect().adjusted( -1, -layoutMetric( LM_TitleEdgeTop ), 1, 0 );
// 		geom.setWidth( geom.width() / tabCount + 1 ); // Split titlebar evenly
// 		geom.translate( geom.width() * item - item, 0 );
// 		QPixmap pix( geom.size() );
// 		QPainter painter( &pix );
// 		paintTab( painter, QRect( QPoint( 0, 0 ), geom.size() ), tabList[item],
// 		    isActive() && visibleClientGroupItem() == item );
// 		drag->setPixmap( pix );
		// If the cursor is on top of the pixmap then it makes the movement jerky on some systems
		//drag->setHotSpot( QPoint( c.x() - geom.x(), c.y() - geom.y() ));
// 		drag->setHotSpot( QPoint( c.x() - geom.x(), -1 ));

		drag->exec( Qt::MoveAction );
		drag_in_progress = false;
		if( drag->target() == 0 && tabList.count() > 1 )
		{ // Remove window from group and move to where the cursor is located
			QPoint pos = QCursor::pos();
			frame.moveTo( pos.x() - c.x(), pos.y() - c.y() );
			removeFromClientGroup( sourceTab, frame );
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
	if( e->source() != 0 && e->mimeData()->hasFormat( clientGroupItemDragMimeType() ) )
	{
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
	int tabClick = itemClicked( point );
	if( tabClick >= 0 )
	{
		const QMimeData *group_data = e->mimeData();
		if( group_data->hasFormat( clientGroupItemDragMimeType() ) )
		{
			if( widget() == e->source() )
			{
				int from = sourceTab;
				int item = itemClicked( point, false );
				if (from < item)
					item++;
				if (item == clientGroupItems().count())
					item = -1;
				moveItemInClientGroup( from, item);
			}
			else
			{
				long source = QString( group_data->data( clientGroupItemDragMimeType() ) ).toLong();
				moveItemToClientGroup( source, itemClicked( point, true ));
			}
			widget()->update();
			return true;
		}
	}
	return false; 
}

bool CrystalClient::dragMoveEvent( QDragMoveEvent* e)
{
	if( !e->mimeData()->hasFormat( clientGroupItemDragMimeType() ) ) return false;
	if (!drag_in_progress) return false;
	
	QPoint point = widget()->mapToParent( e->pos() );

	if (e->source() == widget()) { /* same window, rearrange tab */
		if (clientGroupItems().count() <= 1)
			targetTab = -1;
		else 
			targetTab = itemClicked( point, false );
		
		if (sourceTab != targetTab && targetTab >= 0 && sourceTab >= 0)
		{
			int item = targetTab;
			if (sourceTab < item)
				item++;
			if (item == clientGroupItems().count())
				item = -1;
			moveItemInClientGroup( sourceTab, item);
			sourceTab = targetTab;

		}
	} else {
		sourceTab = -1;
		targetTab = itemClicked( point, true );
		if (targetTab == -1)
			targetTab = clientGroupItems().count();
		widget()->update();
	}

	
	return true;
}

bool CrystalClient::dragLeaveEvent( QDragLeaveEvent* )
{
	drag_in_progress = false;
	widget()->update();
	return false;
}

int CrystalClient::itemClicked( const QPoint &point, bool between )
{
	QRect frame = widget()->frameGeometry();
	QRect r = titlebar_->geometry();
	r.setTop(0);
	QList< ClientGroupItem > list = clientGroupItems();
	int tabs = list.count();
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
