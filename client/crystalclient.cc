//////////////////////////////////////////////////////////////////////////////
// exampleclient.cc
// -------------------
// Example window decoration for KDE
// -------------------
// Copyright (c) 2003, 2004 David Johnson
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////

#include <kconfig.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kimageeffect.h>
#include <kdebug.h>

#include <qbitmap.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qtooltip.h>
#include <qapplication.h>
#include <qimage.h>
#include <qtimer.h>
#include <kapp.h>
#include <netwm.h>

#include "crystalclient.h"
#include "tiles.h"


using namespace Crystal;

// global constants

static const int BUTTONSIZE      = 18;
static const int FRAMESIZE       = 1;
static const int DECOSIZE	 = 14;

static const int CORNER_EDGE_H=7;
static const int CORNER_EDGE_V=7;
static const int CORNER_EDGE_OFFSET=1;



struct WND_CONFIG
{
	int mode;	// The mode (fade, emboss, ...)
	
	double amount;
	bool frame;
}active,inactive;


static int borderwidth=1;
static bool textshadow=true;
static int titlesize = 20;
static bool trackdesktop=true;
static bool roundCorners=false;
static QColor buttonColor(255,255,255);

static QImage deco_close,deco_min,deco_sticky,deco_un_sticky,deco_max,deco_restore,deco_shade,deco_help;



//////////////////////////////////////////////////////////////////////////////
// ExampleFactory Class                                                     //
//////////////////////////////////////////////////////////////////////////////

bool ExampleFactory::initialized_              = false;
Qt::AlignmentFlags ExampleFactory::titlealign_ = Qt::AlignHCenter;


extern "C" KDecorationFactory* create_factory()
{
    return new ExampleFactory();
}

//////////////////////////////////////////////////////
// QImageHolder
//


QImageHolder::QImageHolder(ExampleFactory *vfactory)
:factory(vfactory),img_active(NULL),img_inactive(NULL)
{
	rootpixmap=NULL;
	initialized=false;
}

QImageHolder::~QImageHolder()
{
	if (rootpixmap)delete rootpixmap;
	if (img_active)delete img_active;
	if (img_inactive)delete img_inactive;
}

void QImageHolder::Init()
{
	if (initialized)return;
	
//	printf("Calling Init\n");	
	
	rootpixmap=new KMyRootPixmap(NULL/*,this*/);
	rootpixmap->start();
	rootpixmap->repaint(true);
	connect( rootpixmap,SIGNAL(backgroundUpdated(const QImage*)),this, SLOT(BackgroundUpdated(const QImage*)));
	connect(kapp, SIGNAL(backgroundChanged(int)),SLOT(handleDesktopChanged(int)));
	
	initialized=true;
}

void QImageHolder::repaint(bool force)
{
	Init(); 
	rootpixmap->repaint(force);
}

void QImageHolder::handleDesktopChanged(int)
{
	repaint(true);
}

void QImageHolder::CheckSanity()
{
	if (!initialized)return;
	if (img_active!=NULL)return;

//	printf("SanityCheck failed, uninitializing\n");	
	delete rootpixmap;
	rootpixmap=NULL;
	
	initialized=false;
//	printf("Uninitialized.\n");	
	
}


void ApplyEffect(QImage &src,QImage &dst,WND_CONFIG* cfg,QColorGroup colorgroup)
{
	switch(cfg->mode)
	{
	case 0:dst=KImageEffect::fade(src, cfg->amount, colorgroup.background());
		break;
	case 1:dst=KImageEffect::channelIntensity(src,cfg->amount,KImageEffect::All);
		break;
	case 2:dst=KImageEffect::intensity(src,cfg->amount);
		break;
	case 3:dst=KImageEffect::desaturate(src,cfg->amount);
		break;
	case 4: dst=src;
		KImageEffect::solarize(dst,cfg->amount*100.0);
		break;
//	case 5:dst=KImageEffect::emboss(src);
//		break;
//	case 6:dst=KImageEffect::charcoal(src);
//		break;

	default:dst=src;
		break;	
	}
}

