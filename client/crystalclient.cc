// #define QT_CLEAN_NAMESPACE
#include <kconfig.h>
//#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kdebug.h>

#include <qlabel.h>
#include <qpainter.h>
#include <qtooltip.h>
#include <qapplication.h>
#include <qimage.h>

#include <math.h>

#include "crystalclient.h"
#include "crystalbutton.h"
#include "imageholder.h"
#include "glfont.h"

// Our button images
#include "tiles.h"



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
	// TODO: Move this stuff, where it is save and no longer locks the X-Server on certain machines!
	glInitialized=false;
	for (int i=0;i<ButtonImageCount;i++)
		buttonImages[i]=NULL;

    readConfig();
    initialized_ = true;
	::factory=this;

    image_holder=new QImageHolder();
	gl_font=NULL;
	
    glxcontext=NULL;

	CreateButtonImages();
}

CrystalFactory::~CrystalFactory() 
{ 
	initialized_ = false; 
	if (image_holder)delete image_holder;
	if (gl_font)delete gl_font;
	::factory=NULL;
	for (int i=0;i<ButtonImageCount;i++)
	{
		if (buttonImages[i])delete buttonImages[i];
		buttonImages[i]=NULL;
	}
	if (glxcontext)glXDestroyContext(qt_xdisplay(),glxcontext);
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

bool CrystalFactory::reset(unsigned long /*changed*/)
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
    config.setGroup("CrystalGL");

    QString value = config.readEntry("TitleAlignment", "AlignHCenter");
    if (value == "AlignLeft") titlealign_ = Qt::AlignLeft;
    else if (value == "AlignHCenter") titlealign_ = Qt::AlignHCenter;
    else if (value == "AlignRight") titlealign_ = Qt::AlignRight;

    textshadow=(bool)config.readBoolEntry("TextShadow",true);
    trackdesktop=(bool)config.readBoolEntry("TrackDesktop",true);
    
    
    borderwidth=config.readNumEntry("Borderwidth",4);
    titlesize=config.readNumEntry("Titlebarheight",20);
 
	buttonColor=QColor(255,255,255);
    buttonColor=config.readColorEntry("ButtonColor",&buttonColor);
    roundCorners=config.readNumEntry("RoundCorners",TOP_LEFT & TOP_RIGHT);

	hovereffect=config.readBoolEntry("HoverEffect",true);
	tintButtons=config.readBoolEntry("TintButtons",buttonColor!=QColor(255,255,255));
	repaintMode=config.readNumEntry("RepaintMode",1);
	repaintTime=config.readNumEntry("RepaintTime",200);
	
	fadeButtons=config.readBoolEntry("FadeButtons",true);
	textureSize=1<<(config.readNumEntry("TextureSize",2)+7);
	useRefraction=config.readBoolEntry("SimulateRefraction",true);
	useLighting=config.readBoolEntry("SimulateLighting",true);
	animateActivate=config.readBoolEntry("AnimateActivate",true);
    iorActive=(double)config.readDoubleNumEntry("IORActive",2.4);
    iorInactive=(double)config.readDoubleNumEntry("IORInactive",1.2);
	
	activeColor=QColor(150,160,255);
    activeColor=config.readColorEntry("ActiveColor",&activeColor);
	inactiveColor=QColor(160,160,160);
    inactiveColor=config.readColorEntry("InactiveColor",&inactiveColor);

       
    return true;
}

void CrystalFactory::CreateButtonImages()
{
	for (int i=0;i<ButtonImageCount;i++)
	{
		if (buttonImages[i])buttonImages[i]->reset(); else
		buttonImages[i]=new ButtonImage;
	}

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
}

