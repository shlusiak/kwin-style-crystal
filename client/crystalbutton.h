#ifndef _CRYSTALBUTTON_INCLUDED_
#define _CRYSTALBUTTON_INCLUDED_


#include <qbutton.h>
#include <qdragobject.h>



#define DECOSIZE 14
#define BUTTONSIZE 18
#define FRAMESIZE 2

class CrystalClient;


class ButtonImage
{
public:
	QImage *normal,*hovered,*pressed;
	
	ButtonImage(const QRgb *d_normal=NULL,bool blend=true,QColor color=::factory->buttonColor);
	~ButtonImage();
	
	void SetNormal(const QRgb *d_normal,bool blend=true,QColor color=::factory->buttonColor);
	void SetHovered(const QRgb *d_hovered=NULL,bool blend=true,QColor color=::factory->buttonColor);
	void SetPressed(const QRgb *d_pressed=NULL,bool blend=true,QColor color=::factory->buttonColor);
	void reset();
	
private:
	QImage CreateImage(const QRgb *data,bool blend,QColor color);
};



class CrystalButton : public QButton
{
public:
    CrystalButton(CrystalClient *parent=0, const char *name=0,
                  const QString &tip=NULL,
                  ButtonType type=ButtonHelp,
                  ButtonImage *vimage=NULL);
    ~CrystalButton();

    void setBitmap(ButtonImage *newimage);
    QSize sizeHint() const;
    int lastMousePress() const { return lastmouse_; }
    void reset() { repaint(false); }
	void setFirstLast(bool vfirst,bool vlast) { first|=vfirst; last|=vlast; }
	void resetSize(bool FullSize);
private:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void drawButton(QPainter *painter);
    
    int buttonSizeH() const;
	int buttonSizeV() const;

private:
	bool first,last;
	bool hover;
    CrystalClient *client_;
    ButtonType type_;
    ButtonImage *image;
    int lastmouse_;
};



#endif