void QImageHolder::BackgroundUpdated(const QImage *src)
{
	if (img_active)
	{
		delete img_active;
		img_active=NULL;
	}
	if (img_inactive)
	{
		delete img_inactive;
		img_inactive=NULL;
	}
	
	if (src && !src->isNull())
	{
		src->scale(1024,768);
		QImage tmp=src->copy();

		if (!img_active)img_active=new QImage;
		if (!img_inactive)img_inactive=new QImage;
	
		ApplyEffect(tmp,*img_inactive,&inactive,factory->options()->colorGroup(KDecoration::ColorTitleBar, false));
		tmp=src->copy();
		ApplyEffect(tmp,*img_active,&active,factory->options()->colorGroup(KDecoration::ColorTitleBar, true));
	}
	
	emit repaintNeeded();	
}




//////////////////////////////////////////////////////////////////////////////
// ExampleFactory()
// ----------------
// Constructor

ExampleFactory::ExampleFactory()
{
    readConfig();
    initialized_ = true;
    initButtonPixmaps();
    
    image_holder=new QImageHolder(this);
}

//////////////////////////////////////////////////////////////////////////////
// ~ExampleFactory()
// -----------------
// Destructor

ExampleFactory::~ExampleFactory() 
{ 
	initialized_ = false; 
	if (image_holder)delete image_holder;
}


//////////////////////////////////////////////////////////////////////////////
// createDecoration()
// -----------------
// Create the decoration

KDecoration* ExampleFactory::createDecoration(KDecorationBridge* b)
{
    return new ExampleClient(b, this);
}

//////////////////////////////////////////////////////////////////////////////
// reset()
// -------
// Reset the handler. Returns true if decorations need to be remade, false if
// only a repaint is necessary

QImage makeButtonImage(uchar* data)
{
	QImage img=QImage(data,DECOSIZE,DECOSIZE,32,NULL,0,QImage::LittleEndian);
	img.setAlphaBuffer(true);
	
	img=img.copy();
	return KImageEffect::blend(buttonColor,img,1.0f);
}

void ExampleFactory::initButtonPixmaps()
{
	deco_close=makeButtonImage((uchar*)crystal_close_data);
	deco_min=makeButtonImage((uchar*)crystal_min_data);
	deco_sticky=makeButtonImage((uchar*)crystal_sticky_data);
	deco_un_sticky=makeButtonImage((uchar*)crystal_un_sticky_data);
	deco_max=makeButtonImage((uchar*)crystal_max_data);
	deco_restore=makeButtonImage((uchar*)crystal_restore_data);
	deco_shade=makeButtonImage((uchar*)crystal_shade_data);
	deco_help=makeButtonImage((uchar*)crystal_help_data);
}

bool ExampleFactory::reset(unsigned long changed)
{
    // read in the configuration
    initialized_ = false;
    bool confchange = readConfig();
    initButtonPixmaps();
        
    initialized_ = true;

    image_holder->repaint(true);
    
    if (confchange ||
        (changed & (SettingDecoration | SettingButtons | SettingBorder))) {
        return true;
    } else {
        resetDecorations(changed);
        return false;
    }
}

//////////////////////////////////////////////////////////////////////////////
// readConfig()
// ------------
// Read in the configuration file

bool ExampleFactory::readConfig()
{
    // create a config object
    KConfig config("kwincrystalrc");
    config.setGroup("General");

    // grab settings
//    Qt::AlignmentFlags oldalign = titlealign_;
        
    
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
    
    borderwidth=config.readNumEntry("Borderwidth",4);
    titlesize=config.readNumEntry("Titlebarheight",20);
 
    buttonColor=config.readColorEntry("ButtonColor",&buttonColor);
    roundCorners=config.readBoolEntry("RoundCorners",false);

       
    return true;
}