bool CrystalFactory::initGL(Window winId)
{
	if (glInitialized)
	{
		if (!glXMakeCurrent(qt_xdisplay(),winId,glxcontext))return false;
		return true;
	}
	
    Display *dpy=qt_xdisplay();
    int attrib[] = { GLX_RGBA,
        GLX_RED_SIZE, 1,
        GLX_GREEN_SIZE, 1,
        GLX_BLUE_SIZE, 1,
        GLX_DOUBLEBUFFER,
        GLX_DEPTH_SIZE, 0,
        None };
   int scrnum;
   XVisualInfo *visinfo;

   scrnum = DefaultScreen( dpy );

   visinfo = glXChooseVisual( dpy, scrnum, attrib );
   if (!visinfo) {
      printf("Error: couldn't get an RGB, Double-buffered visual\n");
      return false;
   }

   glxcontext = glXCreateContext( dpy, visinfo, NULL, True );
   if (!glxcontext) {
      printf("Error: glXCreateContext failed\n");
	  XFree(visinfo);
	  return false;
      
   }
   XFree(visinfo);	
   
   
   
   
   // GL initialized fine, now setup rendering states
   glXMakeCurrent(qt_xdisplay(),winId,glxcontext);
	
    glDisable( GL_CULL_FACE );
    glDisable( GL_LIGHTING );
    glDisable( GL_LIGHT0 );
    glDisable(GL_AUTO_NORMAL);
    glDisable(GL_NORMALIZE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL); 
    glDisable(GL_ACCUM); 
	glDisable(GL_ALPHA_TEST);

    glShadeModel(GL_SMOOTH);


	glClearColor( 0.0, 0.0, 0.0, 1.0 );

	QFont font=options()->font(false, false);
	gl_font=new GLFont(font);
	
	gl_font->init();
	glInitialized=true;
	
	return true;
}

/* Borrowed from the Qt source code */
QImage CrystalFactory::convertToGLFormat( const QImage& img )
{
    QImage res = img.convertDepth( 32 );
    res = res.mirror();

    if ( QImage::systemByteOrder() == QImage::BigEndian ) {
	// Qt has ARGB; OpenGL wants RGBA
	for ( int i=0; i < res.height(); i++ ) {
	    uint *p = (uint*)res.scanLine( i );
	    uint *end = p + res.width();
	    while ( p < end ) {
		*p = (*p << 8) | ((*p >> 24) & 0xFF);
		p++;
	    }
	}
    }
    else {
	// Qt has ARGB; OpenGL wants ABGR (i.e. RGBA backwards)
	res = res.swapRGB();
    }
    return res;
}







//////////////////////////////////////////////////////////////////////////////
// CrystalClient Class                                                      //
//////////////////////////////////////////////////////////////////////////////

CrystalClient::CrystalClient(KDecorationBridge *b,CrystalFactory *f)
    : KDecoration(b,f)
{
// 	::factory->clients.append(this);
}

CrystalClient::~CrystalClient()
{
	for (int i=0;i<ButtonTypeCount;i++)if (button[i])
	{
		delete button[i];
		button[i]=NULL;
	}
    glXMakeCurrent(qt_xdisplay(),None,NULL);	// release glXContext
// 	::factory->clients.remove(this);
}

//////////////////////////////////////////////////////////////////////////////
// init()
// ------
// Actual initializer for class

