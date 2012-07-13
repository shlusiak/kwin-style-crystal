#ifndef CRYSTALCLIENT_H
#define CRYSTALCLIENT_H

#include <qlayout.h>
#include <GL/glx.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>
#include <qtimer.h>
#include <qptrlist.h>


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



inline void glColorQ(const QColor x,const double alpha=1.0) 
{ glColor4ub(x.red(),x.green(),x.blue(),(unsigned char)(alpha*255.0)); }



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
	bool makeCurrent(Window winId=0);
public:
	QImageHolder *image_holder;
	GLFont *gl_font;
	
	int titlesize;
	bool hovereffect,fadeInactiveButtons,animateHover;
	QColor normalColorNormal,normalColorHovered,normalColorPressed;
	QColor closeColorNormal,closeColorHovered,closeColorPressed;
	int borderwidth;
	bool textshadow,antialiaseCaption;
	bool scrollWindows;
	bool trackdesktop;
	int roundCorners;
	int repaintMode,repaintTime;
	bool useRefraction,useLighting,animateActivate,useTransparency;
	double iorActive,iorInactive;
	int textureSize;
	int buttontheme;
	int brightness;
	QColor activeColor,inactiveColor;
	GLXContext glxcontext;

	ButtonImage *buttonImages[ButtonImageCount];
	QPtrList <CrystalClient> clients;
	
	static QImage convertToGLFormat(const QImage &);
private:
    bool readConfig();
	void CreateButtonImages();
private:
    static bool initialized_;
	bool glInitialized;
    static Qt::AlignmentFlags titlealign_;
	Window dummyWindow;
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
	void startAnimation();
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

	void ClientWindows(Window* frame,Window* wrapper,Window *client);
	
	void renderSide(const int bt,const int bl, const int bb, const int br,const double ior,const int side);
	void renderLighting(const bool top,const bool left,const bool bottom,const bool right);
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
	void animate();
private:
    CrystalButton *button[ButtonTypeCount];
    QSpacerItem *titlebar_;
	QGridLayout *mainlayout;
    QTimer timer;
	
	QTimer animationtimer;
	double animation;
	int bl,bt,br,bb;
	
public:
	bool FullMax;
};


#endif // EXAMPLECLIENT_H