//////////////////////////////////////////////////////////////////////////////
// ExampleButton Class                                                      //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// ExampleButton()
// ---------------
// Constructor

ExampleButton::ExampleButton(ExampleClient *parent, const char *name,
                             const QString& tip, ButtonType type,
                             QImage *bitmap)
    : QButton(parent->widget(), name), client_(parent), type_(type),
      deco_(0), lastmouse_(0)
{
    setBackgroundMode(NoBackground);
    setFixedSize(buttonSize(), buttonSize());
    setCursor(arrowCursor);

    if (bitmap==NULL || bitmap->isNull())deco_=NULL;
    	else deco_=bitmap;

    QToolTip::add(this, tip);
}

ExampleButton::~ExampleButton()
{

}

//////////////////////////////////////////////////////////////////////////////
// sizeHint()
// ----------
// Return size hint

QSize ExampleButton::sizeHint() const
{
	return QSize(buttonSize(),buttonSize());
}

int ExampleButton::buttonSize() const
{
	return (titlesize>BUTTONSIZE)?BUTTONSIZE:titlesize;
}

//////////////////////////////////////////////////////////////////////////////
// enterEvent()
// ------------
// Mouse has entered the button

void ExampleButton::enterEvent(QEvent *e)
{
    // if we wanted to do mouseovers, we would keep track of it here
    QButton::enterEvent(e);
}

//////////////////////////////////////////////////////////////////////////////
// leaveEvent()
// ------------
// Mouse has left the button

void ExampleButton::leaveEvent(QEvent *e)
{
    // if we wanted to do mouseovers, we would keep track of it here
    QButton::leaveEvent(e);
}

//////////////////////////////////////////////////////////////////////////////
// mousePressEvent()
// -----------------
// Button has been pressed

void ExampleButton::mousePressEvent(QMouseEvent* e)
{
    lastmouse_ = e->button();
    int button;
    // translate and pass on mouse event
    switch(e->button())
    {
    	case LeftButton:
		button=LeftButton;
		break;
	case RightButton:
		if ((type_ == ButtonMax) || (type_ == ButtonMin) || (type_ == ButtonMenu))
			button=LeftButton; else button=NoButton;
		break;
	case MidButton:
		if ((type_ == ButtonMax) || (type_ == ButtonMin))
			button=LeftButton; else button=NoButton;
		break;
	
	default:button=NoButton;
		break;
    }
    
    
    QMouseEvent me(e->type(), e->pos(), e->globalPos(),
                   button, e->state());
    QButton::mousePressEvent(&me);
}

//////////////////////////////////////////////////////////////////////////////
// mouseReleaseEvent()
// -----------------
// Button has been released

void ExampleButton::mouseReleaseEvent(QMouseEvent* e)
{
    lastmouse_ = e->button();
    int button;
    // translate and pass on mouse event
    switch(e->button())
    {
    	case LeftButton:
		button=LeftButton;
		break;
	case RightButton:
		if ((type_ == ButtonMax) || (type_ == ButtonMin) || (type_ == ButtonMenu))
			button=LeftButton; else button=NoButton;
		break;
	case MidButton:
		if ((type_ == ButtonMax) || (type_ == ButtonMin))
			button=LeftButton; else button=NoButton;
		break;
	
	default:button=NoButton;
		break;
    }
    QMouseEvent me(e->type(), e->pos(), e->globalPos(), button, e->state());
    QButton::mouseReleaseEvent(&me);
}

//////////////////////////////////////////////////////////////////////////////
// drawButton()
// ------------
// Draw the button

