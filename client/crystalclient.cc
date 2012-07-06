#include <kconfig.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kdebug.h>

#include <qbitmap.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qtooltip.h>
#include <qapplication.h>
#include <qimage.h>
#include <qpopupmenu.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <qworkspace.h>

#include "crystalclient.h"
#include "crystalbutton.h"
#include "imageholder.h"

// Button themes
#include "tiles.h"
#include "aqua.h"
#include "knifty.h"



CrystalFactory* factory=NULL;

bool CrystalFactory::initialized_              = false;
Qt::AlignmentFlags CrystalFactory::titlealign_ = Qt::AlignHCenter;



extern "C" KDecorationFactory* create_factory()
{
    return new CrystalFactory();
}


//////////////////////////////////////////////////////////////////////////////
// CrystalFactory Class                                                     //
//////////////////////////////////////////////////////////////////////////////

CrystalFactory::CrystalFactory()
{
	for (int i=0;i<ButtonImageCount;i++)
		buttonImages[i]=NULL;

    readConfig();
    initialized_ = true;
	::factory=this;

    image_holder=new QImageHolder();
	CreateButtonImages();
}

CrystalFactory::~CrystalFactory() 
{ 
	initialized_ = false; 
	::factory=NULL;
	if (image_holder)delete image_holder;
	for (int i=0;i<ButtonImageCount;i++)
	{
		if (buttonImages[i])delete buttonImages[i];
		buttonImages[i]=NULL;
	}
}

KDecoration* CrystalFactory::createDecoration(KDecorationBridge* b)
{
    return new CrystalClient(b,factory );
}

//////////////////////////////////////////////////////////////////////////////
// reset()
// -------
// Reset the handler. Returns true if decorations need to be remade, false if
// only a repaint is necessary

