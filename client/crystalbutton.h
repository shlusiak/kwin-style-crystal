#ifndef _CRYSTALBUTTON_INCLUDED_
#define _CRYSTALBUTTON_INCLUDED_

#include <qbutton.h>

class CrystalClient;
class ButtonImage;

class CrystalButton : public QButton
{
	Q_OBJECT

public:
    CrystalButton(CrystalClient *parent=0, const char *name=0,
                  const QString &tip=NULL,
                  ButtonType type=ButtonHelp,
                  ButtonImage *vimage=NULL);
    virtual ~CrystalButton();

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

private slots:
	void animate();

private:
	QTimer animation_timer;
	bool first,last;
	bool hover;
	float animation;
    CrystalClient *client_;
    ButtonType type_;
    ButtonImage *image;
    int lastmouse_;
};


#endif