void ExampleButton::drawButton(QPainter *painter)
{
    if (!ExampleFactory::initialized()) return;
    
    QColorGroup group;
    int dx, dy;

    QPixmap pufferPixmap;
    pufferPixmap.resize(BUTTONSIZE, BUTTONSIZE);
    QPainter pufferPainter(&pufferPixmap);
    
    // paint a plain box with border
    QImage *background=((ExampleFactory*)client_->factory())->image_holder->image(client_->isActive());
    if (background)
    {
    	QRect r=rect();
	QPoint p=mapToGlobal(QPoint(0,0));
	r.moveBy(p.x(),p.y());
	
	pufferPainter.drawImage(QPoint(0,0),*background,r);
    }else{
	group = client_->options()->colorGroup(KDecoration::ColorTitleBar, client_->isActive());
	pufferPainter.fillRect(rect(), group.background());
    }

    QRect r(1,1,rect().width()-2,rect().height()-2);

    if (type_ == ButtonMenu) {
        // we paint the mini icon (which is 16 pixels high)
        dx = (width() - 16) / 2;
        dy = (height() - 16) / 2;

	if (dx<1 || dy<1)
	{
		if (isDown()) { r.moveBy(1,1); }
        	pufferPainter.drawPixmap(r, client_->icon().pixmap(QIconSet::Small,
                                                           QIconSet::Normal));
	}else{
        	if (isDown()) { dx++; dy++; }
		pufferPainter.drawPixmap(dx, dy, client_->icon().pixmap(QIconSet::Small,
                                                           QIconSet::Normal));
	}
    } else if (deco_) {
        // otherwise we paint the deco
	
        dx = (width() - DECOSIZE) / 2;
        dy = (height() - DECOSIZE) / 2;
	
	if (dx<1 || dy<1)
	{	// Deco size is smaller than image, we need to stretch it
		
		pufferPainter.drawImage(r,*deco_);
		if (isDown())
			pufferPainter.drawImage(r,*deco_);
	}else{
		// Otherwise we just paint it
		pufferPainter.drawImage(QPoint(dx,dy),*deco_);
		if (isDown())
			pufferPainter.drawImage(QPoint(dx,dy),*deco_);
	}
    }
	pufferPainter.end();
    	painter->drawPixmap(0,0, pufferPixmap);    

}

//////////////////////////////////////////////////////////////////////////////
// ExampleClient Class                                                      //
//////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////
// ExampleClient()
// ---------------
// Constructor

ExampleClient::ExampleClient(KDecorationBridge *b, ExampleFactory *f)
    : KDecoration(b, f) , my_factory(f)
{ 
}

ExampleClient::~ExampleClient()
{
    for (int n=0; n<ButtonTypeCount; n++) {
        if (button[n]) delete button[n];
    }
}

//////////////////////////////////////////////////////////////////////////////
// init()
// ------
// Actual initializer for class

void ExampleClient::init()
{
    createMainWidget(WResizeNoErase | WRepaintNoErase);
    widget()->installEventFilter(this);

    // for flicker-free redraws
    widget()->setBackgroundMode(NoBackground);

    // setup layout
    QGridLayout *mainlayout = new QGridLayout(widget(), 4, 3); // 4x3 grid
    QHBoxLayout *titlelayout = new QHBoxLayout();
    titlebar_ = new QSpacerItem(1, titlesize, QSizePolicy::Expanding,
                                QSizePolicy::Fixed);

    mainlayout->setResizeMode(QLayout::FreeResize);
    mainlayout->addRowSpacing(0, FRAMESIZE);
    mainlayout->addRowSpacing(3, FRAMESIZE*2);
    mainlayout->addColSpacing(0, FRAMESIZE*2);
    mainlayout->addColSpacing(2, FRAMESIZE*2);

    mainlayout->addLayout(titlelayout, 1, 1);
    if (isPreview()) {
        mainlayout->addWidget(
        new QLabel(i18n("<b><center>Example preview</center></b>"),
        widget()), 2, 1);
    } else {
        mainlayout->addItem(new QSpacerItem(0, 0), 2, 1);
    }

    // the window should stretch
    mainlayout->setRowStretch(2, 10);
    mainlayout->setColStretch(1, 10);
    
    updateMask();    
    
    // setup titlebar buttons
    titlelayout->addItem(new QSpacerItem(borderwidth/2,0));
    
    for (int n=0; n<ButtonTypeCount; n++) button[n] = 0;
    addButtons(titlelayout, options()->titleButtonsLeft());
    titlelayout->addItem(titlebar_);
    addButtons(titlelayout, options()->titleButtonsRight());
    titlelayout->addItem(new QSpacerItem(borderwidth/2,0));

    connect ( my_factory->image_holder,SIGNAL(repaintNeeded()),this,SLOT(Repaint()));
    connect ( &timer,SIGNAL(timeout()),this,SLOT(Repaint()));
}