bool CrystalFactory::reset(unsigned long changed)
{
    // read in the configuration
    initialized_ = false;
//    bool confchange = 
	readConfig();

    initialized_ = true;

    image_holder->repaint(true);
	CreateButtonImages();
	
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// readConfig()
// ------------
// Read in the configuration file

bool CrystalFactory::readConfig()
{
    // create a config object
    KConfig config("kwincrystalrc");
    config.setGroup("General");

    QString value = config.readEntry("TitleAlignment", "AlignHCenter");
    if (value == "AlignLeft") titlealign_ = Qt::AlignLeft;
    else if (value == "AlignHCenter") titlealign_ = Qt::AlignHCenter;
    else if (value == "AlignRight") titlealign_ = Qt::AlignRight;

    textshadow=(bool)config.readBoolEntry("TextShadow",true);
    trackdesktop=(bool)config.readBoolEntry("TrackDesktop",true);
    
    active.mode=config.readNumEntry("ActiveMode",0);
    inactive.mode=config.readNumEntry("InactiveMode",0);
    active.amount=(double)config.readNumEntry("ActiveShade",50)/100.0;
    inactive.amount=(double)config.readNumEntry("InactiveShade",50)/100.0;
    active.frame=(bool)config.readBoolEntry("ActiveFrame",true);
    inactive.frame=(bool)config.readBoolEntry("InactiveFrame",true);
	buttonColor=QColor(160,160,160);
    active.frameColor=config.readColorEntry("FrameColor1",&buttonColor);
	buttonColor=QColor(128,128,128);
    inactive.frameColor=config.readColorEntry("FrameColor2",&buttonColor);
    
    borderwidth=config.readNumEntry("Borderwidth",4);
    titlesize=config.readNumEntry("Titlebarheight",20);
 
	buttonColor=QColor(255,255,255);
    buttonColor=config.readColorEntry("ButtonColor",&buttonColor);
    roundCorners=config.readNumEntry("RoundCorners",TOP_LEFT & TOP_RIGHT);

	hovereffect=config.readBoolEntry("HoverEffect",false);
	tintButtons=config.readBoolEntry("TintButtons",buttonColor!=QColor(255,255,255));
	repaintMode=config.readNumEntry("RepaintMode",2);
	repaintTime=config.readNumEntry("RepaintTime",200);
	buttontheme=config.readNumEntry("ButtonTheme",0);
       
    return true;
}

void CrystalFactory::CreateButtonImages()
{
	for (int i=0;i<ButtonImageCount;i++)
	{
		if (buttonImages[i])buttonImages[i]->reset(); else
		buttonImages[i]=new ButtonImage;
	}
	
	switch(buttontheme)
	{
	default:	// whee, seems to work. ;)
	case 0:	// Crystal default
		buttonImages[ButtonImageHelp]->SetNormal(crystal_help_data,tintButtons);
		buttonImages[ButtonImageMax]->SetNormal(crystal_max_data,tintButtons);
		buttonImages[ButtonImageRestore]->SetNormal(crystal_restore_data,tintButtons);
		buttonImages[ButtonImageMin]->SetNormal(crystal_min_data,tintButtons);
		buttonImages[ButtonImageClose]->SetNormal(crystal_close_data,tintButtons);
		buttonImages[ButtonImageSticky]->SetNormal(crystal_sticky_data,tintButtons);
		buttonImages[ButtonImageUnSticky]->SetNormal(crystal_un_sticky_data,tintButtons);
		buttonImages[ButtonImageShade]->SetNormal(crystal_shade_data,tintButtons);
	
		buttonImages[ButtonImageAbove]->SetNormal(crystal_above_data,tintButtons);
		buttonImages[ButtonImageUnAbove]->SetNormal(crystal_unabove_data,tintButtons);
		buttonImages[ButtonImageBelow]->SetNormal(crystal_below_data,tintButtons);
		buttonImages[ButtonImageUnBelow]->SetNormal(crystal_unbelow_data,tintButtons);
		break;
	case 1: // Aqua buttons
		buttonImages[ButtonImageHelp]->SetNormal(aqua_default_data,tintButtons);
		buttonImages[ButtonImageMax]->SetNormal(aqua_default_data,tintButtons);
		buttonImages[ButtonImageRestore]->SetNormal(aqua_default_data,tintButtons);
		buttonImages[ButtonImageMin]->SetNormal(aqua_default_data,tintButtons);
		buttonImages[ButtonImageClose]->SetNormal(aqua_default_data,tintButtons);
		buttonImages[ButtonImageSticky]->SetNormal(aqua_sticky_data,tintButtons);
		buttonImages[ButtonImageUnSticky]->SetNormal(aqua_default_data,tintButtons);
		buttonImages[ButtonImageShade]->SetNormal(aqua_default_data,tintButtons);
	
		buttonImages[ButtonImageAbove]->SetNormal(aqua_default_data,tintButtons);
		buttonImages[ButtonImageUnAbove]->SetNormal(aqua_above_data,tintButtons);
		buttonImages[ButtonImageBelow]->SetNormal(aqua_default_data,tintButtons);
		buttonImages[ButtonImageUnBelow]->SetNormal(aqua_below_data,tintButtons);
		
		
		buttonImages[ButtonImageClose]->SetHovered(aqua_close_data,tintButtons);
		buttonImages[ButtonImageMax]->SetHovered(aqua_maximize_data,tintButtons);
		buttonImages[ButtonImageMin]->SetHovered(aqua_minimize_data,tintButtons);
		buttonImages[ButtonImageRestore]->SetHovered(aqua_maximize_data,tintButtons);
		buttonImages[ButtonImageUnSticky]->SetHovered(aqua_un_sticky_data,tintButtons);
		buttonImages[ButtonImageHelp]->SetHovered(aqua_help_data,tintButtons);
		buttonImages[ButtonImageAbove]->SetHovered(aqua_above_data,tintButtons);
		buttonImages[ButtonImageBelow]->SetHovered(aqua_below_data,tintButtons);
		buttonImages[ButtonImageShade]->SetHovered(aqua_shade_data,tintButtons);
		break;
	case 2: // Knifty buttons
		buttonImages[ButtonImageHelp]->SetNormal(knifty_help_data,tintButtons);
		buttonImages[ButtonImageMax]->SetNormal(knifty_max_data,tintButtons);
		buttonImages[ButtonImageRestore]->SetNormal(knifty_restore_data,tintButtons);
		buttonImages[ButtonImageMin]->SetNormal(knifty_min_data,tintButtons);
		buttonImages[ButtonImageClose]->SetNormal(knifty_close_data,tintButtons);
		buttonImages[ButtonImageSticky]->SetNormal(knifty_sticky_data,tintButtons);
		buttonImages[ButtonImageUnSticky]->SetNormal(knifty_un_sticky_data,tintButtons);
		buttonImages[ButtonImageShade]->SetNormal(knifty_shade_data,tintButtons);
        
		buttonImages[ButtonImageAbove]->SetNormal(knifty_above_data,tintButtons);
		buttonImages[ButtonImageUnAbove]->SetNormal(knifty_unabove_data,tintButtons);
		buttonImages[ButtonImageBelow]->SetNormal(knifty_below_data,tintButtons);
		buttonImages[ButtonImageUnBelow]->SetNormal(knifty_unbelow_data,tintButtons);
		break;
	}	

}



//////////////////////////////////////////////////////////////////////////////
// CrystalClient Class                                                      //
//////////////////////////////////////////////////////////////////////////////

CrystalClient::CrystalClient(KDecorationBridge *b,CrystalFactory *f)
    : KDecoration(b,f)
{
	::factory->clients.append(this);
}

CrystalClient::~CrystalClient()
{
	::factory->clients.remove(this);
    for (int n=0; n<ButtonTypeCount; n++) {
        if (button[n]) delete button[n];
    }
}

//////////////////////////////////////////////////////////////////////////////
// init()
// ------
// Actual initializer for class

void CrystalClient::init()
{
    createMainWidget(WResizeNoErase | WRepaintNoErase);
    widget()->installEventFilter(this);
	FullMax=false;
	
    // for flicker-free redraws
    widget()->setBackgroundMode(NoBackground);

    // setup layout
    mainlayout = new QGridLayout(widget(), 4, 3); // 4x3 grid
    titlelayout = new QHBoxLayout();
    titlebar_ = new QSpacerItem(1, ::factory->titlesize, QSizePolicy::Expanding,
                                QSizePolicy::Fixed);

    mainlayout->setResizeMode(QLayout::FreeResize);
    mainlayout->setRowSpacing(0, 1);
	mainlayout->setRowSpacing(3, ::factory->borderwidth*2);

	mainlayout->setColSpacing(2,borderSpacing());
	mainlayout->setColSpacing(0,borderSpacing());
    mainlayout->addLayout(titlelayout, 1, 1);

	
    if (isPreview()) {
        mainlayout->addItem(new QSpacerItem(1, 1,QSizePolicy::Expanding,QSizePolicy::Fixed), 0, 1);
        mainlayout->addItem(new QSpacerItem(1, ::factory->borderwidth,QSizePolicy::Expanding,QSizePolicy::Expanding), 3, 1);
		mainlayout->addWidget(
        new QLabel(i18n("<b><center>Crystal Preview</center></b>"),widget()), 2, 1);
    } else {
        mainlayout->addItem(new QSpacerItem(0, 0), 2, 1);
    }
	
    // the window should stretch
    mainlayout->setRowStretch(2, 10);
    mainlayout->setColStretch(1, 10);
    
    updateMask();
    
    // setup titlebar buttons
//    titlelayout->addItem(new QSpacerItem(::factory->borderwidth/2,0));
    
    for (int n=0; n<ButtonTypeCount; n++) button[n] = 0;
//	titlelayout->insertSpacing(0,borderSpacing());
    addButtons(titlelayout, options()->titleButtonsLeft());
    titlelayout->addItem(titlebar_);
    CrystalButton* lastbutton=addButtons(titlelayout, options()->titleButtonsRight());
	if (lastbutton)lastbutton->setFirstLast(false,true);
//	titlelayout->insertSpacing(-1,borderSpacing());
	
    connect( this, SIGNAL( keepAboveChanged( bool )), SLOT( keepAboveChange( bool )));
    connect( this, SIGNAL( keepBelowChanged( bool )), SLOT( keepBelowChange( bool )));

	
    connect ( ::factory->image_holder,SIGNAL(repaintNeeded()),this,SLOT(Repaint()));
    connect ( &timer,SIGNAL(timeout()),this,SLOT(Repaint()));
}

void CrystalClient::updateMask()
{
	if ((::factory->roundCorners==0)|| (!options()->moveResizeMaximizedWindows() && maximizeMode() & MaximizeFull ) ) 
	{
		setMask(QRegion(widget()->rect()));
		return;
	}
	
    int cornersFlag = ::factory->roundCorners;
	int r(width());
	int b(height());
	QRegion mask;

	mask=QRegion(widget()->rect());
	
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

//////////////////////////////////////////////////////////////////////////////
// addButtons()
// ------------
// Add buttons to title layout

CrystalButton* CrystalClient::addButtons(QBoxLayout *layout, const QString& s)
{
    ButtonImage *bitmap;
    QString tip;
	CrystalButton *lastone=NULL;

    if (s.length() > 0) {
        for (unsigned n=0; n < s.length(); n++) {
			CrystalButton *current=NULL;
            switch (s[n]) {
              case 'M': // Menu button
                  if (!button[ButtonMenu]) 
				  {
                      button[ButtonMenu] = current = new CrystalButton(this, "menu", i18n("Menu"), ButtonMenu, 0);
                      connect(button[ButtonMenu], SIGNAL(pressed()), this, SLOT(menuButtonPressed()));
                  }
                  break;

              case 'S': // Sticky button
                  if (!button[ButtonSticky]) 
				  {
              			if (isOnAllDesktops()) 
						{
              				bitmap = ::factory->buttonImages[ButtonImageSticky];
              				tip = i18n("Not on all desktops");
              			} else {
              				bitmap = ::factory->buttonImages[ButtonImageUnSticky];
							tip = i18n("On All Desktops");
						}
                      	button[ButtonSticky] =current=new CrystalButton(this, "sticky", tip,ButtonSticky, bitmap);
                      	connect(button[ButtonSticky], SIGNAL(clicked()),this, SLOT(toggleOnAllDesktops()));
                  }
                  break;

              case 'H': // Help button
                  if (providesContextHelp()) {
                      button[ButtonHelp] =current=
                          new CrystalButton(this, "help", i18n("Help"),
                                            ButtonHelp, ::factory->buttonImages[ButtonImageHelp]);
                      connect(button[ButtonHelp], SIGNAL(clicked()),
                              this, SLOT(showContextHelp()));
                  }
                  break;

              case 'I': // Minimize button
                  if ((!button[ButtonMin]) && isMinimizable())  {
                      button[ButtonMin] =current=
                          new CrystalButton(this, "iconify", i18n("Minimize"),
                                            ButtonMin, ::factory->buttonImages[ButtonImageMin]);
                      connect(button[ButtonMin], SIGNAL(clicked()),
                              this, SLOT(minButtonPressed()));
                  }
                  break;


              case 'F': // Above button
                  if ((!button[ButtonAbove]))  {
                      button[ButtonAbove] =current=
                          new CrystalButton(this, "above", i18n("Keep Above Others"),
                                            ButtonAbove, ::factory->buttonImages[keepAbove()?ButtonImageUnAbove:ButtonImageAbove]);
                      connect(button[ButtonAbove], SIGNAL(clicked()),
                              this, SLOT(aboveButtonPressed()));
                  }
                  break;

              case 'B': // Below button
                  if ((!button[ButtonBelow]))  {
                      button[ButtonBelow] =current=
                          new CrystalButton(this, "below", i18n("Keep Below Others"),
                                            ButtonBelow, ::factory->buttonImages[keepBelow()?ButtonImageUnBelow:ButtonImageBelow]);
                      connect(button[ButtonBelow], SIGNAL(clicked()),
                              this, SLOT(belowButtonPressed()));
                  }
                  break;

				  
              case 'L': // Shade button
                  if ((!button[ButtonShade]) && isShadeable())  {
                      button[ButtonShade] =current=
                          new CrystalButton(this, "shade", i18n("Shade"),
                                            ButtonShade, ::factory->buttonImages[ButtonImageShade]);
                      connect(button[ButtonShade], SIGNAL(clicked()),
                              this, SLOT(shadeButtonPressed()));
                  }
                  break;
		  
              case 'A': // Maximize button
                  if ((!button[ButtonMax]) && isMaximizable()) {
              if (maximizeMode() == MaximizeFull) {
              bitmap = ::factory->buttonImages[ButtonImageRestore];
              tip = i18n("Restore");
              } else {
              bitmap = ::factory->buttonImages[ButtonImageMax];
              tip = i18n("Maximize");
              }
                      button[ButtonMax]  =current=
                          new CrystalButton(this, "maximize", tip,
                                            ButtonMax, bitmap);
                      connect(button[ButtonMax], SIGNAL(clicked()),
                              this, SLOT(maxButtonPressed()));
                  }
                  break;

              case 'X': // Close button
                  if (isCloseable()) {
                      button[ButtonClose] =current=
                          new CrystalButton(this, "close", i18n("Close"),
                                            ButtonClose, ::factory->buttonImages[ButtonImageClose]);
                      connect(button[ButtonClose], SIGNAL(clicked()),
                              this, SLOT(closeWindow()));
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
				if (layout->findWidget(current)==0)current->setFirstLast(true,false);
			}
			lastone=current;
		}
    }
	return lastone;
}

//////////////////////////////////////////////////////////////////////////////
// activeChange()
// --------------
// window active state has changed

void CrystalClient::activeChange()
{
	Repaint();
}

//////////////////////////////////////////////////////////////////////////////
// captionChange()
// ---------------
// The title has changed

void CrystalClient::captionChange()
{
    widget()->repaint(titlebar_->geometry(), false);
}

//////////////////////////////////////////////////////////////////////////////
// desktopChange()
// ---------------
// Called when desktop/sticky changes

void CrystalClient::desktopChange()
{
    bool d = isOnAllDesktops();
    if (button[ButtonSticky]) {
        button[ButtonSticky]->setBitmap(::factory->buttonImages[d ? ButtonImageSticky : ButtonImageUnSticky ]);
    	QToolTip::remove(button[ButtonSticky]);
    	QToolTip::add(button[ButtonSticky], d ? i18n("Not on all desktops") : i18n("On All Desktops"));
    }
    
//    wallpaper->repaint(true);
}

//////////////////////////////////////////////////////////////////////////////
// iconChange()
// ------------
// The title has changed

void CrystalClient::iconChange()
{
    if (button[ButtonMenu]) {
        button[ButtonMenu]->setBitmap(0);
    }
}

//////////////////////////////////////////////////////////////////////////////
// maximizeChange()
// ----------------
// Maximized state has changed

void CrystalClient::maximizeChange()
{
    bool m = (maximizeMode() == MaximizeFull);
    if (button[ButtonMax]) {
        button[ButtonMax]->setBitmap(::factory->buttonImages[ m ? ButtonImageRestore : ButtonImageMax ]);
    	QToolTip::remove(button[ButtonMax]);
    	QToolTip::add(button[ButtonMax], m ? i18n("Restore") : i18n("Maximize"));
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
		mainlayout->setColSpacing(0,0);
		mainlayout->setColSpacing(2,0);
	}else{
		mainlayout->setColSpacing(2,borderSpacing());
		mainlayout->setColSpacing(0,borderSpacing());
	}
	
	mainlayout->setRowSpacing(0, (FullMax)?0:1);
	for (int i=0;i<ButtonTypeCount;i++)if (button[i])
		button[i]->resetSize(FullMax);
	widget()->layout()->activate();
}

int CrystalClient::borderSpacing()
{
	if (::factory->roundCorners)
		return (::factory->borderwidth<=6)?5: ::factory->borderwidth-1;
	return (::factory->borderwidth<=2)?1: ::factory->borderwidth-1;
}

//////////////////////////////////////////////////////////////////////////////
// shadeChange()
// -------------
// Called when window shading changes

void CrystalClient::shadeChange()
{ 
	return;
	updateLayout(); 
	updateMask();
	Repaint();
}

//////////////////////////////////////////////////////////////////////////////
// borders()
// ----------
// Get the size of the borders

void CrystalClient::borders(int &l, int &r, int &t, int &b) const
{
    l = r = ::factory->borderwidth;
    t = ::factory->titlesize;
    if (!isShade())b = ::factory->borderwidth; else b=1;
	
	if (!options()->moveResizeMaximizedWindows() )
	{
		if ( maximizeMode() & MaximizeHorizontal )	l=r=1;
		if ( maximizeMode() & MaximizeVertical )
		{
			b=isShade()?1:1;
			t=::factory->titlesize;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// resize()
// --------
// Called to resize the window

void CrystalClient::resize(const QSize &size)
{
    widget()->resize(size);
}

//////////////////////////////////////////////////////////////////////////////
// minimumSize()
// -------------
// Return the minimum allowable size for this window

QSize CrystalClient::minimumSize() const
{
    return widget()->minimumSize();
}

//////////////////////////////////////////////////////////////////////////////
// mousePosition()
// ---------------
// Return logical mouse position

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

//////////////////////////////////////////////////////////////////////////////
// eventFilter()
// -------------
// Event filter

bool CrystalClient::eventFilter(QObject *obj, QEvent *e)
{
    if (obj != widget()) return false;

    switch (e->type()) {

	  case QEvent::MouseButtonDblClick: {
          mouseDoubleClickEvent(static_cast<QMouseEvent *>(e));
          return true;
      }
      case QEvent::MouseButtonPress: {
	  	  QMouseEvent *me=static_cast<QMouseEvent *>(e);
		  
		  if (me->state() & ControlButton)
		  {
		  	ShowTabMenu(me);
		  }else processMousePressEvent(me);
          return true;
      }
      case QEvent::Paint: {
          paintEvent(static_cast<QPaintEvent *>(e));
          return true;
      }
	  case QEvent::Wheel: {
	  	  mouseWheelEvent(static_cast<QWheelEvent *>(e));
	  	  return true;
	  }
      case QEvent::Resize: {
          resizeEvent(static_cast<QResizeEvent *>(e));
          return true;
      }
      case QEvent::Show: {
          showEvent(static_cast<QShowEvent *>(e));
          return true;
	  }
      case QEvent::Move: {
          moveEvent(static_cast<QMoveEvent *>(e));	  
	      return true;
	  }
      default: {
          return false;
      }
    }

    return false;
}

void CrystalClient::ClientWindows(Window* frame,Window* wrapper,Window *client)
{
	Window root=0,parent=0,*children=NULL;
	uint numc;
	// Our Deco is the child of a frame, get our parent
	XQueryTree(qt_xdisplay(),widget()->winId(),&root,frame,&children,&numc);
	if (children!=NULL)XFree(children);
	
	// frame has two children, us and a wrapper, get the wrapper
	XQueryTree(qt_xdisplay(),*frame,&root,&parent,&children,&numc);	
	for (uint i=0;i<numc;i++)
	{
//		printf("Child of frame[%d]=%d\n",i,children[i]);
		if (children[i]!=widget()->winId())*wrapper=children[i];
	}
	XFree(children);
	
	// wrapper has only one child, which is the client. We want this!!
	XQueryTree(qt_xdisplay(),*wrapper,&root,&parent,&children,&numc);
	if (numc==1)*client=children[0];	
	if (children!=NULL)XFree(children);
}

//////////////////////////////////////////////////////////////////////////////
// mouseDoubleClickEvent()
// -----------------------
// Doubleclick on title

void CrystalClient::mouseDoubleClickEvent(QMouseEvent *e)
{
    if ((titlebar_->geometry().contains(e->pos()))&&(e->button()==LeftButton)) titlebarDblClickOperation();
		else 
	{
		QMouseEvent me(QEvent::MouseButtonPress,e->pos(),e->button(),e->state());
		processMousePressEvent(&me);
	}
}

void CrystalClient::mouseWheelEvent(QWheelEvent *e)
{
	// FIXME: Make it good!
	if (titlebar_->geometry().contains(e->pos()))
	{
		QPtrList <CrystalClient> *l=&(::factory->clients);
		
		if (l->current()==NULL) for (unsigned int i=0;i<l->count();i++) if ((l->at(i))->isActive()) break;
		
		CrystalClient *n=this;
		
		do
		{
			if(e->delta()>0)
			{
				n=l->next();
				if (n==NULL)n=l->first();
			}else{
				n=l->prev();
				if (n==NULL)n=l->last();
			}
			if (n->desktop()==desktop())break;
		}while(n!=this);
			
		Window client,frame,wrapper;
		n->ClientWindows(&frame,&wrapper,&client);
//		int p=XRaiseWindow(qt_xdisplay(),frame);
//		printf("%d\n",p);

// 		XSetInputFocus(qt_xdisplay(),client,RevertToParent,CurrentTime);
// 		XSetInputFocus(qt_xdisplay(),wrapper,RevertToParent,CurrentTime);
// 		XSetInputFocus(qt_xdisplay(),frame,RevertToParent,CurrentTime);
	}
}

//////////////////////////////////////////////////////////////////////////////
// paintEvent()
// ------------
// Repaint the window

void CrystalClient::paintEvent(QPaintEvent*)
{
	if (!CrystalFactory::initialized()) return;

	QColorGroup group;
	QPainter painter(widget());

	// draw the titlebar
	group = options()->colorGroup(KDecoration::ColorTitleBar, isActive());
   
	if (::factory->trackdesktop)
		::factory->image_holder->repaint(false); // If other desktop than the last, regrab the root image
	QPixmap *background=::factory->image_holder->image(isActive());
    
	{
		QRect r;
		QPoint p=widget()->mapToGlobal(QPoint(0,0));
		int bl,br,bt,bb;
		borders(bl,br,bt,bb);
	
		QPixmap pufferPixmap;
		pufferPixmap.resize(widget()->width(), bt);
		QPainter pufferPainter(&pufferPixmap);

		r=QRect(p.x(),p.y(),widget()->width(),bt);
		if (background && !background->isNull())pufferPainter.drawPixmap(QPoint(0,0),*background,r);
		else 
		{
			pufferPainter.fillRect(widget()->rect(),group.background());
			painter.fillRect(widget()->rect(), group.background());
		}
	
	// draw title text
		pufferPainter.setFont(options()->font(isActive(), false));
	
		QColor color=options()->color(KDecoration::ColorFont, isActive());
		r=titlebar_->geometry();
		r.moveBy(0,-1);
	
		if (::factory->textshadow)
		{
			pufferPainter.translate(1,1);
			pufferPainter.setPen(color.dark(200));
			pufferPainter.drawText(r,CrystalFactory::titleAlign() | AlignVCenter,caption());
			pufferPainter.translate(-1,-1);
		}
	
		pufferPainter.setPen(color);
		pufferPainter.drawText(r,
			CrystalFactory::titleAlign() | AlignVCenter,
			caption());

		if (::factory->borderwidth>0 && background && !background->isNull())
		{	// Draw the side and bottom of the window with transparency
			r=QRect(p.x(),p.y()+bt,bl,widget()->height()-bt);
			painter.drawPixmap(QPoint(0,bt),*background,r);
	
			r=QRect(widget()->width()-br+p.x(),p.y()+bt,widget()->width(),widget()->height()-bt);
			painter.drawPixmap(QPoint(widget()->width()-br,bt),*background,r);

			r=QRect(p.x()+bl,p.y()+widget()->height()-bb,widget()->width()-bl-br,bb);
			painter.drawPixmap(QPoint(bl,widget()->height()-bb),*background,r);
		}
	
		pufferPainter.end();
	
	
		painter.drawPixmap(0,0,pufferPixmap);
	}
	if (background==NULL)
	{	// We don't have a background image, draw a solid rectangle
		// And notify image_holder that we need an update asap
		if (::factory)if (::factory->image_holder)
		// UnInit image_holder, on next Repaint it will be Init'ed again.
		QTimer::singleShot(500,::factory->image_holder,SLOT(CheckSanity()));
	}

	WND_CONFIG* wndcfg=(isActive()?&::factory->active:&::factory->inactive);
	// draw frame
	if (wndcfg->frame)
	{
		group = options()->colorGroup(KDecoration::ColorFrame, isActive());

    	// outline the frame
		painter.setPen(wndcfg->frameColor);
		painter.drawRect(widget()->rect());
		if ((::factory->roundCorners) && !(!options()->moveResizeMaximizedWindows() && maximizeMode() & MaximizeFull))
		{
			int cornersFlag = ::factory->roundCorners;
			int r(width());
			int b(height());
  
            // Draw edge of top-left corner inside the area removed by the mask.
            if(cornersFlag & TOP_LEFT) {
                painter.drawPoint(3, 1);
                painter.drawPoint(4, 1);
                painter.drawPoint(2, 2);
                painter.drawPoint(1, 3);
                painter.drawPoint(1, 4);
            }

            // Draw edge of top-right corner inside the area removed by the mask.
            if(cornersFlag & TOP_RIGHT) {
                painter.drawPoint(r - 5, 1);
                painter.drawPoint(r - 4, 1);
                painter.drawPoint(r - 3, 2);
                painter.drawPoint(r - 2, 3);
                painter.drawPoint(r - 2, 4);
            }

            // Draw edge of bottom-left corner inside the area removed by the mask.
            if(cornersFlag & BOT_LEFT) {
                painter.drawPoint(1, b - 5);
                painter.drawPoint(1, b - 4);
                painter.drawPoint(2, b - 3);
                painter.drawPoint(3, b - 2);
                painter.drawPoint(4, b - 2);
            }

            // Draw edge of bottom-right corner inside the area removed by the mask.
            if(cornersFlag & BOT_RIGHT) {
                painter.drawPoint(r - 2, b - 5);
                painter.drawPoint(r - 2, b - 4);
                painter.drawPoint(r - 3, b - 3);
                painter.drawPoint(r - 4, b - 2);
                painter.drawPoint(r - 5, b - 2);
            }
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
// resizeEvent()
// -------------
// Window is being resized

void CrystalClient::resizeEvent(QResizeEvent *)
{
	if (widget()->isShown()) 
	{
		if (::factory->repaintMode==1)Repaint();
		// repaint only every xxx ms
		else if (::factory->repaintMode==3 || !timer.isActive())
		{
			// Repaint only, when mode!=fade || amount<100
			WND_CONFIG* wnd=isActive()?&::factory->active:&::factory->inactive;
			if (wnd->mode!=0 || wnd->amount<100)
				timer.start(::factory->repaintTime,true);	
		}
		updateMask();
	}
}

void CrystalClient::moveEvent(QMoveEvent *)
{
	if (widget()->isShown()) 
	{
		if (::factory->repaintMode==1)Repaint();
		// repaint every xxx ms, so constant moving does not take too much CPU
		else if (::factory->repaintMode==3 || !timer.isActive())
		{
			// Repaint only, when mode!=fade || value<100, because otherwise it is a plain color
			WND_CONFIG* wnd=isActive()?&::factory->active:&::factory->inactive;
			if (wnd->mode!=0 || wnd->amount<100)
				timer.start(::factory->repaintTime,true);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// showEvent()
// -----------
// Window is being shown

void CrystalClient::showEvent(QShowEvent *)
{
	if (widget()->isShown()) 
		Repaint();
}

void CrystalClient::Repaint()
{
	widget()->repaint(false);
	for (int n=0; n<ButtonTypeCount; n++)
		if (button[n]) button[n]->reset();
}

//////////////////////////////////////////////////////////////////////////////
// maxButtonPressed()
// -----------------
// Max button was pressed

void CrystalClient::maxButtonPressed()
{
    if (button[ButtonMax]) {
        switch (button[ButtonMax]->lastMousePress()) {
          case MidButton:
              maximize(maximizeMode() ^ MaximizeVertical);
              break;
          case RightButton:
              maximize(maximizeMode() ^ MaximizeHorizontal);
              break;
          default:
              (maximizeMode() == MaximizeFull) ? maximize(MaximizeRestore)
                  : maximize(MaximizeFull);
        }
    }
}

void CrystalClient::minButtonPressed()
{
    if (button[ButtonMin]) {
        switch (button[ButtonMin]->lastMousePress()) {
          case MidButton:
	  	performWindowOperation(LowerOp);
	  	break;
	  case RightButton:
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

void CrystalClient::keepAboveChange(bool set)
{
	if (button[ButtonAbove])
	{
		button[ButtonAbove]->setBitmap(::factory->buttonImages[keepAbove()?ButtonImageUnAbove:ButtonImageAbove]);
	}
}

void CrystalClient::keepBelowChange(bool set)
{
	if (button[ButtonBelow])
	{
		button[ButtonBelow]->setBitmap(::factory->buttonImages[keepBelow()?ButtonImageUnBelow:ButtonImageBelow]);
	}
}

void CrystalClient::shadeButtonPressed()
{
    if (button[ButtonShade]) {
        switch (button[ButtonShade]->lastMousePress()) {
          case MidButton:
	  case RightButton:
//              if (isShadeable()) setShade(!isShade());
              break;
          default:
              if (isShadeable()) setShade(!isShade());
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// menuButtonPressed()
// -------------------
// Menu button was pressed (popup the menu)

void CrystalClient::menuButtonPressed()
{
	if (!button[ButtonMenu])return;
	
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

	// Do not show menu immediately, so a double click to close the window does not cause flicker
	QTimer::singleShot(150,this,SLOT(menuPopUp()));
}

void CrystalClient::menuPopUp()
{
	QPoint p(button[ButtonMenu]->rect().bottomLeft().x(),
                 button[ButtonMenu]->rect().bottomLeft().y());
	KDecorationFactory* f = factory();
	showWindowMenu(button[ButtonMenu]->mapToGlobal(p));
	if (!f->exists(this)) return; // decoration was destroyed
	button[ButtonMenu]->setDown(false);
}

void CrystalClient::ShowTabMenu(QMouseEvent *me)
{	
	// FIXME This stuff does not work at all!
	return;
	QPopupMenu p(widget());
	for (uint i=0;i<(::factory->clients.count());i++)
	{
		CrystalClient* c=::factory->clients.at(i);
		if (c!=this)
			p.insertItem(c->caption(),(int)c);
	}
	CrystalClient* client=(CrystalClient*)p.exec(widget()->mapToGlobal(me->pos()));
	if ((int)client==-1) return;
	if (!::factory->exists(client)) return;
	
	XWindowAttributes attr;
	Window w_client2,w_frame2,w_wrapper2,w_client1,w_frame1,w_wrapper1;
	
	client->ClientWindows(&w_frame1,&w_wrapper1,&w_client1);
	XGetWindowAttributes(qt_xdisplay(),w_frame1,&attr);
	
	
	ClientWindows(&w_frame2,&w_wrapper2,&w_client2);
	
	
	XWindowChanges c;
	c.x=attr.x;
	c.y=attr.y;
	c.width=attr.width;
	c.height=attr.height;
	
//	XReconfigureWMWindow(qt_xdisplay(),w_frame,0,CWX|CWY|CWWidth|CWHeight,&c);
	
//	XUnmapWindow(qt_xdisplay(),w_frame2);
	grabXServer();
	static XSizeHints size;
	size.flags=PSize;
	size.width=attr.width;
	size.height=attr.height;
	XSetWMNormalHints(qt_xdisplay(),w_client2,&size);
	XResizeWindow(qt_xdisplay(),w_client2,attr.width,attr.height);
	ungrabXServer();
	
//	XMapWindow(qt_xdisplay(),w_frame2);

//	XMoveResizeWindow( qt_xdisplay(), w_frame, attr.x, attr.y, attr.width, attr.height );
//	resize(QSize(attr.width,attr.height));

//	XMoveResizeWindow( qt_xdisplay(), w_wrapper, 0, 0, attr.width, attr.height);
//	XMoveResizeWindow( qt_xdisplay(), w_client, 0, 0, attr.width, attr.height);


}


#include "crystalclient.moc"
