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

#include "crystalclient.h"
#include "tiles.h"

using namespace Example;

// global constants

static const int BUTTONSIZE      = 18;
static const int TITLESIZE       = 24;
static const int FRAMESIZE       = 1;

struct WND_CONFIG
{
	int mode;	// The mode (fade, emboss, ...)
	
	double amount;
	bool frame;

}active,inactive;

int borderwidth=1;



QImage deco_close((uchar*)crystal_close_data,18,18,32,NULL,0,QImage::LittleEndian);
QImage deco_min((uchar*)crystal_min_data,18,18,32,NULL,0,QImage::LittleEndian);
QImage deco_sticky((uchar*)crystal_sticky_data,18,18,32,NULL,0,QImage::LittleEndian);
QImage deco_un_sticky((uchar*)crystal_un_sticky_data,18,18,32,NULL,0,QImage::LittleEndian);
QImage deco_max((uchar*)crystal_max_data,18,18,32,NULL,0,QImage::LittleEndian);
QImage deco_restore((uchar*)crystal_restore_data,18,18,32,NULL,0,QImage::LittleEndian);
QImage deco_help((uchar*)crystal_help_data,18,18,32,NULL,0,QImage::LittleEndian);


//////////////////////////////////////////////////////////////////////////////
// ExampleFactory Class                                                     //
//////////////////////////////////////////////////////////////////////////////

bool ExampleFactory::initialized_              = false;
Qt::AlignmentFlags ExampleFactory::titlealign_ = Qt::AlignHCenter;

extern "C" KDecorationFactory* create_factory()
{
    return new Example::ExampleFactory();
}

//////////////////////////////////////////////////////////////////////////////
// ExampleFactory()
// ----------------
// Constructor

ExampleFactory::ExampleFactory()
{
    readConfig();
    initialized_ = true;
    
    deco_close.setAlphaBuffer(true);
    deco_max.setAlphaBuffer(true);
    deco_restore.setAlphaBuffer(true);
    deco_sticky.setAlphaBuffer(true);
    deco_un_sticky.setAlphaBuffer(true);
    deco_min.setAlphaBuffer(true);
    deco_help.setAlphaBuffer(true);
}

//////////////////////////////////////////////////////////////////////////////
// ~ExampleFactory()
// -----------------
// Destructor

ExampleFactory::~ExampleFactory() { initialized_ = false; }

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