void ExampleClient::updateMask()
{
	if (!roundCorners)
	{
		mask=QRegion(widget()->rect());
		setMask(mask);
//		clearMask();
		return;
	}
	// Two rects, which leave the four corners blank	
	mask =QRegion(CORNER_EDGE_H,0,width()-CORNER_EDGE_H*2,height());
	mask+=QRegion(0,CORNER_EDGE_V,width(),height()-CORNER_EDGE_V*2);
	
	// Two upper edges
	mask+=QRegion(CORNER_EDGE_OFFSET,CORNER_EDGE_OFFSET,CORNER_EDGE_H*2,CORNER_EDGE_V*2,QRegion::Ellipse);
	mask+=QRegion(width()-CORNER_EDGE_H*2+1-CORNER_EDGE_OFFSET,CORNER_EDGE_OFFSET,CORNER_EDGE_H*2,CORNER_EDGE_V*2,QRegion::Ellipse);

	// Lower edges
	mask+=QRegion(CORNER_EDGE_OFFSET,height()-CORNER_EDGE_V*2+1-CORNER_EDGE_OFFSET,CORNER_EDGE_H*2,CORNER_EDGE_V*2,QRegion::Ellipse);
	mask+=QRegion(width()-CORNER_EDGE_H*2+1-CORNER_EDGE_OFFSET,height()-CORNER_EDGE_V*2+1-CORNER_EDGE_OFFSET,CORNER_EDGE_H*2,CORNER_EDGE_V*2,QRegion::Ellipse);
	
	setMask(mask);
}

//////////////////////////////////////////////////////////////////////////////
// addButtons()
// ------------
// Add buttons to title layout

