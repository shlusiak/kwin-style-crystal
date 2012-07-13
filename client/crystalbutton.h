#ifndef _CRYSTALBUTTON_INCLUDED_
#define _CRYSTALBUTTON_INCLUDED_


#include <qbutton.h>
#include "crystalclient.h"
#include <GL/gl.h>



#define DECOSIZE 14
#define BUTTONSIZE 18
#define FRAMESIZE 2

class CrystalClient;


class ButtonImage
{
public:
	QImage *normal,*hovered,*pressed;
	GLuint t_normal,t_hovered,t_pressed;
	
	ButtonImage(const QRgb *d_normal=NULL,bool blend=true,QColor color=::factory->buttonColor);
	~ButtonImage();
	
	void SetNormal(QImage image);
	void SetNormal(const QRgb *d_normal,bool blend=true,QColor color=::factory->buttonColor);
	void SetHovered(const QRgb *d_hovered=NULL,bool blend=true,QColor color=::factory->buttonColor);
	void SetPressed(const QRgb *d_pressed=NULL,bool blend=true,QColor color=::factory->buttonColor);
	void reset();
	void resetTextures();
	void check();
	void activate(QImage *img,GLuint texture);
	
private:
	QImage CreateImage(const QRgb *data,bool blend,QColor color);
	GLuint CreateTexture(QImage *img);
};



class CrystalButton : public QObject,public QSpacerItem
{
	Q_OBJECT
public:
    CrystalButton(CrystalClient *parent=0, const char *name=0,
                  const QString &tip=NULL,
                  ButtonType type=ButtonHelp,
                  ButtonImage *vimage=NULL);
    ~CrystalButton();

    void setBitmap(ButtonImage *newimage);
    int lastMousePress() const { return lastmouse_; }
	void resetSize(bool FullSize);

    void drawButton(double alpha);
	
	bool isInside(QPoint point);

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
	bool hover;
    CrystalClient *client_;
    ButtonType type_;
    ButtonImage *image,menuimage;

    int lastmouse_;
	
	
signals:
	void clicked();
	void pressed();
};



#endif