bool ExampleFactory::reset(unsigned long changed)
{
    // read in the configuration
    initialized_ = false;
    bool confchange = readConfig();
    initialized_ = true;

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
    Qt::AlignmentFlags oldalign = titlealign_;
        
    
    QString value = config.readEntry("TitleAlignment", "AlignHCenter");
    if (value == "AlignLeft") titlealign_ = Qt::AlignLeft;
    else if (value == "AlignHCenter") titlealign_ = Qt::AlignHCenter;
    else if (value == "AlignRight") titlealign_ = Qt::AlignRight;

    active.mode=config.readNumEntry("ActiveMode",0);
    inactive.mode=config.readNumEntry("InactiveMode",0);
    active.amount=(double)config.readNumEntry("ActiveShade",50)/100.0;
    inactive.amount=(double)config.readNumEntry("InactiveShade",50)/100.0;
    active.frame=(bool)config.readBoolEntry("ActiveFrame",true);
    inactive.frame=(bool)config.readBoolEntry("InactiveFrame",true);
    borderwidth=config.readNumEntry("Borderwidth",4);
    
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
    setFixedSize(BUTTONSIZE, BUTTONSIZE);
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
    return QSize(BUTTONSIZE, BUTTONSIZE);
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

    // translate and pass on mouse event
    int button = LeftButton;
    if ((type_ != ButtonMax) && (e->button() != LeftButton)) {
        button = NoButton; // middle & right buttons inappropriate
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

    // translate and pass on mouse event
    int button = LeftButton;
    if ((type_ != ButtonMax) && (e->button() != LeftButton)) {
        button = NoButton; // middle & right buttons inappropriate
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
    group = client_->options()->colorGroup(KDecoration::ColorTitleBar, client_->isActive());
    pufferPainter.fillRect(rect(), group.background());

    if (client_->background)
    {
    	QRect r=rect();
//	QPoint p(0,0);
	r.moveBy(pos().x(),pos().y());
	
	pufferPainter.drawImage(QPoint(0,0),*client_->background,r);
    }


    if (type_ == ButtonMenu) {
        // we paint the mini icon (which is 16 pixels high)
        dx = (width() - 16) / 2;
        dy = (height() - 16) / 2;
        if (isDown()) { dx++; dy++; }

//	const QPixmap px(client_->icon().pixmap(QIconSet::Small,QIconSet::Normal));
//	QImage img(px.convertToImage().copy(0,0,BUTTONSIZE,BUTTONSIZE));
	
//	dst=KImageEffect::blend(img,dst,0.4);

        pufferPainter.drawPixmap(dx, dy, client_->icon().pixmap(QIconSet::Small,
                                                           QIconSet::Normal));

	pufferPainter.end();
    	painter->drawPixmap(0,0, pufferPixmap);    
    } else if (deco_) {
        // otherwise we paint the deco
//    	pufferPainter.end();

//    	QImage dst(pufferPixmap.convertToImage());
//	QImage img(deco_->convertToImage().copy(0,0,BUTTONSIZE,BUTTONSIZE));
//	img.setMask(img);

//        dx = (width() - DECOSIZE) / 2;
//        dy = (height() - DECOSIZE) / 2;
	dx=0;
	dy=0;
//        if (isDown()) { dx++; dy++; }
//        pufferPainter.setPen(group.light());
//        pufferPainter.drawPixmap(dx, dy, *deco_);
//	dst=KImageEffect::blend(img,dst,0.7);
	
//    	painter->drawImage(QPoint(0,0), dst);

//        pufferPainter.drawImage(QPoint(dx, dy), *deco_);
	pufferPainter.drawImage(QPoint(dx,dy),*deco_);
	
	if (isDown())
		pufferPainter.drawImage(QPoint(dx,dy),*deco_);


	pufferPainter.end();
    	painter->drawPixmap(0,0, pufferPixmap);    
    }

}

//////////////////////////////////////////////////////////////////////////////
// ExampleClient Class                                                      //
//////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////
// ExampleClient()
// ---------------
// Constructor

ExampleClient::ExampleClient(KDecorationBridge *b, KDecorationFactory *f)
    : KDecoration(b, f) { ; }

ExampleClient::~ExampleClient()
{
    if (background)delete background;
    if (kWinModule)delete kWinModule;
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
    titlebar_ = new QSpacerItem(1, TITLESIZE, QSizePolicy::Expanding,
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

    QColorGroup group = options()->colorGroup(KDecoration::ColorTitleBar, isActive());
    background=NULL;
    wallpaper=new KMyRootPixmap(widget());
    wallpaper->repaint(true);
    wallpaper->start();
    connect( wallpaper,SIGNAL(backgroundUpdated(const QImage&)),this, SLOT(BackgroundUpdated(const QImage&)));        
    connect( wallpaper,SIGNAL(applyEffect(QImage&,QImage&)),this,SLOT(applyEffect(QImage&,QImage&)));
    
    // setup titlebar buttons
    for (int n=0; n<ButtonTypeCount; n++) button[n] = 0;
    addButtons(titlelayout, options()->titleButtonsLeft());
    titlelayout->addItem(titlebar_);
    addButtons(titlelayout, options()->titleButtonsRight());
    
    kWinModule = new KWinModule(0,KWinModule::INFO_DESKTOP);
    connect( kWinModule,SIGNAL(currentDesktopChanged(int)), this,SLOT(DesktopChanged(int)) );
}

void ExampleClient::BackgroundUpdated(const QImage &bg)
{
	if (background)delete background;
	background=NULL;
	
	background=new QImage(bg);
	
	widget()->repaint(false);
	for (int n=0; n<ButtonTypeCount; n++) if (button[n])
		button[n]->repaint(false);
}

void ExampleClient::DesktopChanged(int desk)
{
	if (isOnAllDesktops() || desk==desktop())
		wallpaper->repaint(true);
}

void ExampleClient::applyEffect(QImage &src,QImage &dst)
{
	QColorGroup group = options()->colorGroup(KDecoration::ColorTitleBar, isActive());
	WND_CONFIG* cfg=isActive()?&active:&inactive;
	
	switch(cfg->mode)
	{
	case 0:dst=KImageEffect::fade(src, cfg->amount, group.background());
		break;
	case 1:dst=KImageEffect::channelIntensity(src,cfg->amount,KImageEffect::All);
		break;
	case 2:dst=KImageEffect::intensity(src,cfg->amount);
		break;
	case 3:dst=KImageEffect::desaturate(src,cfg->amount);
		break;
	case 4: KImageEffect::solarize(src,cfg->amount*100.0);
		dst=src;
		break;
	case 5:dst=KImageEffect::emboss(src);
		break;
	case 6:dst=KImageEffect::charcoal(src);
		break;

		
	default:dst=src;
		break;	
	}
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
                              this, SLOT(minimize()));
                      layout->addWidget(button[ButtonMin]);
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
    for (int n=0; n<ButtonTypeCount; n++)
        if (button[n]) button[n]->reset();
	
    wallpaper->repaint(true);
    widget()->repaint(false);
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
    
    wallpaper->repaint(true);
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
    t = TITLESIZE + FRAMESIZE;
    b = FRAMESIZE * borderwidth;
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

    if (point.y() <= FRAMESIZE*4) {
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
//    if (button[ButtonMenu]->geometry().contains(e->pos()))closeWindow(); 
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
    QRect title(FRAMESIZE,FRAMESIZE,width()-FRAMESIZE,TITLESIZE-FRAMESIZE);
    group = options()->colorGroup(KDecoration::ColorTitleBar, isActive());
   
    
    if (background!=NULL) 
    {    
    	QPoint p(0,0);
/*    	QRect src=title;
    	src.moveBy(0,0);
	src.rBottom()++;*/
//    	painter.drawPixmap(p,*background,src);
	painter.drawImage(p,*background);
    }else{
    	wallpaper->repaint(true);
    }

    // draw title text
    painter.setFont(options()->font(isActive(), false));
    painter.setPen(options()->color(KDecoration::ColorFont, isActive()));
    painter.drawText(title.x() + FRAMESIZE+BUTTONSIZE*2, title.y(),
                     title.width() - FRAMESIZE * 2-BUTTONSIZE*4, title.height(),
                     ExampleFactory::titleAlign() | AlignVCenter,
                     caption());

    // draw frame
    if ((isActive()&& active.frame)||(!isActive() && inactive.frame))
    {
    	group = options()->colorGroup(KDecoration::ColorFrame, isActive());

    	// outline the frame
    	painter.setPen(group.dark());
    	painter.drawRect(widget()->rect());
    }
}

//////////////////////////////////////////////////////////////////////////////
// resizeEvent()
// -------------
// Window is being resized

void ExampleClient::resizeEvent(QResizeEvent *)
{
    if (widget()->isShown()) {
        QRegion region = widget()->rect();
//        region = region.subtract(titlebar_->geometry());
    widget()->erase(region);
    }
}

void ExampleClient::moveEvent(QMoveEvent *)
{
    if (widget()->isShown()) {
        QRegion region = widget()->rect();
//        region = region.subtract(titlebar_->geometry());
    widget()->erase(region);
    }	
}

//////////////////////////////////////////////////////////////////////////////
// showEvent()
// -----------
// Window is being shown

void ExampleClient::showEvent(QShowEvent *)
{
    widget()->repaint();
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

//////////////////////////////////////////////////////////////////////////////
// menuButtonPressed()
// -------------------
// Menu button was pressed (popup the menu)

void ExampleClient::menuButtonPressed()
{
    if (button[ButtonMenu]) {
        QPoint p(button[ButtonMenu]->rect().bottomLeft().x(),
                 button[ButtonMenu]->rect().bottomLeft().y());
        KDecorationFactory* f = factory();
        showWindowMenu(button[ButtonMenu]->mapToGlobal(p));
        if (!f->exists(this)) return; // decoration was destroyed
        button[ButtonMenu]->setDown(false);
    }
}

#include "crystalclient.moc"