void ExampleClient::addButtons(QBoxLayout *layout, const QString& s)
{
    QImage *bitmap;
    QString tip;

    if (s.length() > 0) {
        for (unsigned n=0; n < s.length(); n++) {
            switch (s[n]) {
              case 'M': // Menu button
                  if (!button[ButtonMenu]) {
                      button[ButtonMenu] =
                          new ExampleButton(this, "menu", i18n("Menu"),
                                            ButtonMenu, 0);
                      connect(button[ButtonMenu], SIGNAL(pressed()),
                              this, SLOT(menuButtonPressed()));
                      layout->addWidget(button[ButtonMenu]);
                  }
                  break;

              case 'S': // Sticky button
                  if (!button[ButtonSticky]) {
              if (isOnAllDesktops()) {
              bitmap = &deco_sticky;
              tip = i18n("Not On All Desktops");
              } else {
              bitmap = &deco_un_sticky;
              tip = i18n("On All Desktops");
              }
                      button[ButtonSticky] =
                          new ExampleButton(this, "sticky", tip,
                                            ButtonSticky, bitmap);
                      connect(button[ButtonSticky], SIGNAL(clicked()),
                              this, SLOT(toggleOnAllDesktops()));
                      layout->addWidget(button[ButtonSticky]);
                  }
                  break;

              case 'H': // Help button
                  if ((!button[ButtonHelp]) && providesContextHelp()) {
                      button[ButtonHelp] =
                          new ExampleButton(this, "help", i18n("Help"),
                                            ButtonHelp, &deco_help);
                      connect(button[ButtonHelp], SIGNAL(clicked()),
                              this, SLOT(showContextHelp()));
                      layout->addWidget(button[ButtonHelp]);
                  }
                  break;

              case 'I': // Minimize button
                  if ((!button[ButtonMin]) && isMinimizable())  {
                      button[ButtonMin] =
                          new ExampleButton(this, "iconify", i18n("Minimize"),
                                            ButtonMin, &deco_min);
                      connect(button[ButtonMin], SIGNAL(clicked()),
                              this, SLOT(minButtonPressed()));
                      layout->addWidget(button[ButtonMin]);
                  }
                  break;

              case 'L': // Shade button
                  if ((!button[ButtonShade]) && isShadeable())  {
                      button[ButtonShade] =
                          new ExampleButton(this, "shade", i18n("Shade"),
                                            ButtonShade, &deco_shade);
                      connect(button[ButtonShade], SIGNAL(clicked()),
                              this, SLOT(shadeButtonPressed()));
                      layout->addWidget(button[ButtonShade]);
                  }
                  break;
		  
              case 'A': // Maximize button
                  if ((!button[ButtonMax]) && isMaximizable()) {
              if (maximizeMode() == MaximizeFull) {
              bitmap = &deco_restore;
              tip = i18n("Restore");
              } else {
              bitmap = &deco_max;
              tip = i18n("Maximize");
              }
                      button[ButtonMax]  =
                          new ExampleButton(this, "maximize", tip,
                                            ButtonMax, bitmap);
                      connect(button[ButtonMax], SIGNAL(clicked()),
                              this, SLOT(maxButtonPressed()));
                      layout->addWidget(button[ButtonMax]);
                  }
                  break;

              case 'X': // Close button
                  if ((!button[ButtonClose]) && isCloseable()) {
                      button[ButtonClose] =
                          new ExampleButton(this, "close", i18n("Close"),
                                            ButtonClose, &deco_close);
                      connect(button[ButtonClose], SIGNAL(clicked()),
                              this, SLOT(closeWindow()));
                      layout->addWidget(button[ButtonClose]);
                  }
                  break;

              case '_': // Spacer item
                  layout->addSpacing(FRAMESIZE);
            }
    }
    }
}

//////////////////////////////////////////////////////////////////////////////
// activeChange()
// --------------
// window active state has changed

void ExampleClient::activeChange()
{
	Repaint();
}

//////////////////////////////////////////////////////////////////////////////
// captionChange()
// ---------------
// The title has changed

void ExampleClient::captionChange()
{
    widget()->repaint(titlebar_->geometry(), false);
}

//////////////////////////////////////////////////////////////////////////////
// desktopChange()
// ---------------
// Called when desktop/sticky changes

void ExampleClient::desktopChange()
{
    bool d = isOnAllDesktops();
    if (button[ButtonSticky]) {
        button[ButtonSticky]->setBitmap(d ? &deco_sticky : &deco_un_sticky);
    	QToolTip::remove(button[ButtonSticky]);
    	QToolTip::add(button[ButtonSticky], d ? i18n("Not On All Desktops") : i18n("On All Desktops"));
    }
    
//    wallpaper->repaint(true);
}

//////////////////////////////////////////////////////////////////////////////
// iconChange()
// ------------
// The title has changed

