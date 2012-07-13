#ifndef _CRYSTALBUTTON_INCLUDED_
#define _CRYSTALBUTTON_INCLUDED_


#include "crystalclient.h"
#include <GL/gl.h>



#define DECOSIZE 14
#define BUTTONSIZE 18
#define FRAMESIZE 2

class CrystalClient;


enum BUTTONIMAGETYPE
{ ButtonNormal,ButtonHovered,ButtonPressed };

class ButtonImage
{
public:
	ButtonImage();
	~ButtonImage();
	
	void SetNormal(QImage image);
	void SetNormal(const QRgb *d_normal,QColor colornormal=::factory->normalColorNormal,QColor colorhovered=::factory->normalColorHovered,QColor colorpressed=::factory->normalColorPressed);
	void SetHovered(const QRgb *d_hovered=NULL,QColor color=::factory->normalColorHovered);
	void SetPressed(const QRgb *d_pressed=NULL,QColor color=::factory->normalColorHovered);
	void reset();
	
	void drawNormal(QRect r,double alpha);
	void drawHovered(QRect r,double alpha);
	void drawPressed(QRect r,double alpha);
	bool hasNormal() { return (t_normal!=0); }
	bool hasHovered() { return (t_hovered!=0); }
	bool hasPressed() { return (t_pressed!=0); }
	
private:
	QColor color_normal,color_hovered,color_pressed;
	GLuint t_normal,t_hovered,t_pressed;
	
	void draw(QRect r);
	QImage CreateImage(const QRgb *data);
	GLuint CreateTexture(QImage img);
};



class CrystalButton : public QObject
{
	Q_OBJECT
public:
    CrystalButton(CrystalClient *parent=0, const char *name=0,
                  const QString &tip=NULL,
                  ButtonType type=ButtonHelp,
                  ButtonImage *vimage=NULL);
    ~CrystalButton();

    void setBitmap(ButtonImage *newimage);
    int lastMousePress() const { return lastmouse; }
	void resetSize(bool FullSize);

    void drawButton(double alpha);
	QLayoutItem* layout() { return spacer; }
	QRect geometry() { return layout()->geometry(); }
	
	bool isInside(QPoint point);
	bool isHover() { return hover; }
	bool animate();
	
    void enterEvent();
    void leaveEvent();
    bool mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
private:
    int buttonSizeH() const;
	int buttonSizeV() const;

	bool handleMouseButton(int button);
	
	void repaint();
private:
	QSpacerItem *spacer;
	bool hover;
    CrystalClient *client_;
    ButtonType type_;
    ButtonImage *image,menuimage;
	double animation;

    int _lastmouse,lastmouse;
	
	
signals:
	void clicked();
	void pressed();
};



#endif