void CrystalClient::init()
{
    createMainWidget(WResizeNoErase | WRepaintNoErase);
//	QWidget *w=new CrystalWidget(initialParentWidget(),initialWFlags()|WResizeNoErase|WRepaintNoErase,this);
//	setMainWidget(w);
	
    widget()->installEventFilter(this);
	FullMax=false;
	
    // for flicker-free redraws
    widget()->setBackgroundMode(NoBackground);

    // setup layout
    mainlayout = new QGridLayout(widget(), 4, 3,0,-1,"mainlayout"); // 4x3 grid
    QHBoxLayout *titlelayout = new QHBoxLayout();
    titlebar_ = new QSpacerItem(1, ::factory->titlesize-2*FRAMESIZE+1, QSizePolicy::Expanding,
                                QSizePolicy::Fixed);

    mainlayout->setResizeMode(QLayout::FreeResize);
    mainlayout->setRowSpacing(0, FRAMESIZE);
    mainlayout->setRowSpacing(3, ::factory->borderwidth*2);
    mainlayout->setColSpacing(0, borderSpacing());
    mainlayout->setColSpacing(2, borderSpacing());

    mainlayout->addLayout(titlelayout, 1, 1);
    if (isPreview()) {
        mainlayout->addItem(new QSpacerItem(1, 1,QSizePolicy::Expanding,QSizePolicy::Fixed), 0, 1);
        mainlayout->addItem(new QSpacerItem(1, ::factory->borderwidth,QSizePolicy::Expanding,QSizePolicy::Expanding), 3, 1);
      mainlayout->addWidget(
        new QLabel(i18n("<b><center>Preview</center></b>"),widget()), 2, 1);
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
    addButtons(titlelayout, options()->titleButtonsLeft());
    titlelayout->addItem(titlebar_);
    addButtons(titlelayout, options()->titleButtonsRight());
	
    connect( this, SIGNAL( keepAboveChanged( bool )), SLOT( keepAboveChange( bool )));
    connect( this, SIGNAL( keepBelowChanged( bool )), SLOT( keepBelowChange( bool )));

    connect ( ::factory->image_holder,SIGNAL(repaintNeeded()),this,SLOT(Repaint()));
    connect ( &timer,SIGNAL(timeout()),this,SLOT(Repaint()));
	connect (&animationtimer,SIGNAL(timeout()),this,SLOT(animate()));

	animation=isActive()?1.0:0.0;

	::factory->image_holder->Init();
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

void CrystalClient::addButtons(QBoxLayout *layout, const QString& s)
{
    ButtonImage *bitmap;
    QString tip;

    if (s.length() > 0) {
        for (unsigned n=0; n < s.length(); n++) {
            switch (s[n]) {
              case 'M': // Menu button
                  if (!button[ButtonMenu]) 
				  {
                      button[ButtonMenu] = new CrystalButton(this, "menu", i18n("Menu"), ButtonMenu, 0);
                      connect(button[ButtonMenu], SIGNAL(pressed()), this, SLOT(menuButtonPressed()));
					  layout->addItem(button[ButtonMenu]->layout());
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
                      	button[ButtonSticky] =new CrystalButton(this, "sticky", tip,ButtonSticky, bitmap);
                      	connect(button[ButtonSticky], SIGNAL(clicked()),this, SLOT(toggleOnAllDesktops()));
						layout->addItem(button[ButtonSticky]->layout());
                  }
                  break;

              case 'H': // Help button
                  if (providesContextHelp()) {
                      button[ButtonHelp] =
                          new CrystalButton(this, "help", i18n("Help"),
                                            ButtonHelp, ::factory->buttonImages[ButtonImageHelp]);
                      connect(button[ButtonHelp], SIGNAL(clicked()),
                              this, SLOT(showContextHelp()));
					  layout->addItem(button[ButtonHelp]->layout());
                  }
                  break;

              case 'I': // Minimize button
                  if ((!button[ButtonMin]) && isMinimizable())  {
                      button[ButtonMin] =
                          new CrystalButton(this, "iconify", i18n("Minimize"),
                                            ButtonMin, ::factory->buttonImages[ButtonImageMin]);
                      connect(button[ButtonMin], SIGNAL(clicked()),
                              this, SLOT(minButtonPressed()));
						layout->addItem(button[ButtonMin]->layout());
                  }
                  break;


              case 'F': // Above button
                  if ((!button[ButtonAbove]))  {
                      button[ButtonAbove] =
                          new CrystalButton(this, "above", i18n("Keep Above Others"),
                                            ButtonAbove, ::factory->buttonImages[keepAbove()?ButtonImageUnAbove:ButtonImageAbove]);
                      connect(button[ButtonAbove], SIGNAL(clicked()),
                              this, SLOT(aboveButtonPressed()));
					layout->addItem(button[ButtonAbove]->layout());
                  }
                  break;

              case 'B': // Below button
                  if ((!button[ButtonBelow]))  {
                      button[ButtonBelow] =
                          new CrystalButton(this, "below", i18n("Keep Below Others"),
                                            ButtonBelow, ::factory->buttonImages[keepBelow()?ButtonImageUnBelow:ButtonImageBelow]);
                      connect(button[ButtonBelow], SIGNAL(clicked()),
                              this, SLOT(belowButtonPressed()));
						layout->addItem(button[ButtonBelow]->layout());
                  }
                  break;

				  
              case 'L': // Shade button
                  if ((!button[ButtonShade]) && isShadeable())  {
                      button[ButtonShade] =
                          new CrystalButton(this, "shade", i18n("Shade"),
                                            ButtonShade, ::factory->buttonImages[ButtonImageShade]);
                      connect(button[ButtonShade], SIGNAL(clicked()),
                              this, SLOT(shadeButtonPressed()));
                  	layout->addItem(button[ButtonShade]->layout());
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
                      button[ButtonMax]  =
                          new CrystalButton(this, "maximize", tip,
                                            ButtonMax, bitmap);
                      connect(button[ButtonMax], SIGNAL(clicked()),
                              this, SLOT(maxButtonPressed()));
					layout->addItem(button[ButtonMax]->layout());
                  }
                  break;

              case 'X': // Close button
                  if (isCloseable()) {
                      button[ButtonClose] =
                          new CrystalButton(this, "close", i18n("Close"),
                                            ButtonClose, ::factory->buttonImages[ButtonImageClose]);
                      connect(button[ButtonClose], SIGNAL(clicked()),
                              this, SLOT(closeWindow()));
					  layout->addItem(button[ButtonClose]->layout());
                  }
                  break;

              case '_': // Spacer item
                  layout->addSpacing(4);
				  break;
			}
		}
    }

}

//////////////////////////////////////////////////////////////////////////////
// activeChange()
// --------------
// window active state has changed

void CrystalClient::activeChange()
{
	if (::factory->animateActivate)
	{
		if (!animationtimer.isActive())animationtimer.start(80);
	}else{
		animation=isActive()?1.0:0.0;
		Repaint();
	}
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
//    	QToolTip::remove(button[ButtonSticky]);
//    	QToolTip::add(button[ButtonSticky], d ? i18n("Not on all desktops") : i18n("On All Desktops"));
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
//    	QToolTip::remove(button[ButtonMax]);
//    	QToolTip::add(button[ButtonMax], m ? i18n("Restore") : i18n("Maximize"));
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
		titlebar_->changeSize(1, ::factory->titlesize, QSizePolicy::Expanding,
                                QSizePolicy::Fixed);
	}else{
		mainlayout->setColSpacing(2,borderSpacing());
		mainlayout->setColSpacing(0,borderSpacing());
		titlebar_->changeSize(1, ::factory->titlesize-FRAMESIZE*2+1, QSizePolicy::Expanding,
                                QSizePolicy::Fixed);
	}
	

	mainlayout->setRowSpacing(0, (FullMax)?0:FRAMESIZE);
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
    if (!isShade())b = ::factory->borderwidth; else b=0;
	
	if (!options()->moveResizeMaximizedWindows() )
	{
		if ( maximizeMode() & MaximizeHorizontal )	l=r=1;
		if ( maximizeMode() & MaximizeVertical )
		{
			b=isShade()?0:1;
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
// 	if (crystalwidget)
// 		crystalwidget->resize(size);
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

	for (int i=0;i<ButtonTypeCount;i++)if(button[i])if (button[i]->isHover())return PositionCenter;
	
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
	  	QMouseEvent *me=static_cast<QMouseEvent *>(e);
	  	bool handled=false;
		for (int i=0;i<ButtonTypeCount;i++) if (button[i])
			handled|=button[i]->mousePressEvent(me);
		if (!handled)mouseDoubleClickEvent(static_cast<QMouseEvent *>(e));
		return true;
      }
      case QEvent::MouseButtonPress: {
	  	  QMouseEvent *me=static_cast<QMouseEvent *>(e);
		  bool handled=false;
		  for (int i=0;i<ButtonTypeCount;i++) if (button[i])
		  	handled|=button[i]->mousePressEvent(me);
		  
		  if (!handled) processMousePressEvent(me);
          return true;
      }
      case QEvent::MouseButtonRelease: {
	  	  QMouseEvent *me=static_cast<QMouseEvent *>(e);
		  
		  for (int i=0;i<ButtonTypeCount;i++) if (button[i])
				button[i]->mouseReleaseEvent(me);
          return true;
      }
	  case QEvent::MouseMove: {
	  	  for (int i=0;i<ButtonTypeCount;i++)if (button[i])
		  	button[i]->mouseMoveEvent(static_cast<QMouseEvent *>(e));
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
		  return false;
//          return true;
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

void CrystalClient::mouseWheelEvent(QWheelEvent *)
{ }

//////////////////////////////////////////////////////////////////////////////
// paintEvent()
// ------------
// Repaint the window

void drawBar(double x,double y,double w,double h)
{
	glBegin(GL_QUADS);
	glNormal3f( 0,0,-1);
    glTexCoord2f(x,y+h);		glVertex3f(x,y+h,   0);		// Bottom left
    glTexCoord2f(x+w,y+h);	glVertex3f(x+w,y+h, 0);		// Bottom right
    glTexCoord2f(x+w,y);		glVertex3f(x+w,y,   0);		// Top right
    glTexCoord2f(x,y);		glVertex3f(x,y,     0);		// Top left
	glEnd();
}

inline QColor blendColor(QColor color1,QColor color2,double balance)
{
	return QColor(color1.red()+(int)(balance*(color2.red()-color1.red())),
		color1.green()+(int)(balance*(color2.green()-color1.green())),
		color1.blue()+(int)(balance*(color2.blue()-color1.blue())));
}

void renderGlassVertex(double tx,double ty,const double x,const double y,const double z,const double angx,const double angy,const double ior)
//	ang: 0 is front side
{
	if (angx!=0.0)tx-=tan(angx-angx/ior)*(z);
	if (angy!=0.0)ty-=tan(angy-angy/ior)*(z);
		
	glTexCoord2f(tx,ty);		glVertex3f(x,y,0.0);
}

void renderGlassRect(const double x,const double y,const double w,const double h,const double ior,const double tesselation,const bool horizontal)
{
	const double width=horizontal?h:w;
	double x1,z1,ang;
	glBegin(GL_QUAD_STRIP);
	for (int i=0;i<=tesselation;i++)
	{
		ang=(double)i*M_PI/(double)tesselation-M_PI/2.0;
		x1=sin(ang)/2.0*width+width/2.0;
		z1=cos(ang)*width/2.0;
		
		if (horizontal)
		{
			renderGlassVertex(x,y+x1, 		x,y+x1,z1, 		0,ang,  ior);
			renderGlassVertex(x+w,y+x1,		x+w,y+x1,z1,	0,ang,  ior);
		}else{
			renderGlassVertex(x+x1,y, 		x+x1,y,z1, 		ang,0,  ior);
			renderGlassVertex(x+x1,y+h,		x+x1,y+h,z1,	ang,0,  ior);
		}
	}
	glEnd();
}

void CrystalClient::paintEvent(QPaintEvent*)
{
#define glColorQ(x) glColor3b(x.red()/2,x.green()/2,x.blue()/2)
	if (!CrystalFactory::initialized()) return;

	// This sets up the rendering state, if not already done, and attaches the glxcontext to the winId
	if (!::factory->initGL(widget()->winId()))return;

	if (::factory->trackdesktop)
	::factory->image_holder->repaint(false); // If other desktop than the last, regrab the root image

	glViewport(0,0,(GLint)width(),(GLint)height());

	QPoint tl=widget()->mapToGlobal(QPoint(0,0));
	
	// Translate model matrix so that it fits to screen coordinates
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0,width(),height(),0,-1,1);

	// Translates texture coordinates to fit the screen coordinates
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	glScaled(1.0/::factory->image_holder->screenwidth(),-1.0/::factory->image_holder->screenheight(),0);
	glTranslated(tl.x(),tl.y(),0);
	
	
	glEnable(GL_TEXTURE_2D);
	::factory->image_holder->activateTexture();

//	double myanimation=sin(animation*M_PI/2.0);
	double myanimation=animation;
	
	QColor color=blendColor(::factory->inactiveColor,::factory->activeColor,myanimation);
	
	
	int bl,bt,br,bb;
	borders(bl,br,bt,bb);

	
	// tint the title bar
	glColorQ(color);
	
	if (::factory->useRefraction)
	{
		const double ior=(::factory->iorActive-::factory->iorInactive)*myanimation+(::factory->iorInactive);
		// Top
		renderGlassRect(0,0,width(),bt,ior,7,true);
		// Left
		renderGlassRect(0,bt,bl,height()-bb-bt,ior/1.2,5,false);
		// Right
		renderGlassRect(width()-br,bt,br,height()-bb-bt,ior/1.2,5,false);
		// Bottom
		renderGlassRect(0,height()-bb,width(),bb,ior/1.2,5,true);
	}else{
		drawBar(0,0,width(),bt);
		// Left
		drawBar(0,bt,bl,height()-bb-bt);
		// Right
		drawBar(width()-br,bt,br,height()-bb-bt);
		// Bottom
		drawBar(0,height()-bb,width(),bb);
	}

	
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,0);
	
	if (::factory->useLighting)
	{
		const double light=1.0;
		const double lightalpha=0.15;
		const double dark=0.0;
		const double darkalpha=0.3;
		
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
		glBegin(GL_QUADS);
		// Lighten top 
		glColor4f(light,light,light,lightalpha);	
    	glVertex3f(width(),0,   0);		// Top right
    	glVertex3f(0,0,     0);			// Top left

		glColor4f(light,light,light,0.00);	
    	glVertex3f(isShade()?0:bl/2.0,bt/2.0,  0);		// Bottom left
		glVertex3f(width()-(isShade()?0:br/2.0),bt/2.0, 0);		// Bottom right

		glColor4f(dark,dark,dark,0.00);	
    	glVertex3f(width()-(isShade()?0:br/2.0),bt/2.0, 0);		// Bottom right
		glVertex3f(isShade()?0:bl/2.0,bt/2.0,  0);		// Bottom left
	
		glColor4f(dark,dark,dark,darkalpha);	
    	glVertex3f(isShade()?0:bl,bt,  0);		// Bottom left
		glVertex3f(width()-(isShade()?0:br),bt, 0);		// Bottom right
		glEnd();
    

		if (!isShade())
		{
			// Lighten bottom
			glBegin(GL_QUADS);
			glColor4f(light,light,light,lightalpha);
    		glVertex3f(width()-br,height()-bb,   0);
    		glVertex3f(bl,height()-bb,     0);

			glColor4f(light,light,light,0.00);
    		glVertex3f(bl/2.0,height()-bb/2.0,  0);
			glVertex3f(width()-br/2.0,height()-bb/2.0, 0);

			glColor4f(dark,dark,dark,0.00);
    		glVertex3f(width()-br/2.0,height()-bb/2.0, 0);
			glVertex3f(bl/2.0,height()-bb/2.0,  0);
	
			glColor4f(dark,dark,dark,darkalpha);
    		glVertex3f(0,height(),  0);
			glVertex3f(width(),height(), 0);
			
			
			// Enlighten left side
			glColor4f(light,light,light,lightalpha);	
    		glVertex3f(0,0,   0);
    		glVertex3f(0,height(),     0);

			glColor4f(light,light,light,0.00);	
    		glVertex3f(bl/2.0,height()-bb/2.0,  0);
			glVertex3f(bl/2.0,bt/2.0, 0);

			glColor4f(dark,dark,dark,0.00);	
    		glVertex3f(bl/2.0,bt/2.0, 0);
			glVertex3f(bl/2.0,height()-bb/2.0,  0);
	
			glColor4f(dark,dark,dark,darkalpha);	
    		glVertex3f(bl,height()-bb,  0);
			glVertex3f(bl,bt, 0);
			
			
			// Enlighten right side
			glColor4f(light,light,light,lightalpha);
    		glVertex3f(width()-br,bt,   0);
    		glVertex3f(width()-br,height()-bb,     0);

			glColor4f(light,light,light,0.0);	
    		glVertex3f(width()-br/2.0,height()-bb/2.0,  0);
			glVertex3f(width()-br/2.0,bt/2.0, 0);

			glColor4f(dark,dark,dark,0.00);	
    		glVertex3f(width()-br/2.0,bt/2.0, 0);
			glVertex3f(width()-br/2.0,height()-bb/2.0,  0);
	
			glColor4f(dark,dark,dark,darkalpha);	
    		glVertex3f(width(),height(),  0);
			glVertex3f(width(),0, 0);
			glEnd();
		}
   
	}
	
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();	
	
	if (!caption().isNull())
	{   // Render caption 
		QRect r=titlebar_->geometry();
		r.moveBy(-1,-1);
		QColor color1=options()->color(KDecoration::ColorFont, false);
		QColor color2=options()->color(KDecoration::ColorFont, true);
		if (::factory->textshadow)
		{	// First draw shadow
			r.moveBy(1,1);
			glColorQ(blendColor(color1.dark(300),color2.dark(300),myanimation));
			::factory->gl_font->renderText(r,CrystalFactory::titleAlign(),caption().ascii());
			r.moveBy(-1,-1);
		}
		glColorQ(blendColor(color1,color2,myanimation));
		::factory->gl_font->renderText(r,CrystalFactory::titleAlign(),caption().ascii());
	}

	double buttonFade=(::factory->fadeButtons?0.5+0.5*myanimation:1.0);
	for (int i=0;i<ButtonTypeCount;i++)if (button[i])
		button[i]->drawButton(buttonFade);
	
	
	// Swap buffers and be happy
	glXSwapBuffers( qt_xdisplay(),widget()->winId() );
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

void CrystalClient::keepAboveChange(bool /*set*/)
{
	if (button[ButtonAbove])
	{
		button[ButtonAbove]->setBitmap(::factory->buttonImages[keepAbove()?ButtonImageUnAbove:ButtonImageAbove]);
	}
}

void CrystalClient::keepBelowChange(bool/* set*/)
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
// 	QTimer::singleShot(150,this,SLOT(menuPopUp()));

	QPoint p(button[ButtonMenu]->geometry().bottomLeft().x(),
                 button[ButtonMenu]->geometry().bottomLeft().y());
	KDecorationFactory* f = factory();
	showWindowMenu(widget()->mapToGlobal(p));
	if (!f->exists(this)) return; // decoration was destroyed

}

void CrystalClient::menuPopUp()
{
	QPoint p(button[ButtonMenu]->geometry().bottomLeft().x(),
                 button[ButtonMenu]->geometry().bottomLeft().y());
	KDecorationFactory* f = factory();
	showWindowMenu(widget()->mapToGlobal(p));
	if (!f->exists(this)) return; // decoration was destroyed
}

void CrystalClient::animate()
{
	if (isActive())
	{
		animation+=0.3;
		if (animation>1.0)
		{
			animation=1.0;
			animationtimer.stop();
		}
	}else{
		animation-=0.3;
		if (animation<0.0)
		{
			animation=0.0;
			animationtimer.stop();
		}
	}
	Repaint();
}


#include "crystalclient.moc"
