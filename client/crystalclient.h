#ifndef CRYSTALCLIENT_H
#define CRYSTALCLIENT_H

#include <qlayout.h>
#include <GL/glx.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>
#include <qtimer.h>
// #include <qptrlist.h>
#include <X11/Xlib.h>

class QSpacerItem;
class QPoint;

class CrystalClient;
class CrystalFactory;
class CrystalButton;
class QImageHolder;

class ButtonImage;
class GLFont;

extern CrystalFactory *factory;


#define TOP_LEFT 1
#define TOP_RIGHT 2
#define BOT_LEFT 4
#define BOT_RIGHT 8




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
	
	bool initGL();
	bool setupGL(Window winId);
public:
	QImageHolder *image_holder;
	GLFont *gl_font;
	GLXContext glxcontext;
	
	int titlesize;
	bool hovereffect,tintButtons,fadeButtons;
	QColor buttonColor;
	int borderwidth;
	bool textshadow,antialiaseCaption;
	bool trackdesktop;
	int roundCorners;
	int repaintMode,repaintTime;
	bool useRefraction,useLighting,animateActivate;
	double iorActive,iorInactive;
	int textureSize;
	int buttontheme;
	int brightness;
	QColor activeColor,inactiveColor;
	
	ButtonImage *buttonImages[ButtonImageCount];
// 	QPtrList <CrystalClient> clients;
	
	static QImage convertToGLFormat(const QImage &);
private:
    bool readConfig();
	void CreateButtonImages();
private:
    static bool initialized_;
	bool glInitialized;
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
	
	void renderText(QString str);
private:
    void addButtons(QBoxLayout* layout, const QString& buttons);
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
	
	void ClientWindows(Window* frame,Window* wrapper,Window* client);
public slots:
	void Repaint();
private slots:
    void maxButtonPressed();
    void minButtonPressed();
    void shadeButtonPressed();
	void aboveButtonPressed();
	void belowButtonPressed();
    void menuButtonPressed();
	
	void keepAboveChange( bool );
	void keepBelowChange( bool );
	void menuPopUp();
	void animate();
private:
    CrystalButton *button[ButtonTypeCount];
    QSpacerItem *titlebar_;
	QGridLayout *mainlayout;
    QTimer timer;
	
	QTimer animationtimer;
	double animation;
	
public:
	bool FullMax;
};


#endif // EXAMPLECLIENT_H