void ExampleClient::iconChange()
{
    if (button[ButtonMenu]) {
        button[ButtonMenu]->setBitmap(0);
        button[ButtonMenu]->repaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// maximizeChange()
// ----------------
// Maximized state has changed

void ExampleClient::maximizeChange()
{
    bool m = (maximizeMode() == MaximizeFull);
    if (button[ButtonMax]) {
        button[ButtonMax]->setBitmap(m ? &deco_restore : &deco_max);
    QToolTip::remove(button[ButtonMax]);
    QToolTip::add(button[ButtonMax], m ? i18n("Restore") : i18n("Maximize"));
    }
}

//////////////////////////////////////////////////////////////////////////////
// shadeChange()
// -------------
// Called when window shading changes

void ExampleClient::shadeChange()
{ ; }

//////////////////////////////////////////////////////////////////////////////
// borders()
// ----------
// Get the size of the borders

void ExampleClient::borders(int &l, int &r, int &t, int &b) const
{
    l = r = FRAMESIZE*borderwidth;
    t = titlesize + FRAMESIZE;
    if (!isShade())b = FRAMESIZE * borderwidth; else b=FRAMESIZE;
}

//////////////////////////////////////////////////////////////////////////////
// resize()
// --------
// Called to resize the window

void ExampleClient::resize(const QSize &size)
{
    widget()->resize(size);
}

//////////////////////////////////////////////////////////////////////////////
// minimumSize()
// -------------
// Return the minimum allowable size for this window

QSize ExampleClient::minimumSize() const
{
    return widget()->minimumSize();
}

//////////////////////////////////////////////////////////////////////////////
// mousePosition()
// ---------------
// Return logical mouse position

KDecoration::Position ExampleClient::mousePosition(const QPoint &point) const
{
    const int corner = 24;
    Position pos;
    const int RESIZESIZE=FRAMESIZE*borderwidth;

    if (isShade()) pos=PositionCenter; 
    else if (point.y() <= FRAMESIZE*3) {
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

bool ExampleClient::eventFilter(QObject *obj, QEvent *e)
{
    if (obj != widget()) return false;

    switch (e->type()) {
      case QEvent::MouseButtonDblClick: {
          mouseDoubleClickEvent(static_cast<QMouseEvent *>(e));
          return true;
      }
      case QEvent::MouseButtonPress: {
          processMousePressEvent(static_cast<QMouseEvent *>(e));
          return true;
      }
      case QEvent::Paint: {
          paintEvent(static_cast<QPaintEvent *>(e));
          return true;
      }
      case QEvent::Resize: {
          resizeEvent(static_cast<QResizeEvent *>(e));
          return true;
      }
      case QEvent::Show: {
          showEvent(static_cast<QShowEvent *>(e));
          return true;
      case QEvent::Move: {
          moveEvent(static_cast<QMoveEvent *>(e));	  
	  return true;
      }
      }
      default: {
          return false;
      }
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////////
// mouseDoubleClickEvent()
// -----------------------
// Doubleclick on title

void ExampleClient::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (titlebar_->geometry().contains(e->pos())) titlebarDblClickOperation();
}

//////////////////////////////////////////////////////////////////////////////
// paintEvent()
// ------------
// Repaint the window

void ExampleClient::paintEvent(QPaintEvent*)
{
    if (!ExampleFactory::initialized()) return;

    QColorGroup group;
    QPainter painter(widget());

    // draw the titlebar
    group = options()->colorGroup(KDecoration::ColorTitleBar, isActive());
   
    if (trackdesktop)
    	my_factory->image_holder->repaint(false); // If other desktop than the last, regrab the root image
    QImage *background=my_factory->image_holder->image(isActive());
    
    if (background)
    {
	QRect r;
	QPoint p=widget()->mapToGlobal(QPoint(0,0));
    	int bl,br,bt,bb;
	borders(bl,br,bt,bb);
    	    
	QPixmap pufferPixmap;
    	pufferPixmap.resize(widget()->width(), bt);
    	QPainter pufferPainter(&pufferPixmap);

	r=QRect(p.x(),p.y(),widget()->width(),bt);
	pufferPainter.drawImage(QPoint(0,0),*background,r);
	
    	// draw title text
    	pufferPainter.setFont(options()->font(isActive(), false));
	
	QColor color=options()->color(KDecoration::ColorFont, isActive());
//	r=QRect(FRAMESIZE + FRAMESIZE+BUTTONSIZE*2, FRAMESIZE, width() - FRAMESIZE - FRAMESIZE * 2-BUTTONSIZE*4, titlesize-1);
	r=titlebar_->geometry();
	
	if (textshadow)
	{
		pufferPainter.translate(1,1);
	    	pufferPainter.setPen(color.dark(200));
    		pufferPainter.drawText(r,
                     	ExampleFactory::titleAlign() | AlignVCenter,
                     	caption());
		pufferPainter.translate(-1,-1);
	}
	
    	pufferPainter.setPen(color);
    	pufferPainter.drawText(r,
                     ExampleFactory::titleAlign() | AlignVCenter,
                     caption());

	if (borderwidth>0)
	{	// Draw the side and bottom of the window with transparency
		r=QRect(p.x(),p.y()+bt,bl,widget()->height()-bt);
		painter.drawImage(QPoint(0,bt),*background,r);

		r=QRect(widget()->width()-br+p.x(),p.y()+bt,widget()->width(),widget()->height()-bt);
		painter.drawImage(QPoint(widget()->width()-br,bt),*background,r);

		r=QRect(p.x()+bl,p.y()+widget()->height()-bb,widget()->width()-bl-br,bb);
		painter.drawImage(QPoint(bl,widget()->height()-bb),*background,r);
	}
	
	pufferPainter.end();
	painter.drawPixmap(0,0,pufferPixmap);
    }else{	// We don't have a background image, draw a solid rectangle
	group = options()->colorGroup(KDecoration::ColorTitleBar, isActive());
	painter.fillRect(widget()->rect(), group.background());
	// And notify image_holder that we need an update asap
	if (my_factory)if (my_factory->image_holder)
		// UnInit image_holder, on next Repaint it will be Init'ed again.
		QTimer::singleShot(500,my_factory->image_holder,SLOT(CheckSanity()));
    }

    // draw frame
    if ((isActive()&& active.frame)||(!isActive() && inactive.frame))
    {
    	group = options()->colorGroup(KDecoration::ColorFrame, isActive());

    	// outline the frame
	if (!roundCorners)
	{
    		painter.setPen(group.dark());
		painter.drawRect(widget()->rect());
	}
    }
}

//////////////////////////////////////////////////////////////////////////////
// resizeEvent()
// -------------
// Window is being resized

void ExampleClient::resizeEvent(QResizeEvent *)
{
    if (widget()->isShown()) {
        // repaint only every 200 ms
	if (!timer.isActive())timer.start(200,true);
	updateMask();
    }
}

void ExampleClient::moveEvent(QMoveEvent *)
{
    if (widget()->isShown()) {
	// repaint every 200 ms, so constant moving does not take too much CPU
	if (!timer.isActive())timer.start(200,true);
    }	
}

//////////////////////////////////////////////////////////////////////////////
// showEvent()
// -----------
// Window is being shown

void ExampleClient::showEvent(QShowEvent *)
{
	if (widget()->isShown()) 
		Repaint();
}

void ExampleClient::Repaint()
{
	widget()->repaint(false);
        for (int n=0; n<ButtonTypeCount; n++)
              if (button[n]) button[n]->reset();
}

//////////////////////////////////////////////////////////////////////////////
// maxButtonPressed()
// -----------------
// Max button was pressed

void ExampleClient::maxButtonPressed()
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

void ExampleClient::minButtonPressed()
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

void ExampleClient::shadeButtonPressed()
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

void ExampleClient::menuButtonPressed()
{
	if (!button[ButtonMenu])return;
	
	static QTime* t = 0;
	static ExampleClient* lastClient = 0;
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
		
	QPoint p(button[ButtonMenu]->rect().bottomLeft().x(),
                 button[ButtonMenu]->rect().bottomLeft().y());
	KDecorationFactory* f = factory();
	showWindowMenu(button[ButtonMenu]->mapToGlobal(p));
	if (!f->exists(this)) return; // decoration was destroyed
	button[ButtonMenu]->setDown(false);

}

#include "crystalclient.moc"
